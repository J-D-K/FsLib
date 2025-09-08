#include "error.hpp"
#include "fslib.hpp"

#include <string>

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

fslib::SaveInfoReader::SaveInfoReader(SaveInfoReader &&saveInfoReader) noexcept
    : m_infoReader(saveInfoReader.m_infoReader)
    , m_isOpen(saveInfoReader.m_isOpen)
    , m_bufferCount(saveInfoReader.m_bufferCount)
    , m_readCount(saveInfoReader.m_readCount)
    , m_saveInfoBuffer(std::move(saveInfoReader.m_saveInfoBuffer))
{
    saveInfoReader.m_infoReader  = {0};
    saveInfoReader.m_isOpen      = false;
    saveInfoReader.m_bufferCount = 0;
    saveInfoReader.m_readCount   = 0;
}

fslib::SaveInfoReader &fslib::SaveInfoReader::operator=(SaveInfoReader &&saveInfoReader) noexcept
{
    m_infoReader     = saveInfoReader.m_infoReader;
    m_isOpen         = saveInfoReader.m_isOpen;
    m_bufferCount    = saveInfoReader.m_bufferCount;
    m_readCount      = saveInfoReader.m_readCount;
    m_saveInfoBuffer = std::move(saveInfoReader.m_saveInfoBuffer);

    saveInfoReader.m_infoReader  = {0};
    saveInfoReader.m_isOpen      = false;
    saveInfoReader.m_bufferCount = 0;
    saveInfoReader.m_readCount   = 0;

    return *this;
}

fslib::SaveInfoReader::~SaveInfoReader() { SaveInfoReader::close(); }

void fslib::SaveInfoReader::open(FsSaveDataSpaceId saveDataSpaceID, size_t bufferCount)
{
    m_isOpen = false;
    SaveInfoReader::close();

    const bool openError = error::occurred(fsOpenSaveDataInfoReader(&m_infoReader, saveDataSpaceID));
    if (openError) { return; }
    SaveInfoReader::allocate_save_info_array(bufferCount);

    m_isOpen = true;
}

void fslib::SaveInfoReader::open(FsSaveDataSpaceId saveSpaceID, AccountUid accountID, size_t bufferCount)
{
    m_isOpen = false;
    SaveInfoReader::close();

    const FsSaveDataFilter saveFilter = {.filter_by_application_id      = false,
                                         .filter_by_save_data_type      = false,
                                         .filter_by_user_id             = true,
                                         .filter_by_system_save_data_id = false,
                                         .filter_by_index               = false,
                                         .save_data_rank                = FsSaveDataRank_Primary,
                                         .padding                       = {0},
                                         .attr                          = {.application_id      = 0,
                                                                           .uid                 = accountID,
                                                                           .system_save_data_id = 0,
                                                                           .save_data_type      = 0,
                                                                           .save_data_rank      = FsSaveDataRank_Primary,
                                                                           .save_data_index     = 0}};

    const bool openError = error::occurred(fsOpenSaveDataInfoReaderWithFilter(&m_infoReader, saveSpaceID, &saveFilter));
    if (openError) { return; }
    SaveInfoReader::allocate_save_info_array(bufferCount);

    m_isOpen = true;
}

void fslib::SaveInfoReader::open(FsSaveDataSpaceId saveSpaceID, FsSaveDataType saveType, size_t bufferCount)
{
    m_isOpen = false;
    SaveInfoReader::close();

    const FsSaveDataFilter saveFilter = {.filter_by_application_id      = false,
                                         .filter_by_save_data_type      = true,
                                         .filter_by_user_id             = false,
                                         .filter_by_system_save_data_id = false,
                                         .filter_by_index               = false,
                                         .save_data_rank                = FsSaveDataRank_Primary,
                                         .padding                       = {0},
                                         .attr                          = {.application_id  = 0,
                                                                           .uid             = {0},
                                                                           .save_data_type  = saveType,
                                                                           .save_data_rank  = FsSaveDataRank_Primary,
                                                                           .save_data_index = 0}};

    const bool openError = error::occurred(fsOpenSaveDataInfoReaderWithFilter(&m_infoReader, saveSpaceID, &saveFilter));
    if (openError) { return; }
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

bool fslib::SaveInfoReader::is_open() const { return m_isOpen; }

bool fslib::SaveInfoReader::read()
{
    // This function will try to read as many as possible. It will return false once the count is 0.
    const bool readError =
        error::occurred(fsSaveDataInfoReaderRead(&m_infoReader, m_saveInfoBuffer.get(), m_bufferCount, &m_readCount));
    const bool validCount = m_readCount > 0;
    if (readError || !validCount) { return false; }

    return true;
}

int64_t fslib::SaveInfoReader::get_read_count() const { return m_readCount; }

FsSaveDataInfo &fslib::SaveInfoReader::at(int index)
{
    if (index < 0 || index >= m_readCount) { return m_saveInfoBuffer[0]; }
    return m_saveInfoBuffer[index];
}

FsSaveDataInfo &fslib::SaveInfoReader::operator[](int index) { return m_saveInfoBuffer[index]; }

const FsSaveDataInfo *fslib::SaveInfoReader::begin() const noexcept { return &m_saveInfoBuffer[0]; }

const FsSaveDataInfo *fslib::SaveInfoReader::end() const noexcept { return &m_saveInfoBuffer[m_readCount]; }

void fslib::SaveInfoReader::allocate_save_info_array(size_t bufferCount)
{
    // Record this.
    m_bufferCount = bufferCount;

    // Allocate. This should free any previously freed buffers.
    m_saveInfoBuffer = std::make_unique<FsSaveDataInfo[]>(m_bufferCount);
}
