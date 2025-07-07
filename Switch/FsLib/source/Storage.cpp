#include "Storage.hpp"
#include "error.hpp"
#include <string>

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
    m_isOpen = false;

    Storage::close();

    if (error::occurred(fsOpenBisStorage(&m_storageHandle, partitionID)))
    {
        return;
    }

    if (error::occurred(fsStorageGetSize(&m_storageHandle, &m_streamSize)))
    {
        return;
    }
    m_offset = 0;
    m_isOpen = true;
}

void fslib::Storage::close()
{
    fsStorageClose(&m_storageHandle);
}

ssize_t fslib::Storage::read(void *buffer, size_t bufferSize)
{
    if (m_offset + static_cast<int64_t>(bufferSize) > m_streamSize)
    {
        bufferSize = m_streamSize - m_offset;
    }

    if (error::occurred(fsStorageRead(&m_storageHandle, m_offset, buffer, static_cast<uint64_t>(bufferSize))))
    {
        return -1;
    }

    // There isn't really a way to make sure this worked 100%...
    m_offset += bufferSize;
    return bufferSize;
}

signed char fslib::Storage::read_byte()
{
    if (m_offset >= m_streamSize)
    {
        return -1;
    }

    char byte = 0x00;
    if (error::occurred(fsStorageRead(&m_storageHandle, m_offset++, &byte, 1)))
    {
        return -1;
    }
    return byte;
}
