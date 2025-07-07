#include "save_file_system.hpp"
#include "error.hpp"
#include "fslib.hpp"
#include <switch.h>

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

    FsFileSystem filesystem;
    if (error::occurred(fsOpenSaveDataFileSystemBySystemSaveDataId(&filesystem, saveDataSpaceID, &saveDataAttributes)))
    {
        return false;
    }

    if (!fslib::map_file_system(deviceName, &filesystem))
    {
        fsFsClose(&filesystem);
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

    FsFileSystem filesystem;
    if (error::occurred(fsOpenSaveDataFileSystem(&filesystem, saveDataSpaceID, &saveDataAttributes)))
    {
        return false;
    }

    if (!fslib::map_file_system(deviceName, &filesystem))
    {
        fsFsClose(&filesystem);
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

    FsFileSystem filesystem;
    if (error::occurred(fsOpenSaveDataFileSystem(&filesystem, FsSaveDataSpaceId_User, &saveDataAttributes)))
    {
        return false;
    }

    if (!fslib::map_file_system(deviceName, &filesystem))
    {
        fsFsClose(&filesystem);
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

    FsFileSystem filesystem;
    if (error::occurred(fsOpenSaveDataFileSystem(&filesystem, FsSaveDataSpaceId_User, &saveDataAttributes)))
    {
        return false;
    }

    if (!fslib::map_file_system(deviceName, &filesystem))
    {
        fsFsClose(&filesystem);
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

    FsFileSystem filesystem;
    if (error::occurred(fsOpenSaveDataFileSystem(&filesystem, FsSaveDataSpaceId_User, &saveDataAttributes)))
    {
        return false;
    }

    if (!fslib::map_file_system(deviceName, &filesystem))
    {
        fsFsClose(&filesystem);
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

    FsFileSystem filesystem;
    if (error::occurred(fsOpenSaveDataFileSystem(&filesystem, saveDataSpaceID, &saveDataAttributes)))
    {
        return false;
    }

    if (!fslib::map_file_system(deviceName, &filesystem))
    {
        fsFsClose(&filesystem);
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

    FsFileSystem filesystem;
    if (error::occurred(
            fsOpenSaveDataFileSystemBySystemSaveDataId(&filesystem, FsSaveDataSpaceId_User, &saveDataAttributes)))
    {
        return false;
    }

    if (!fslib::map_file_system(deviceName, &filesystem))
    {
        fsFsClose(&filesystem);
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

    FsFileSystem filesystem;
    if ((saveInfo.save_data_type == FsSaveDataType_System || saveInfo.save_data_type == FsSaveDataType_SystemBcat) &&
        error::occurred(
            fsOpenSaveDataFileSystemBySystemSaveDataId(&filesystem,
                                                       static_cast<FsSaveDataSpaceId>(saveInfo.save_data_id),
                                                       &saveDataAttributes)))
    {
        return false;
    }
    else if (error::occurred(fsOpenSaveDataFileSystem(&filesystem,
                                                      static_cast<FsSaveDataSpaceId>(saveInfo.save_data_space_id),
                                                      &saveDataAttributes)))
    {
        return false;
    }

    if (!fslib::map_file_system(deviceName, &filesystem))
    {
        fsFsClose(&filesystem);
        return false;
    }

    return true;
}
