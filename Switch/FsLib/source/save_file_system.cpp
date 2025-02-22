#include "save_file_system.hpp"
#include "fslib.hpp"
#include "string.hpp"
#include <switch.h>

extern std::string g_fslibErrorString;

bool fslib::open_system_save_file_system(std::string_view deviceName,
                                         uint64_t systemSaveID,
                                         FsSaveDataSpaceId saveDataSpaceID,
                                         FsSaveDataRank saveDataRank,
                                         AccountUid accountID)
{
    FsSaveDataAttribute saveDataAttributes = {.application_id = 0,
                                              .uid = accountID,
                                              .system_save_data_id = systemSaveID,
                                              .save_data_type = FsSaveDataType_System,
                                              .save_data_rank = saveDataRank,
                                              .save_data_index = 0};

    FsFileSystem fileSystem;
    Result fsError = fsOpenSaveDataFileSystemBySystemSaveDataId(&fileSystem, saveDataSpaceID, &saveDataAttributes);
    if (R_FAILED(fsError))
    {
        g_fslibErrorString = string::get_formatted_string("Error opening system save data: 0x%X.", fsError);
        return false;
    }

    if (!fslib::map_file_system(deviceName, &fileSystem))
    {
        fsFsClose(&fileSystem);
        return false;
    }

    return true;
}

bool fslib::open_account_save_file_system(std::string_view deviceName,
                                          uint64_t applicationID,
                                          AccountUid userID,
                                          FsSaveDataSpaceId saveDataSpaceID,
                                          FsSaveDataRank saveDataRank)
{
    FsSaveDataAttribute saveDataAttributes = {.application_id = applicationID,
                                              .uid = userID,
                                              .system_save_data_id = 0,
                                              .save_data_type = FsSaveDataType_Account,
                                              .save_data_rank = saveDataRank,
                                              .save_data_index = 0};

    FsFileSystem fileSystem;
    Result fsError = fsOpenSaveDataFileSystem(&fileSystem, saveDataSpaceID, &saveDataAttributes);
    if (R_FAILED(fsError))
    {
        g_fslibErrorString = string::get_formatted_string("Error opening account save data: 0x%X.", fsError);
        return false;
    }

    if (!fslib::map_file_system(deviceName, &fileSystem))
    {
        fsFsClose(&fileSystem);
        return false;
    }

    return true;
}

bool fslib::open_bcat_save_file_system(std::string_view deviceName, uint64_t applicationID)
{
    FsSaveDataAttribute saveDataAttributes = {.application_id = applicationID,
                                              .uid = {0},
                                              .system_save_data_id = 0,
                                              .save_data_type = FsSaveDataType_Bcat,
                                              .save_data_rank = FsSaveDataRank_Primary,
                                              .save_data_index = 0};

    FsFileSystem fileSystem;
    Result fsError = fsOpenSaveDataFileSystem(&fileSystem, FsSaveDataSpaceId_User, &saveDataAttributes);
    if (R_FAILED(fsError))
    {
        g_fslibErrorString = string::get_formatted_string("Error opening BCAT save data: 0x%X.", fsError);
        return false;
    }

    if (!fslib::map_file_system(deviceName, &fileSystem))
    {
        fsFsClose(&fileSystem);
        return false;
    }

    return true;
}

bool fslib::open_device_save_file_system(std::string_view deviceName, uint64_t applicationID)
{
    FsSaveDataAttribute saveDataAttributes = {.application_id = applicationID,
                                              .uid = {0},
                                              .system_save_data_id = 0,
                                              .save_data_type = FsSaveDataType_Device,
                                              .save_data_rank = FsSaveDataRank_Primary,
                                              .save_data_index = 0};

    FsFileSystem fileSystem;
    Result fsError = fsOpenSaveDataFileSystem(&fileSystem, FsSaveDataSpaceId_User, &saveDataAttributes);
    if (R_FAILED(fsError))
    {
        g_fslibErrorString = string::get_formatted_string("Error opening device save data: 0x%X.", fsError);
        return false;
    }

    if (!fslib::map_file_system(deviceName, &fileSystem))
    {
        fsFsClose(&fileSystem);
        return false;
    }

    return true;
}

