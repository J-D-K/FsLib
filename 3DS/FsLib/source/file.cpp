#include "fslib.hpp"
#include "string.hpp"

#include <array>
#include <cstdarg>

namespace
{
    constexpr size_t VA_BUFFER_SIZE = 0x1000;
}

fslib::File::File(const fslib::Path &filePath, uint32_t openFlags, uint64_t fileSize)
{
    File::open(filePath, openFlags, fileSize);
}

fslib::File::File(fslib::File &&file) { *this = std::move(file); }

fslib::File &fslib::File::operator=(fslib::File &&file)
{
    m_handle    = file.m_handle;
    m_isOpen    = file.m_isOpen;
    m_openFlags = file.m_openFlags;
    m_offset    = file.m_offset;
    m_size      = file.m_size;

    file.m_handle    = 0;
    file.m_isOpen    = false;
    file.m_openFlags = 0;
    file.m_offset    = 0;
    file.m_size      = 0;

    return *this;
}

fslib::File::~File() { File::close(); }

void fslib::File::open(const fslib::Path &filePath, uint32_t openFlags, uint64_t fileSize)
{
    m_isOpen = false;

    FS_Archive archive;
    const bool found = fslib::get_archive_by_device_name(filePath.get_device(), archive);
    if (!found) { return; }

    const bool create  = openFlags & FS_OPEN_CREATE;
    const bool exists  = fslib::file_exists(filePath);
    const bool deleted = create && exists && fslib::delete_file(filePath);
    if (exists && create && !deleted) { return; }

    const bool created = create && fslib::create_file(filePath, fileSize);
    if (!created) { return; }

    // Save openFlags without FS_OPEN_CREATE if it's there so extdata works correctly.
    m_openFlags = (openFlags & ~FS_OPEN_CREATE);

    uint64_t *size       = reinterpret_cast<uint64_t *>(&m_size);
    const bool openError = error::libctru(FSUSER_OpenFile(&m_handle, archive, filePath.get_fs_path(), m_openFlags, 0));
    const bool sizeError = !openError && error::libctru(FSFILE_GetSize(m_handle, size));
    if (openError || sizeError) { return; }

    // I added FS_OPEN_APPEND to FsLib. This isn't normally part of ctrulib/3DS.
    m_offset = m_openFlags & FS_OPEN_APPEND ? m_size : 0;
    m_isOpen = true;
}

void fslib::File::close()
{
    if (m_isOpen) { FSFILE_Close(m_handle); }
}

bool fslib::File::is_open() const { return m_isOpen; }

uint64_t fslib::File::tell() const { return m_offset; }

uint64_t fslib::File::get_size() const { return m_size; }

bool fslib::File::end_of_file() const { return m_offset >= m_size; }

void fslib::File::seek(int64_t offset, File::Origin origin)
{
    switch (origin)
    {
        case File::BEGINNING: m_offset = offset; break;
        case File::CURRENT: m_offset += offset; break;
        case File::END: m_offset = m_size + offset; break;
    }

    if (m_offset < 0) { m_offset = 0; }
    else { File::resize_if_needed(0); }
}

ssize_t fslib::File::read(void *buffer, size_t bufferSize)
{
    if (!File::is_open_for_reading()) { return -1; }

    uint32_t bytesRead{};
    const uint64_t offset = m_offset;
    const bool readError  = error::libctru(FSFILE_Read(m_handle, &bytesRead, offset, buffer, bufferSize));
    if (readError) { bytesRead = m_offset + bufferSize > m_size ? m_size - m_offset : bufferSize; }

    m_offset += bytesRead;
    return bytesRead;
}

bool fslib::File::read_line(char *buffer, size_t bufferSize)
{
    if (!File::is_open_for_reading()) { return false; }

    for (size_t i = 0; i < bufferSize; i++)
    {
        // To do: Look at this better sometime.
        const signed char nextByte = File::get_byte();
        const bool goodByte        = nextByte != -1;
        const bool lineBreak       = goodByte && (nextByte == '\r' || nextByte == '\n');
        if (!goodByte || lineBreak) { break; }

        buffer[i] = nextByte;
    }

    return true;
}

bool fslib::File::read_line(std::string &line)
{
    if (!File::is_open_for_reading()) { return false; }

    line.clear();

    signed char nextByte{};
    while ((nextByte = File::get_byte()) != -1)
    {
        if (nextByte == '\n' || nextByte == '\r') { return true; }
        line += nextByte;
    }

    return false;
}

signed char fslib::File::get_byte()
{
    if (!File::is_open_for_reading()) { return -1; }

    char byteRead{};
    uint32_t bytesRead{};
    const bool readError = error::libctru(FSFILE_Read(m_handle, &bytesRead, m_offset, &byteRead, 1));
    if (readError) { return -1; }

    return byteRead;
}

ssize_t fslib::File::write(const void *buffer, size_t bufferSize)
{
    if (!File::is_open_for_writing() || !File::resize_if_needed(bufferSize)) { return -1; }

    uint32_t bytesWritten{};
    const bool writeError = error::libctru(FSFILE_Write(m_handle, &bytesWritten, m_offset, buffer, bufferSize, 0));
    if (writeError) { return -1; }

    m_offset += bytesWritten;
    return bytesWritten;
}

bool fslib::File::writef(const char *format, ...)
{
    std::array<char, VA_BUFFER_SIZE> vaBuffer{0};

    std::va_list vaList{};
    va_start(vaList, format);
    std::vsnprintf(vaBuffer.data(), VA_BUFFER_SIZE, format, vaList);
    va_end(vaList);

    const int length = std::char_traits<char>::length(vaBuffer.data());
    return File::write(vaBuffer.data(), length) == length;
}

fslib::File &fslib::File::operator<<(const char *string)
{
    const int size = std::char_traits<char>::length(string);
    File::write(string, size);

    return *this;
}

fslib::File &fslib::File::operator<<(const std::string &string)
{
    File::write(string.c_str(), string.length());

    return *this;
}

bool fslib::File::put_byte(char byte)
{
    if (!File::is_open_for_writing() || !File::resize_if_needed(1)) { return false; }

    uint32_t bytesWritten{};
    const bool writeError = error::libctru(FSFILE_Write(m_handle, &bytesWritten, m_offset++, &byte, 1, 0));
    if (writeError) { return false; }

    return true;
}

bool fslib::File::flush()
{
    if (!File::is_open_for_writing()) { return false; }

    const bool flushError = error::libctru(FSFILE_Flush(m_handle));
    if (flushError) { return false; }

    return true;
}

bool fslib::File::resize_if_needed(size_t bufferSize)
{
    if (!File::is_open_for_writing()) { return false; }

    const bool offsetOOB      = m_offset > m_size;
    const bool bufferTooLarge = m_offset + bufferSize > m_size;
    if (!offsetOOB && !bufferTooLarge) { return true; }

    uint64_t newSize{};
    if (offsetOOB) { newSize = m_offset; }
    else { newSize = m_offset + bufferSize; };

    const bool error = error::libctru(FSFILE_SetSize(m_handle, newSize));
    if (error) { return false; }

    m_size = newSize;
    return true;
}
