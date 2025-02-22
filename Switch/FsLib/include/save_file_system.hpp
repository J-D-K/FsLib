#pragma once
#include <cstdint>
#include <string>
#include <switch.h>

namespace fslib
{
    /// @brief Attempts to open system save data with SystemSaveID and map it to DeviceName.
    /// @param deviceName Name of device to map to.
    /// @param systemSaveID ID of system save.
    /// @param saveDataSpaceID (Optional) FsSaveDataSpaceID.
    /// @param saveDataRank (Optional) Rank of save data.
    /// @param accountID (Optional) AccountID to use to open system save. This is usually 0, but can be tied to accounts.
    /// @return True on success. False on failure.
    bool open_system_save_file_system(std::string_view deviceName,
                                      uint64_t systemSaveID,
                                      FsSaveDataSpaceId saveDataSpaceID = FsSaveDataSpaceId_System,
                                      FsSaveDataRank saveDataRank = FsSaveDataRank_Primary,
                                      AccountUid accountID = {0});

    /// @brief Attempts to open account save data with ApplicationID and UserID and map it to DeviceName.
    /// @param deviceName Name of device to map to.
    /// @param applicationID Title ID of game or title.
    /// @param userID Account ID of user that "owns" the filesystem.
    /// @param saveDataSpaceID (Optional) Save data space ID.
    /// @param saveDataRank (Optional) SaveDataRank.
    /// @return True on success. False on failure.
    bool open_account_save_file_system(std::string_view deviceName,
                                       uint64_t applicationID,
                                       AccountUid userID,
                                       FsSaveDataSpaceId saveDataSpaceID = FsSaveDataSpaceId_User,
                                       FsSaveDataRank saveDataRank = FsSaveDataRank_Primary);

    /// @brief Attempts to open BCAT save data with ApplicationID and map it to DeviceName.
    /// @param deviceName Name of device to map to.
    /// @param applicationID Title ID of game or title.
    /// @return True on success. False on failure.
    bool open_bcat_save_file_system(std::string_view deviceName, uint64_t applicationID);

    /// @brief Attempts to open device save data with ApplicationID and map it to DeviceName.
    /// @param deviceName Name of device to map to.
    /// @param applicationID Title ID of game or title.
    /// @return True on success. False on failure.
    bool open_device_save_file_system(std::string_view deviceName, uint64_t applicationID);

    /// @brief Attempts to open temporary save data and map it to DeviceName.
    /// @param deviceName Name of device to map to.
    /// @return True on success. False on failure.
    /// @note I've only ever seen this used once before...
    bool open_temporary_save_file_system(std::string_view deviceName);

    /// @brief Attempts to open cache save data and map it to DeviceName.
    /// @param deviceName Name of device to map to.
    /// @param applicationID Title ID of game or title.
    /// @param saveDataIndex Index of the cache save.
    /// @param saveDataSpaceID (Optional) Save data space ID.
    /// @param saveDataRank (Optional) Save data rank.
    /// @return True on success. False on failure.
    bool open_cache_save_file_system(std::string_view deviceName,
                                     uint64_t applicationID,
                                     uint16_t saveDataIndex,
                                     FsSaveDataSpaceId saveDataSpaceID = FsSaveDataSpaceId_User,
                                     FsSaveDataRank saveDataRank = FsSaveDataRank_Primary);

    /// @brief Attempts to open system BCAT save data and map it to DeviceName.
    /// @param deviceName Name of device to map to.
    /// @param systemSaveID ID of system save.
    /// @return True on success. False on failure.
    /// @note I've never seen this used before. Literally <b>ever</b>.
    bool open_system_bcat_save_file_system(std::string_view deviceName, uint64_t systemSaveID);

    /// @brief Attempts to open save data with FsSaveDataInfo passed.
    /// @param deviceName Name of device to map to.
    /// @param saveInfo FsSaveDataInfo to mount from.
    /// @return True on success. False on failure.
    bool open_save_data_with_save_info(std::string_view deviceName, const FsSaveDataInfo &saveInfo);
} // namespace fslib
