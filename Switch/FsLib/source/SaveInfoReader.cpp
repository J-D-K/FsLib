#include "fslib.hpp"
#include "string.hpp"
#include <string>

extern std::string g_fslibErrorString;

fslib::SaveInfoReader::SaveInfoReader(FsSaveDataSpaceId saveDataSpaceID)
{
    SaveInfoReader::open(saveDataSpaceID);
}

fslib::SaveInfoReader::~SaveInfoReader()
{
    SaveInfoReader::close();
}

void fslib::SaveInfoReader::open(FsSaveDataSpaceId saveDataSpaceID)
{
    // Just in case
    SaveInfoReader::close();

    Result fsError = fsOpenSaveDataInfoReader(&m_infoReader, saveDataSpaceID);
    if (R_FAILED(fsError))
    {
        g_fslibErrorString = string::getFormattedString("Error opening save data info reader: 0x%X.", fsError);
        return;
    }
    // Should be good.
    m_isOpen = true;
}

void fslib::SaveInfoReader::close(void)
{
    if (m_isOpen)
    {
        fsSaveDataInfoReaderClose(&m_infoReader);
        m_isOpen = false;
    }
}

bool fslib::SaveInfoReader::isOpen(void) const
{
    return m_isOpen;
}

bool fslib::SaveInfoReader::read(void)
{
    // We're only reading one at a time. I don't think libnx has a function to get the count?
    int64_t totalEntries = 0;
    Result fsError = fsSaveDataInfoReaderRead(&m_infoReader, &m_saveInfo, 1, &totalEntries);
    if (R_FAILED(fsError) || totalEntries == 0)
    {
        g_fslibErrorString = string::getFormattedString("Error reading save data info: 0x%X.", fsError);
        return false;
    }
    return true;
}

FsSaveDataInfo &fslib::SaveInfoReader::get(void)
{
    return m_saveInfo;
}
