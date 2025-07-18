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
} // namespace

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
    m_offset     = file.m_offset;
    m_streamSize = file.m_streamSize;
    m_isOpen     = file.m_isOpen;
    m_openFlags  = file.m_openFlags;
    std::memcpy(&m_fileHandle, &file.m_fileHandle, sizeof(FsFile));

    file.m_offset     = 0;
    file.m_streamSize = 0;
    file.m_isOpen     = false;
    file.m_openFlags  = 0;
    std::memset(&file.m_fileHandle, 0x00, sizeof(FsFile));
    return *this;
}

fslib::File::~File() { File::close(); }

void fslib::File::open(const fslib::Path &filePath, uint32_t openFlags, int64_t fileSize)
{
    File::close();

    if (!filePath.is_valid()) { return; }

    FsFileSystem *filesystem{};
    const std::string_view device = filePath.get_device_name();
    const char *path              = filePath.get_path();
    const bool filesystemFound    = fslib::get_file_system_by_device_name(device, &filesystem);
    if (!filesystemFound) { return; }

    const bool openCreate = (openFlags & FsOpenMode_Create); // This is a flag added to fslib to make this easier.
    const bool openWrite  = (openFlags & FsOpenMode_Write);
    const bool openAppend = (openFlags & FsOpenMode_Append);
    if (openAppend && !openWrite) { openFlags |= FsOpenMode_Write; }

    // This is needed in case FsOpenMode_Create is passed and the file already exists.
    const bool fileNeedsDelete = openCreate && fslib::file_exists(filePath);
    const bool fileDeleted     = fileNeedsDelete && fslib::delete_file(filePath);
    if (fileNeedsDelete && !fileDeleted) { return; }

    const bool fileNeedsCreate = openCreate && !fslib::file_exists(filePath);
    const bool fileCreated     = fileNeedsCreate && fslib::create_file(filePath, fileSize);
    if (fileNeedsCreate && !fileCreated) { return; }

    // We need to strip this before passing the flags to the system. It's a helper flag for fslib and the Switch doesn't like
    // it.
    openFlags &= ~FsOpenMode_Create;

    const bool openError = error::occurred(fsFsOpenFile(filesystem, path, openFlags, &m_fileHandle));
    const bool sizeError = !openError && error::occurred(fsFileGetSize(&m_fileHandle, &m_streamSize));
    if (openError || sizeError) { return; }

    m_openFlags = openFlags;
    m_offset    = openAppend ? m_streamSize : 0;

    m_isOpen = true;
}

void fslib::File::close()
{
    if (!m_isOpen) { return; }
    fsFileClose(&m_fileHandle);
    m_isOpen = false;
}

bool fslib::File::is_open() const { return m_isOpen; }

ssize_t fslib::File::read(void *buffer, size_t bufferSize)
{
    if (!File::is_open_for_reading()) { return -1; }

    uint64_t bytesRead{};
    const bool readError     = error::occurred(fsFileRead(&m_fileHandle, m_offset, buffer, bufferSize, 0, &bytesRead));
    const bool readSizeCheck = bytesRead <= bufferSize; // This check is in place from the 3DS.
    if (readError || !readSizeCheck)
    {
        // This will signal failure.
        return -1;
    }
    m_offset += bytesRead;
    return bytesRead;
}

bool fslib::File::read_line(char *lineOut, size_t lineLength)
{
    if (!File::is_open_for_reading()) { return false; }

    signed char nextChar{};
    for (size_t i = 0; i < lineLength; i++)
    {
        const bool goodRead = !Stream::end_of_stream() && (nextChar = File::get_byte()) != -1;
        if (!goodRead) { return false; }
        if (nextChar == '\n') { return true; }

        lineOut[i] = nextChar;
    }
    return false;
}

bool fslib::File::read_line(std::string &lineOut)
{
    if (!File::is_open_for_reading()) { return false; }

    lineOut.clear();

    signed char next{};
    for (int64_t i = 0; i < m_streamSize; i++)
    {
        const bool goodByte = (next = File::get_byte()) != -1;
        if (!goodByte) { return false; }

        const bool lineEnd = next == '\n';
        if (lineEnd) { return true; }
        lineOut += next;
    }
    return false;
}

signed char fslib::File::get_byte()
{
    if (!File::is_open_for_reading()) { return -1; }

    char byte{};
    uint64_t bytesRead{};
    const bool streamEnd = Stream::end_of_stream();
    const bool readError = !streamEnd && error::occurred(fsFileRead(&m_fileHandle, m_offset++, &byte, 1, 0, &bytesRead));
    if (readError) { return -1; }
    return byte;
}

ssize_t fslib::File::write(const void *buffer, size_t bufferSize)
{
    const bool openForWrite = File::is_open_for_writing();
    const bool resized      = openForWrite && File::resize_if_needed(bufferSize);
    if (!openForWrite || !resized) { return -1; }
    // print("Write check");

    const bool writeError = error::occurred(fsFileWrite(&m_fileHandle, m_offset, buffer, bufferSize, 0));
    if (writeError) { return -1; }
    // There's no real way to verify this was completely successful on Switch
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

    return File::write(vaBuffer, std::char_traits<char>::length(vaBuffer)) != -1;
}

bool fslib::File::put_byte(char byte)
{
    const bool openForWrite = File::is_open_for_writing();
    const bool resized      = File::resize_if_needed(1); // This is funny.
    if (!openForWrite || !resized) { return false; }

    // I'm not calling another function for 1 byte.
    const bool writeError = error::occurred(fsFileWrite(&m_fileHandle, m_offset++, &byte, 1, 0));
    if (writeError) { return false; }
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
    if (!File::is_open_for_writing()) { return false; }

    const bool flushError = error::occurred(fsFileFlush(&m_fileHandle));
    if (flushError) { return false; }
    return true;
}

bool fslib::File::resize_if_needed(size_t bufferSize)
{
    const size_t spaceRemaining = m_streamSize - m_offset;
    const int64_t newFileSize   = m_offset + bufferSize; // We may or may not use this.
    if (bufferSize <= spaceRemaining) { return true; }

    const bool setSizeError = error::occurred(fsFileSetSize(&m_fileHandle, newFileSize));
    if (setSizeError) { return false; }

    m_streamSize = newFileSize;
    return true;
}
