#include "fslib.hpp"
#include "string.hpp"
#include <string>

namespace
{
    const char *ERROR_OPENING_WITH_FILTER = "Error opening SaveInfoReader with filter: 0x%X.";
}

extern std::string g_fslibErrorString;

fslib::SaveInfoReader::SaveInfoReader(FsSaveDataSpaceId saveDataSpaceID)
{
    SaveInfoReader::open(saveDataSpaceID);
}

fslib::SaveInfoReader::SaveInfoReader(FsSaveDataSpaceId saveSpaceID, AccountUid accountID)
{
    SaveInfoReader::open(saveSpaceID, accountID);
}

fslib::SaveInfoReader::SaveInfoReader(FsSaveDataSpaceId saveSpaceID, FsSaveDataType saveType)
{
    SaveInfoReader::open(saveSpaceID, saveType);
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

void fslib::SaveInfoReader::open(FsSaveDataSpaceId saveSpaceID, AccountUid accountID)
{
    // Just in case.
    SaveInfoReader::close();

    // Filter
    FsSaveDataFilter saveFilter = {.filter_by_application_id = false,
                                   .filter_by_save_data_type = false,
                                   .filter_by_user_id = true,
                                   .filter_by_system_save_data_id = false,
                                   .filter_by_index = false,
                                   .save_data_rank = FsSaveDataRank_Primary,
                                   .padding = {0},
                                   .attr = {.application_id = 0,
                                            .uid = accountID,
                                            .system_save_data_id = 0,
                                            .save_data_type = 0,
                                            .save_data_rank = FsSaveDataRank_Primary,
                                            .save_data_index = 0}};

    Result fsError = fsOpenSaveDataInfoReaderWithFilter(&m_infoReader, saveSpaceID, &saveFilter);
    if (R_FAILED(fsError))
    {
        g_fslibErrorString = string::getFormattedString(ERROR_OPENING_WITH_FILTER, fsError);
        return;
    }
    m_isOpen = true;
}

void fslib::SaveInfoReader::open(FsSaveDataSpaceId saveSpaceID, FsSaveDataType saveType)
{
    SaveInfoReader::close();

    FsSaveDataFilter saveFilter = {
        .filter_by_application_id = false,
        .filter_by_save_data_type = true,
        .filter_by_user_id = false,
        .filter_by_system_save_data_id = false,
        .filter_by_index = false,
        .save_data_rank = FsSaveDataRank_Primary,
        .padding = {0},
        .attr = {.application_id = 0, .uid = {0}, .save_data_type = saveType, .save_data_rank = FsSaveDataRank_Primary, .save_data_index = 0}};

    Result fsError = fsOpenSaveDataInfoReaderWithFilter(&m_infoReader, saveSpaceID, &saveFilter);
    if (R_FAILED(fsError))
    {
        g_fslibErrorString = string::getFormattedString(ERROR_OPENING_WITH_FILTER, fsError);
        return;
    }
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

fslib::SaveInfoReader::operator bool(void) const
{
    return m_isOpen;
}
