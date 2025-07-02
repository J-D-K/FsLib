#include "fslib.hpp"
#include "string.hpp"
#include <string>

namespace
{
    const char *ERROR_OPENING_WITH_FILTER = "Error opening SaveInfoReader with filter: 0x%X.";
}

extern std::string g_fslibErrorString;

fslib::SaveInfoReader::SaveInfoReader(FsSaveDataSpaceId saveDataSpaceID, size_t bufferCount)
{
    SaveInfoReader::open(saveDataSpaceID, bufferCount);
}

fslib::SaveInfoReader::SaveInfoReader(FsSaveDataSpaceId saveSpaceID, AccountUid accountID, size_t bufferCount)
{
    SaveInfoReader::open(saveSpaceID, accountID, bufferCount);
}

fslib::SaveInfoReader::SaveInfoReader(FsSaveDataSpaceId saveSpaceID, FsSaveDataType saveType, size_t bufferCount)
{
    SaveInfoReader::open(saveSpaceID, saveType, bufferCount);
}

fslib::SaveInfoReader::~SaveInfoReader()
{
    SaveInfoReader::close();
}

void fslib::SaveInfoReader::open(FsSaveDataSpaceId saveDataSpaceID, size_t bufferCount)
{
    // Set this just in case.
    m_isOpen = false;

    // Just in case
    SaveInfoReader::close();

    // Save this quick.
    m_bufferCount = bufferCount;

    Result fsError = fsOpenSaveDataInfoReader(&m_infoReader, saveDataSpaceID);
    if (R_FAILED(fsError))
    {
        g_fslibErrorString = string::get_formatted_string("Error opening save data info reader: 0x%X.", fsError);
        return;
    }

    SaveInfoReader::allocate_save_info_array(bufferCount);

    // Should be good.
    m_isOpen = true;
}

void fslib::SaveInfoReader::open(FsSaveDataSpaceId saveSpaceID, AccountUid accountID, size_t bufferCount)
{
    m_isOpen = false;

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
        g_fslibErrorString = string::get_formatted_string(ERROR_OPENING_WITH_FILTER, fsError);
        return;
    }

    SaveInfoReader::allocate_save_info_array(bufferCount);

    m_isOpen = true;
}

void fslib::SaveInfoReader::open(FsSaveDataSpaceId saveSpaceID, FsSaveDataType saveType, size_t bufferCount)
{
    m_isOpen = false;

    SaveInfoReader::close();

    FsSaveDataFilter saveFilter = {.filter_by_application_id = false,
                                   .filter_by_save_data_type = true,
                                   .filter_by_user_id = false,
                                   .filter_by_system_save_data_id = false,
                                   .filter_by_index = false,
                                   .save_data_rank = FsSaveDataRank_Primary,
                                   .padding = {0},
                                   .attr = {.application_id = 0,
                                            .uid = {0},
                                            .save_data_type = saveType,
                                            .save_data_rank = FsSaveDataRank_Primary,
                                            .save_data_index = 0}};

    Result fsError = fsOpenSaveDataInfoReaderWithFilter(&m_infoReader, saveSpaceID, &saveFilter);
    if (R_FAILED(fsError))
    {
        g_fslibErrorString = string::get_formatted_string(ERROR_OPENING_WITH_FILTER, fsError);
        return;
    }

    SaveInfoReader::allocate_save_info_array(bufferCount);

    m_isOpen = true;
}

void fslib::SaveInfoReader::close()
{
    if (m_isOpen)
    {
        fsSaveDataInfoReaderClose(&m_infoReader);
        m_isOpen = false;
    }
}

bool fslib::SaveInfoReader::is_open() const
{
    return m_isOpen;
}

bool fslib::SaveInfoReader::read()
{
    // This function will try to read as many as possible. It will return false once the count is 0.
    Result fsError = fsSaveDataInfoReaderRead(&m_infoReader, m_saveInfoBuffer.get(), m_bufferCount, &m_readCount);
    if (R_FAILED(fsError) || m_readCount == 0)
    {
        g_fslibErrorString = string::get_formatted_string("Error reading save data info: 0x%X.", fsError);
        return false;
    }
    return true;
}

int64_t fslib::SaveInfoReader::get_read_count() const
{
    return m_readCount;
}

fslib::SaveInfoReader::operator bool() const
{
    return m_isOpen;
}

FsSaveDataInfo &fslib::SaveInfoReader::operator[](int index)
{
    if (index < 0 || index >= static_cast<int>(m_bufferCount))
    {
        // To do: Better solution.
        return m_saveInfoBuffer[0];
    }
    return m_saveInfoBuffer[index];
}

void fslib::SaveInfoReader::allocate_save_info_array(size_t bufferCount)
{
    // Record this.
    m_bufferCount = bufferCount;

    // Allocate. This should free any previously freed buffers.
    m_saveInfoBuffer = std::make_unique<FsSaveDataInfo[]>(m_bufferCount);
}