bool fslib::open_temporary_save_file_system(std::string_view deviceName)
{
    FsSaveDataAttribute saveDataAttributes = {.application_id = 0,
                                              .uid = {0},
                                              .system_save_data_id = 0,
                                              .save_data_type = FsSaveDataType_Temporary,
                                              .save_data_rank = FsSaveDataRank_Primary,
                                              .save_data_index = 0};

    FsFileSystem fileSystem;
    Result fsError = fsOpenSaveDataFileSystem(&fileSystem, FsSaveDataSpaceId_User, &saveDataAttributes);
    if (R_FAILED(fsError))
    {
        g_fslibErrorString = string::get_formatted_string("Error opening temporary save data: 0x%X.", fsError);
        return false;
    }

    if (!fslib::map_file_system(deviceName, &fileSystem))
    {
        fsFsClose(&fileSystem);
        return false;
    }

    return true;
}

bool fslib::open_cache_save_file_system(std::string_view deviceName,
                                        uint64_t applicationID,
                                        uint16_t saveDataIndex,
                                        FsSaveDataSpaceId saveDataSpaceID,
                                        FsSaveDataRank saveDataRank)
{
    FsSaveDataAttribute saveDataAttributes = {.application_id = applicationID,
                                              .uid = {0},
                                              .system_save_data_id = 0,
                                              .save_data_type = FsSaveDataType_Cache,
                                              .save_data_rank = saveDataRank,
                                              .save_data_index = saveDataIndex};

    FsFileSystem fileSystem;
    Result fsError = fsOpenSaveDataFileSystem(&fileSystem, saveDataSpaceID, &saveDataAttributes);
    if (R_FAILED(fsError))
    {
        g_fslibErrorString = string::get_formatted_string("Error opening cache save data: 0x%X.", fsError);
        return false;
    }

    if (!fslib::map_file_system(deviceName, &fileSystem))
    {
        fsFsClose(&fileSystem);
        return false;
    }

    return true;
}

bool fslib::open_system_bcat_save_file_system(std::string_view deviceName, uint64_t systemSaveID)
{
    FsSaveDataAttribute saveDataAttributes = {.application_id = 0,
                                              .uid = {0},
                                              .system_save_data_id = systemSaveID,
                                              .save_data_type = FsSaveDataType_SystemBcat,
                                              .save_data_rank = FsSaveDataRank_Primary,
                                              .save_data_index = 0};

    FsFileSystem fileSystem;
    Result fsError =
        fsOpenSaveDataFileSystemBySystemSaveDataId(&fileSystem, FsSaveDataSpaceId_User, &saveDataAttributes);
    if (R_FAILED(fsError))
    {
        g_fslibErrorString = string::get_formatted_string("Error opening system bcat save data: 0x%X.", fsError);
        return false;
    }

    if (!fslib::map_file_system(deviceName, &fileSystem))
    {
        fsFsClose(&fileSystem);
        return false;
    }

    return true;
}

bool fslib::open_save_data_with_save_info(std::string_view deviceName, const FsSaveDataInfo &saveInfo)
{
    // This is actually nicer.
    FsSaveDataAttribute saveDataAttributes = {.application_id = saveInfo.application_id,
                                              .uid = saveInfo.uid,
                                              .system_save_data_id = saveInfo.system_save_data_id,
                                              .save_data_type = saveInfo.save_data_type,
                                              .save_data_rank = saveInfo.save_data_rank,
                                              .save_data_index = saveInfo.save_data_index};

    Result fsError = 0;
    FsFileSystem fileSystem;
    if (saveInfo.save_data_type == FsSaveDataType_System || saveInfo.save_data_type == FsSaveDataType_SystemBcat)
    {
        fsError =
            fsOpenSaveDataFileSystemBySystemSaveDataId(&fileSystem,
                                                       static_cast<FsSaveDataSpaceId>(saveInfo.save_data_space_id),
                                                       &saveDataAttributes);
    }
    else
    {
        fsError = fsOpenSaveDataFileSystem(&fileSystem,
                                           static_cast<FsSaveDataSpaceId>(saveInfo.save_data_space_id),
                                           &saveDataAttributes);
    }

    if (R_FAILED(fsError))
    {
        g_fslibErrorString =
            string::get_formatted_string("Error opening save data with FsSaveDataInfo: 0x%X.", fsError);
        return false;
    }

    if (!fslib::map_file_system(deviceName, &fileSystem))
    {
        fsFsClose(&fileSystem);
        return false;
    }

    return true;
}
