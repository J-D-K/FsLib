#include "File.hpp"
#include "errorCommon.h"
#include "file_functions.hpp"
#include "fslib.hpp"
#include "string.hpp"
#include <cstdarg>
#include <cstring>
#include <string>

namespace
{
    // Buffer size for writef.
    constexpr size_t VA_BUFFER_SIZE = 0x1000;

    // These two strings are use multiple times in the reading and writing functions for errors.
    constexpr std::string_view ERROR_NOT_OPEN_FOR_READING = "Error: File not open for reading.";
    constexpr std::string_view ERROR_NOT_OPEN_FOR_WRITING = "Error: File not open for writing.";
} // namespace

extern std::string g_fslibErrorString;

fslib::File::File(const fslib::Path &filePath, uint32_t openFlags, int64_t fileSize)
{
    File::open(filePath, openFlags, fileSize);
}

fslib::File::File(fslib::File &&file)
{
    // Just let the operator do the work.
    *this = std::move(file);
}

fslib::File &fslib::File::operator=(fslib::File &&file)
{
    // Steal the parent stuff.
    m_offset = file.m_offset;
    file.m_offset = 0;

    m_streamSize = file.m_streamSize;
    file.m_streamSize = 0;

    m_isOpen = file.m_isOpen;
    file.m_isOpen = false;

    // Steal the handle/service.
    std::memcpy(&m_fileHandle, &file.m_fileHandle, sizeof(FsFile));
    std::memset(&file.m_fileHandle, 0x00, sizeof(FsFile));

    // Steal the flags.
    m_openFlags = file.m_openFlags;
    file.m_openFlags = 0;

    return *this;
}

fslib::File::~File()
{
    File::close();
}

void fslib::File::open(const fslib::Path &filePath, uint32_t openFlags, int64_t fileSize)
{
    // So this class can be reused.
    File::close();

    if (!filePath.is_valid())
    {
        g_fslibErrorString = ERROR_INVALID_PATH;
        return;
    }

    // Small correction just in case.
    if ((openFlags & FsOpenMode_Append) && !(openFlags & FsOpenMode_Write))
    {
        openFlags |= FsOpenMode_Write;
    }

    FsFileSystem *fileSystem;
    if (!fslib::get_file_system_by_device_name(filePath.get_device_name(), &fileSystem))
    {
        g_fslibErrorString = ERROR_DEVICE_NOT_FOUND;
        return;
    }

    if ((openFlags & FsOpenMode_Create) && fslib::file_exists(filePath) && !fslib::delete_file(filePath))
    {
        // Previous calls should set error. Let's not worry about it.
        return;
    }

    if ((openFlags & FsOpenMode_Create) && !fslib::create_file(filePath, fileSize))
    {
        return;
    }

    // Need to remove FsLib's added flag before trying to actually open the file.
    if (openFlags & FsOpenMode_Create)
    {
        openFlags &= ~FsOpenMode_Create;
    }

    Result fsError = fsFsOpenFile(fileSystem, filePath.get_path(), openFlags, &m_fileHandle);
    if (R_FAILED(fsError))
    {
        g_fslibErrorString = string::get_formatted_string("Error opening file: 0x%X.", fsError);
        return;
    }

    fsError = fsFileGetSize(&m_fileHandle, &m_streamSize);
    if (R_FAILED(fsError))
    {
        g_fslibErrorString = string::get_formatted_string("Error getting file's size: 0x%X.", fsError);
        return;
    }
    // Save flags and set offset.
    m_openFlags = openFlags;
    m_offset = (m_openFlags & FsOpenMode_Append) ? m_streamSize : 0;
    // We're good?
    m_isOpen = true;
}

void fslib::File::close()
{
    if (m_isOpen)
    {
        fsFileClose(&m_fileHandle);
        m_isOpen = false;
    }
}

bool fslib::File::is_open() const
{
    return m_isOpen;
}

ssize_t fslib::File::read(void *buffer, size_t bufferSize)
{
    if (!m_isOpen || !File::is_open_for_reading())
    {
        g_fslibErrorString = ERROR_NOT_OPEN_FOR_READING;
        return -1;
    }

    uint64_t bytesRead = 0;
    Result fsError = fsFileRead(&m_fileHandle, m_offset, buffer, bufferSize, FsReadOption_None, &bytesRead);
    if (R_FAILED(fsError) || bytesRead > bufferSize) // Carrying over that last one from 3DS...
    {
        g_fslibErrorString = string::get_formatted_string("Error reading from file: 0x%X.", fsError);
        // I don't think this is a problem on Switch like on 3DS, but just in case.
        bytesRead = m_offset + static_cast<int64_t>(bufferSize) > m_streamSize ? m_streamSize - m_offset : bufferSize;
    }
    m_offset += bytesRead;
    return bytesRead;
}

