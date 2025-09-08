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

extern void print(const char *format, ...);

fslib::File::File(const fslib::Path &filePath, uint32_t openFlags, int64_t fileSize) noexcept
{
    File::open(filePath, openFlags, fileSize);
}

fslib::File::File(fslib::File &&file)
    : Stream(std::move(file))
    , m_handle(file.m_handle)
    , m_flags(file.m_flags)
{
    file.m_handle = {0};
    file.m_flags  = 0;
}

fslib::File &fslib::File::operator=(fslib::File &&file) noexcept
{
    // Steal the parent stuff.
    m_offset     = file.m_offset;
    m_streamSize = file.m_streamSize;
    m_isOpen     = file.m_isOpen;
    m_flags      = file.m_flags;
    m_handle     = file.m_handle;

    file.m_offset     = 0;
    file.m_streamSize = 0;
    file.m_isOpen     = false;
    file.m_flags      = 0;
    file.m_handle     = {0};
    return *this;
}

fslib::File::~File() noexcept { File::close(); }

void fslib::File::open(const fslib::Path &filePath, uint32_t openFlags, int64_t fileSize) noexcept
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

    const bool openError = error::occurred(fsFsOpenFile(filesystem, path, openFlags, &m_handle));
    const bool sizeError = !openError && error::occurred(fsFileGetSize(&m_handle, &m_streamSize));
    if (openError || sizeError) { return; }

    m_flags  = openFlags;
    m_offset = openAppend ? m_streamSize : 0;

    m_isOpen = true;
}

void fslib::File::close() noexcept
{
    if (!m_isOpen) { return; }
    fsFileClose(&m_handle);
    m_isOpen = false;
}

bool fslib::File::is_open() const noexcept { return m_isOpen; }

ssize_t fslib::File::read(void *buffer, uint64_t bufferSize) noexcept
{
    if (!File::is_open_for_reading()) { return -1; }

    uint64_t bytesRead{};
    const bool readError     = error::occurred(fsFileRead(&m_handle, m_offset, buffer, bufferSize, 0, &bytesRead));
    const bool readSizeCheck = bytesRead <= bufferSize; // This check is in place from the 3DS.
    if (readError || !readSizeCheck)
    {
        // This will signal failure.
        return -1;
    }
    m_offset += bytesRead;
    return bytesRead;
}

bool fslib::File::read_line(char *lineOut, size_t lineLength) noexcept
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

bool fslib::File::read_line(std::string &lineOut) noexcept
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

signed char fslib::File::get_byte() noexcept
{
    if (!File::is_open_for_reading()) { return -1; }

    char byte{};
    uint64_t bytesRead{};
    const bool streamEnd = Stream::end_of_stream();
    const bool readError = !streamEnd && error::occurred(fsFileRead(&m_handle, m_offset++, &byte, 1, 0, &bytesRead));
    if (readError) { return -1; }
    return byte;
}

ssize_t fslib::File::write(const void *buffer, uint64_t bufferSize) noexcept
{
    const bool openForWrite = File::is_open_for_writing();
    const bool resized      = File::resize_if_needed(bufferSize);
    if (!openForWrite || !resized) { return -1; }

    const bool writeError = error::occurred(fsFileWrite(&m_handle, m_offset, buffer, bufferSize, 0));
    if (writeError) { return -1; }
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
    const bool openForWrite = File::is_open_for_writing();
    const bool resized      = File::resize_if_needed(1); // This is funny.
    if (!openForWrite || !resized) { return false; }

    // I'm not calling another function for 1 byte.
    const bool writeError = error::occurred(fsFileWrite(&m_handle, m_offset++, &byte, 1, 0));
    if (writeError) { return false; }
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

void fslib::File::seek(int64_t offset, Stream::Origin origin)
{
    switch (origin)
    {
        case Stream::Origin::BEGINNING: m_offset = offset; break;
        case Stream::Origin::CURRENT: m_offset += offset; break;
        case Stream::Origin::END: m_offset = m_streamSize + offset; break;
    }

    if (m_offset < 0) { m_offset = 0; }
    File::resize_if_needed(0);
}

bool fslib::File::flush() noexcept
{
    if (!File::is_open_for_writing()) { return false; }

    const bool flushError = error::occurred(fsFileFlush(&m_handle));
    if (flushError) { return false; }
    return true;
}

bool fslib::File::resize_if_needed(int64_t bufferSize)
{
    if (!File::is_open_for_writing()) { return false; }

    const bool offsetOOB      = m_offset > m_streamSize;
    const bool bufferTooLarge = m_offset + bufferSize > m_streamSize;
    if (!offsetOOB && !bufferTooLarge) { return true; }

    int64_t newFileSize{};
    if (offsetOOB) { newFileSize = m_offset; }
    else if (bufferTooLarge) { newFileSize = m_offset + bufferSize; }

    const bool resizeError = error::occurred(fsFileSetSize(&m_handle, newFileSize));
    if (resizeError) { return false; }

    m_streamSize = newFileSize;
    return true;
}
