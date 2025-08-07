#include "fslib.hpp"
#include "string.hpp"
#include <cstdarg>

namespace
{
    constexpr size_t VA_BUFFER_SIZE = 0x1000;
}

extern std::string g_fslibErrorString;

fslib::File::File(const fslib::Path &filePath, uint32_t openFlags, uint64_t fileSize)
{
    File::open(filePath, openFlags, fileSize);
}

fslib::File::~File()
{
    File::close();
}

void fslib::File::open(const fslib::Path &filePath, uint32_t openFlags, uint64_t fileSize)
{
    // Just in case file is reused.
    m_isOpen = false;

    FS_Archive archive;
    if (!fslib::processDeviceAndPath(filePath, &archive))
    {
        return;
    }

    if ((openFlags & FS_OPEN_CREATE) && fslib::fileExists(filePath) && !fslib::deleteFile(filePath))
    {
        return;
    }

    if ((openFlags & FS_OPEN_CREATE) && !fslib::createFile(filePath, fileSize))
    {
        return;
    }

    // Save openFlags without FS_OPEN_CREATE if it's there so extdata works correctly.
    m_openFlags = (openFlags & ~FS_OPEN_CREATE);

    Result fsError = FSUSER_OpenFile(&m_fileHandle, archive, filePath.getPath(), m_openFlags, 0);
    if (R_FAILED(fsError))
    {
        g_fslibErrorString = string::getFormattedString("Error opening file: 0x%08X.", fsError);
        return;
    }

    fsError = FSFILE_GetSize(m_fileHandle, reinterpret_cast<uint64_t *>(&m_fileSize));
    if (R_FAILED(fsError))
    {
        g_fslibErrorString = string::getFormattedString("Error getting file size: 0x%08X.", fsError);
        return;
    }
    // I added FS_OPEN_APPEND to FsLib. This isn't normally part of ctrulib/3DS.
    m_offset = m_openFlags & FS_OPEN_APPEND ? m_fileSize : 0;
    m_isOpen = true;
}

void fslib::File::close()
{
    if (m_isOpen)
    {
        FSFILE_Close(m_fileHandle);
    }
}

bool fslib::File::isOpen() const
{
    return m_isOpen;
}

uint64_t fslib::File::tell() const
{
    return m_offset;
}

uint64_t fslib::File::getSize() const
{
    return m_fileSize;
}

bool fslib::File::endOfFile() const
{
    return m_offset >= m_fileSize;
}

void fslib::File::seek(int64_t offset, uint8_t origin)
{
    switch (origin)
    {
        case File::beginning:
        {
            m_offset = offset;
        }
        break;

        case File::current:
        {
            m_offset += offset;
        }
        break;

        case File::end:
        {
            m_offset = m_fileSize + offset;
        }
        break;
    }
    File::ensureOffsetIsValid();
}

ssize_t fslib::File::read(void *buffer, size_t bufferSize)
{
    if (!File::isOpenForReading())
    {
        return -1;
    }

    uint32_t bytesRead = 0;
    Result fsError = FSFILE_Read(m_fileHandle,
                                 &bytesRead,
                                 static_cast<uint64_t>(m_offset),
                                 buffer,
                                 static_cast<uint32_t>(bufferSize));
    if (R_FAILED(fsError))
    {
        g_fslibErrorString = string::getFormattedString("Error reading data from file: 0x%08X.", fsError);
        // This needs to be corrected. Sometimes read errors will set the bytes read to -1?
        bytesRead = m_offset + bufferSize > m_fileSize ? m_fileSize - m_offset : bufferSize;
    }
    m_offset += bytesRead;
    return bytesRead;
}

bool fslib::File::readLine(char *buffer, size_t bufferSize)
{
    if (!File::isOpenForReading())
    {
        return false;
    }

    for (size_t i = 0; i < bufferSize; i++)
    {
        signed char nextByte = 0x00;
        if ((nextByte = File::getByte()) == -1)
        {
            return false;
        }
        else if (nextByte == '\n' || nextByte == '\r')
        {
            return true;
        }
        buffer[i] = nextByte;
    }
    return false;
}

