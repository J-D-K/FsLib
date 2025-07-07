#include "File.hpp"
#include "error.hpp"
#include "file_functions.hpp"
#include "fslib.hpp"
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


fslib::File::File(const fslib::Path &filePath, uint32_t openFlags, int64_t fileSize)
{
    File::open(filePath, openFlags, fileSize);
}

fslib::File::File(fslib::File &&file) noexcept
{
    // Just let the operator do the work.
    *this = std::move(file);
}

fslib::File &fslib::File::operator=(fslib::File &&file) noexcept
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
        return;
    }

    // Small correction just in case.
    if ((openFlags & FsOpenMode_Append) && !(openFlags & FsOpenMode_Write))
    {
        openFlags |= FsOpenMode_Write;
    }

    FsFileSystem *filesystem;
    if (!fslib::get_file_system_by_device_name(filePath.get_device_name(), &filesystem))
    {
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

    if (error::occurred(fsFsOpenFile(filesystem, filePath.get_path(), openFlags, &m_fileHandle)))
    {
        return;
    }

    if (error::occurred(fsFileGetSize(&m_fileHandle, &m_streamSize)))
    {
        return;
    }

    // Save flags and set offset.
    m_openFlags = openFlags;
    m_offset = (m_openFlags & FsOpenMode_Append) ? m_streamSize : 0;

    m_isOpen = true;
}

void fslib::File::close() noexcept
{
    if (m_isOpen)
    {
        fsFileClose(&m_fileHandle);
        m_isOpen = false;
    }
}

bool fslib::File::is_open() const noexcept
{
    return m_isOpen;
}

ssize_t fslib::File::read(void *buffer, size_t bufferSize) noexcept
{
    if (!m_isOpen || !File::is_open_for_reading())
    {
        return -1;
    }

    uint64_t bytesRead = 0;
    if (error::occurred(fsFileRead(&m_fileHandle, m_offset, buffer, bufferSize, FsReadOption_None, &bytesRead)) ||
        bytesRead > bufferSize)
    {
        // This is a correction like on 3DS, but I'm not sure Switch has the same problem.
        bytesRead = m_offset + bufferSize > static_cast<uint64_t>(m_streamSize) ? m_streamSize - m_offset : bufferSize;
    }

    m_offset += bytesRead;

    return bytesRead;
}

bool fslib::File::read_line(char *lineOut, size_t lineLength) noexcept
{
    if (!m_isOpen || !File::is_open_for_reading())
    {
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
        if (nextCharacter == '\n')
        {
            return true;
        }

        // Add it to lineOut
        lineOut[i] = nextCharacter;
    }

    // Just assume something went wrong?
    return false;
}

bool fslib::File::read_line(std::string &lineOut)
{
    if (!m_isOpen || !File::is_open_for_reading())
    {
        return false;
    }

    lineOut.clear();

    signed char next = 0x00;
    while (!Stream::end_of_stream() && (next = File::get_byte()) != -1 && next != '\n')
    {
        lineOut += next;
    }
    return true;
}

signed char fslib::File::get_byte() noexcept
{
    if (!m_isOpen || !File::is_open_for_reading())
    {
        return -1;
    }

    char byte = 0x00;
    uint64_t bytesRead = 0;
    if (error::occurred(fsFileRead(&m_fileHandle, m_offset++, &byte, 1, FsReadOption_None, &bytesRead)))
    {
        return -1;
    }

    return byte;
}

ssize_t fslib::File::write(const void *buffer, size_t bufferSize) noexcept
{
    if (!m_isOpen || !File::is_open_for_writing() || !File::resize_if_needed(bufferSize))
    {
        return -1;
    }

    if (error::occurred(fsFileWrite(&m_fileHandle, m_offset, buffer, bufferSize, FsWriteOption_None)))
    {
        return -1;
    }

    // There's no real way to verify this was completely successful on Switch
    m_offset += bufferSize;
    return bufferSize;
}

bool fslib::File::writef(const char *format, ...) noexcept
{
    char vaBuffer[VA_BUFFER_SIZE] = {0};

    std::va_list vaList;
    va_start(vaList, format);
    vsnprintf(vaBuffer, VA_BUFFER_SIZE, format, vaList);
    va_end(vaList);

    return File::write(vaBuffer, std::char_traits<char>::length(vaBuffer)) != -1;
}

bool fslib::File::put_byte(char byte) noexcept
{
    // L o L
    if (!m_isOpen || !File::is_open_for_writing() || !File::resize_if_needed(1))
    {
        return false;
    }

    // I'm not calling another function for 1 byte.
    if (error::occurred(fsFileWrite(&m_fileHandle, m_offset++, &byte, 1, FsWriteOption_None)))
    {
        return false;
    }
    return true;
}

fslib::File &fslib::File::operator<<(const char *string) noexcept
{
    File::write(string, std::char_traits<char>::length(string));
    return *this;
}

fslib::File &fslib::File::operator<<(const std::string &string) noexcept
{
    File::write(string.c_str(), string.length());
    return *this;
}

bool fslib::File::flush() noexcept
{
    if (!m_isOpen || !File::is_open_for_writing())
    {
        return false;
    }

    if (error::occurred(fsFileFlush(&m_fileHandle)))
    {
        return false;
    }

    return true;
}

bool fslib::File::resize_if_needed(size_t bufferSize) noexcept
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

    if (error::occurred(fsFileSetSize(&m_fileHandle, newFileSize)))
    {
        return false;
    }
    // Update size
    m_streamSize = newFileSize;
    return true;
}