bool fslib::File::read_line(char *lineOut, size_t lineLength)
{
    if (!m_isOpen || !File::is_open_for_reading())
    {
        g_fslibErrorString = ERROR_NOT_OPEN_FOR_READING;
        return false;
    }

    signed char nextCharacter = 0x00;
    // Loop within length. I might want to revise this later.
    for (size_t i = 0; i < lineLength; i++)
    {
        if (Stream::end_of_stream() || (nextCharacter = File::get_byte()) == -1)
        {
            // End of file before new line or character read was bad.
            return false;
        }

        // Line break was hit. Return line without it. I hate that some implementations include the line breaks...
        if (nextCharacter == '\n' || nextCharacter == '\r')
        {
            return true;
        }

        // Add it to lineOut
        lineOut[i] = nextCharacter;
    }

    // Just assume something went wrong?
    return false;
}

signed char fslib::File::get_byte()
{
    if (!m_isOpen || !File::is_open_for_reading())
    {
        g_fslibErrorString = ERROR_NOT_OPEN_FOR_READING;
        return -1;
    }

    // I don't want to call another function just for this.
    char character = 0x00;
    uint64_t bytesRead = 0;
    Result fsError = fsFileRead(&m_fileHandle, m_offset++, &character, 1, FsReadOption_None, &bytesRead);
    if (R_FAILED(fsError) || bytesRead == 0)
    {
        g_fslibErrorString = string::get_formatted_string("Error reading byte from file: 0x%X.", fsError);
        return -1;
    }

    return character;
}

ssize_t fslib::File::write(const void *buffer, size_t bufferSize)
{
    if (!m_isOpen || !File::is_open_for_writing() || !File::resize_if_needed(bufferSize))
    {
        g_fslibErrorString = ERROR_NOT_OPEN_FOR_WRITING;
        return -1;
    }

    Result fsError = fsFileWrite(&m_fileHandle, m_offset, buffer, bufferSize, FsWriteOption_None);
    if (R_FAILED(fsError))
    {
        g_fslibErrorString = string::get_formatted_string("Error writing to file: 0x%X.", fsError);
        return -1;
    }
    // There's no real way to verify this was successful on Switch
    m_offset += bufferSize;
    return bufferSize;
}

bool fslib::File::writef(const char *format, ...)
{
    char vaBuffer[VA_BUFFER_SIZE] = {0};

    std::va_list vaList;
    va_start(vaList, format);
    vsnprintf(vaBuffer, VA_BUFFER_SIZE, format, vaList);
    va_end(vaList);

    return File::write(vaBuffer, std::char_traits<char>::length(vaBuffer));
}

bool fslib::File::put_byte(char byte)
{
    // L o L
    if (!m_isOpen || !File::is_open_for_writing() || !File::resize_if_needed(1))
    {
        g_fslibErrorString = ERROR_NOT_OPEN_FOR_WRITING;
        return false;
    }

    // I'm not calling another function for 1 byte.
    Result fsError = fsFileWrite(&m_fileHandle, m_offset++, &byte, 1, FsWriteOption_None);
    if (R_FAILED(fsError))
    {
        g_fslibErrorString =
            string::get_formatted_string("Error writing a single, tiny, miniscule byte to file: 0x%X.", fsError);
        return false;
    }
    return true;
}

fslib::File &fslib::File::operator<<(const char *string)
{
    File::write(string, std::char_traits<char>::length(string));
    return *this;
}

fslib::File &fslib::File::operator<<(const std::string &string)
{
    File::write(string.c_str(), string.length());
    return *this;
}

bool fslib::File::flush()
{
    if (!m_isOpen || !File::is_open_for_writing())
    {
        g_fslibErrorString = ERROR_NOT_OPEN_FOR_WRITING;
        return false;
    }

    Result fsError = fsFileFlush(&m_fileHandle);
    if (R_FAILED(fsError))
    {
        g_fslibErrorString = string::get_formatted_string("Error flushing file: 0x%X.", fsError);
        return false;
    }
    return true;
}

bool fslib::File::resize_if_needed(size_t bufferSize)
{
    // Size remaining in file.
    size_t spaceRemaining = m_streamSize - m_offset;

    // Resize isn't needed. Buffer will fit.
    if (bufferSize <= spaceRemaining)
    {
        return true;
    }

    // Calculate new file size needed to fit buffer.
    int64_t newFileSize = m_offset + bufferSize;

    Result fsError = fsFileSetSize(&m_fileHandle, newFileSize);
    if (R_FAILED(fsError))
    {
        g_fslibErrorString = string::get_formatted_string("Error resizing file to fit buffer: 0x%X.", fsError);
        return false;
    }
    // Update size
    m_streamSize = newFileSize;
    // Yay.
    return true;
}
