#include "SaveInfoIterator.hpp"

#include "SaveInfoReader.hpp"

fslib::SaveInfoIterator::SaveInfoIterator(const SaveInfoReader *infoReader)
    : m_infoReader(infoReader) {};

FsSaveDataInfo *fslib::SaveInfoIterator::begin() const noexcept { return &m_infoReader->m_saveInfoBuffer[0]; }

FsSaveDataInfo *fslib::SaveInfoIterator::end() const noexcept
{
    return &m_infoReader->m_saveInfoBuffer[m_infoReader->m_readCount];
}
