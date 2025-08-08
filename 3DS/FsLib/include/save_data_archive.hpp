#pragma once
#include <3ds.h>
#include <string>

namespace fslib
{
    /// @brief Opens save archive of current title. This is only really useful for old hax entrypoints.
    /// @param deviceName Name of device to map to use. Ex: u"SaveData"
    /// @return True on success. False on failure.
    bool open_save_data(std::u16string_view deviceName);

    /// @brief Opens Extra Data archive associated with ExtDataID and maps it to DeviceName
    /// @param deviceName Name of device to use. Ex: u"ExtData"
    /// @param extDataID ID of archive to open.
    /// @return True on success. False on failure.
    bool open_extra_data(std::u16string_view deviceName, uint32_t extDataID);

    /// @brief Opens Shared Extra Data archive and maps it to DeviceName
    /// @param deviceName Name of device to use. Ex: u"SharedExtData"
    /// @param sharedExtDataID ID of archive to open.
    /// @return True on success. False on failure.
    bool open_shared_extra_data(std::u16string_view deviceName, uint32_t sharedExtDataID);

    /// @brief Opens BOSS Extra Data archive and maps it to DeviceName
    /// @param deviceName Name of device to use. Ex: u"BossExtData"
    /// @param extDataID ID of archive to open.
    /// @return True on success. False on failure.
    bool open_boss_extra_data(std::u16string_view deviceName, uint32_t extDataID);

    /// @brief Opens system save data archive and maps it to DeviceName
    /// @param deviceName Name of device to use. Ex: u"SystemSave"
    /// @param uniqueID ID of archive to open.
    /// @return True on success. False on failure.
    bool open_system_save_data(std::u16string_view deviceName, uint32_t uniqueID);

    /// @brief Opens system save data archive of a system module and maps it to DeviceName
    /// @param deviceName Name of device to use. Ex: u"SystemModule"
    /// @param uniqueID ID of archive to open.
    /// @return True on success. False on failure.
    bool open_system_module_save_data(std::u16string_view deviceName, uint32_t uniqueID);

    /// @brief Opens the save data for the inserted game card and maps it to DeviceName.
    /// @param deviceName Name of device to use. Ex: u"GameCard"
    /// @return True on success. False on failure.
    bool open_gamecard_save_data(std::u16string_view deviceName);

    /// @brief Opens user save data and maps it to DeviceName.
    /// @param deviceName Name of device to use. Ex: u"UserSave"
    /// @param mediaType Media type of target title.
    /// @param lowerID Lower 32 bits of the title ID of the title. Ex: TitleID & 0xFFFFFFFF
    /// @param upperID Upper 32 bites of the title ID of the title. Ex: TitleID >> 32 & 0xFFFFFFFF
    /// @return
    bool open_user_save_data(std::u16string_view deviceName, FS_MediaType mediaType, uint32_t lowerID, uint32_t upperID);
} // namespace fslib