bool fslib::File::readLine(std::string &line)
{
    // Clear line first.
    line.clear();

    if (!File::isOpenForReading())
    {
        // lol i just erased the line for nothing.
        return false;
    }

    signed char nextByte = 0x00;
    while ((nextByte = File::getByte()) != -1)
    {
        if (nextByte == '\n' || nextByte == '\r')
        {
            return true;
        }
        line += nextByte;
    }
    return false;
}

signed char fslib::File::getByte()
{
    if (!File::isOpenForReading() || m_offset >= m_fileSize)
    {
        return -1;
    }

    // This is all needed to read stuff. I'm not calling another function here just to read 1 byte.
    char byteRead = 0x00;
    uint32_t bytesRead = 0;
    Result fsError = FSFILE_Read(m_fileHandle, &bytesRead, m_offset++, &byteRead, 1);
    if (R_FAILED(fsError))
    {
        g_fslibErrorString = string::getFormattedString("Error reading byte from file: 0x%08X.", fsError);
        return -1;
    }
    return byteRead;
}

ssize_t fslib::File::write(const void *buffer, size_t bufferSize)
{
    if (!File::isOpenForWriting() || !File::resizeIfNeeded(bufferSize))
    {
        return -1;
    }

    uint32_t bytesWritten = 0;
    Result fsError = FSFILE_Write(m_fileHandle, &bytesWritten, m_offset, buffer, bufferSize, 0);
    if (R_FAILED(fsError))
    {
        g_fslibErrorString = string::getFormattedString("Error writing to file: 0x%08X.", fsError);
        return -1;
    }
    m_offset += bytesWritten;
    return bytesWritten;
}

bool fslib::File::writef(const char *format, ...)
{
    char vaBuffer[VA_BUFFER_SIZE] = {0};

    std::va_list vaList;
    va_start(vaList, format);
    vsnprintf(vaBuffer, VA_BUFFER_SIZE, format, vaList);
    va_end(vaList);

    size_t stringLength = std::char_traits<char>::length(vaBuffer);
    return File::write(vaBuffer, stringLength) == static_cast<ssize_t>(stringLength);
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

bool fslib::File::putByte(char byte)
{
    if (!File::isOpenForWriting() || !File::resizeIfNeeded(1))
    {
        return false;
    }

    uint32_t bytesWritten = 0;
    Result fsError = FSFILE_Write(m_fileHandle, &bytesWritten, m_offset++, &byte, 1, 0);
    if (R_FAILED(fsError))
    {
        g_fslibErrorString = string::getFormattedString("Error writing byte to file: 0x%08X.", fsError);
        return false;
    }
    return true;
}

bool fslib::File::flush()
{
    if (!File::isOpenForWriting())
    {
        return false;
    }

    Result fsError = FSFILE_Flush(m_fileHandle);
    if (R_FAILED(fsError))
    {
        g_fslibErrorString = string::getFormattedString("Error flushing file: 0x%08X.", fsError);
        return false;
    }
    return true;
}

void fslib::File::ensureOffsetIsValid()
{
    if (m_offset < 0)
    {
        m_offset = 0;
    }
    else if (m_offset > m_fileSize)
    {
        m_offset = m_fileSize;
    }
}

bool fslib::File::resizeIfNeeded(size_t bufferSize)
{
    size_t spaceRemaining = m_fileSize - m_offset;

    // Everything is fine.
    if (spaceRemaining >= bufferSize)
    {
        return true;
    }

    uint64_t newFileSize = m_offset + bufferSize;
    Result fsError = FSFILE_SetSize(m_fileHandle, newFileSize);
    if (R_FAILED(fsError))
    {
        g_fslibErrorString = string::getFormattedString("Error resizing file to it buffer: 0x%08X.", fsError);
        return false;
    }
    // Make sure to update file size.
    m_fileSize = newFileSize;
    return true;
}
