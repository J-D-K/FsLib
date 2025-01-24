#include "Storage.hpp"
#include "string.hpp"
#include <string>

// Globally used error string.
extern std::string g_fslibErrorString;

fslib::Storage::Storage(FsBisPartitionId partitionID)
{
    Storage::open(partitionID);
}

fslib::Storage::~Storage()
{
    if (m_isOpen)
    {
        Storage::close();
    }
}

void fslib::Storage::open(FsBisPartitionId partitionID)
{
    // Just in case this is reused.
    Storage::close();

    Result fsError = fsOpenBisStorage(&m_storageHandle, partitionID);
    if (R_FAILED(fsError))
    {
        g_fslibErrorString = string::getFormattedString("Error opening partition ID %X: 0x%X.", partitionID, fsError);
        return;
    }

    fsError = fsStorageGetSize(&m_storageHandle, &m_streamSize);
    if (R_FAILED(fsError))
    {
        g_fslibErrorString = string::getFormattedString("Error getting storage size: 0x%X.", fsError);
        return;
    }
    // Offset is always 0 since storage is read only.
    m_offset = 0;
    m_isOpen = true;
}

void fslib::Storage::close(void)
{
    fsStorageClose(&m_storageHandle);
}

ssize_t fslib::Storage::read(void *buffer, size_t bufferSize)
{
    if (m_offset + static_cast<int64_t>(bufferSize) > m_streamSize)
    {
        bufferSize = m_streamSize - m_offset;
    }

    Result fsError = fsStorageRead(&m_storageHandle, m_offset, buffer, static_cast<uint64_t>(bufferSize));
    if (R_FAILED(fsError))
    {
        g_fslibErrorString = string::getFormattedString("Error reading from storage: 0x%X.", fsError);
        return 0;
    }
    // There isn't really a way to make sure this worked 100%...
    m_offset += bufferSize;
    return bufferSize;
}

signed char fslib::Storage::readByte(void)
{
    if (m_offset >= m_streamSize)
    {
        return -1;
    }

    char byte = 0x00;
    Result fsError = fsStorageRead(&m_storageHandle, m_offset++, &byte, 1);
    if (R_FAILED(fsError))
    {
        g_fslibErrorString = string::getFormattedString("Error reading byte from storage: 0x%X.", fsError);
        return -1;
    }
    return byte;
}
