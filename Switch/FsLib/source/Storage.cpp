#include "Storage.hpp"
#include "error.hpp"
#include <string>

fslib::Storage::Storage(FsBisPartitionId partitionID)
{
    Storage::open(partitionID);
}

fslib::Storage::~Storage()
{
    if (m_isOpen) { Storage::close(); }
}

void fslib::Storage::open(FsBisPartitionId partitionID)
{
    m_isOpen = false;
    Storage::close();

    const bool openError = error::occurred(fsOpenBisStorage(&m_storageHandle, partitionID));
    const bool sizeError = !openError && error::occurred(fsStorageGetSize(&m_storageHandle, &m_streamSize));
    if (openError || sizeError) { return; }

    m_offset = 0;
    m_isOpen = true;
}

void fslib::Storage::close()
{
    fsStorageClose(&m_storageHandle);
}

ssize_t fslib::Storage::read(void *buffer, size_t bufferSize)
{
    int64_t sBufferSize    = bufferSize;
    const bool validBounds = m_offset + sBufferSize <= m_streamSize;
    sBufferSize            = validBounds ? bufferSize : m_streamSize - m_offset;
    const bool readError   = error::occurred(fsStorageRead(&m_storageHandle, m_offset, buffer, sBufferSize));
    if (readError) { return -1; }

    // There isn't really a way to make sure this worked 100%...
    m_offset += sBufferSize;
    return sBufferSize;
}

signed char fslib::Storage::read_byte()
{
    if (m_offset >= m_streamSize) { return -1; }

    unsigned byte{};
    const bool readError = error::occurred(fsStorageRead(&m_storageHandle, m_offset, &byte, 1));
    if (readError) { return -1; }

    return byte;
}
