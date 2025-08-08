#include "fslib.hpp"

#include "EmptyPath.hpp"
#include "string.hpp"

#include <3ds.h>
#include <string_view>
#include <unordered_map>

namespace
{
    // 3DS can use UTF-16 paths so that's what we're using.
    std::unordered_map<std::u16string_view, FS_Archive> s_deviceMap;

    /// @brief This is reserved.
    constexpr std::u16string_view SDMC_DEVICE_NAME = u"sdmc";
} // namespace

// Checks if device is already in map.
static inline bool device_is_in_use(std::u16string_view deviceName);

bool fslib::initialize()
{
    const bool fsError = error::libctru(fsInit());
    if (fsError) { return false; }

    FS_Archive &sdmc     = s_deviceMap[SDMC_DEVICE_NAME];
    const bool sdmcError = error::libctru(FSUSER_OpenArchive(&sdmc, ARCHIVE_SDMC, EMPTY_PATH));
    if (sdmcError) { return false; }

    return true;
}

void fslib::exit()
{
    for (auto &[deviceName, archive] : s_deviceMap) { FSUSER_CloseArchive(archive); }
    fsExit();
}

bool fslib::map_archive(std::u16string_view deviceName, FS_Archive archive)
{
    const bool sdGuard = deviceName == SDMC_DEVICE_NAME;
    if (sdGuard) { return false; }

    const bool isInUse = device_is_in_use(deviceName);
    if (isInUse) { fslib::close_device(deviceName); }

    s_deviceMap[deviceName] = archive;
    return true;
}

bool fslib::get_archive_by_device_name(std::u16string_view deviceName, FS_Archive &archiveOut)
{
    if (!device_is_in_use(deviceName)) { return false; }
    archiveOut = s_deviceMap[deviceName];
    return true;
}

bool fslib::control_device(std::u16string_view deviceName)
{
    if (!device_is_in_use(deviceName)) { return false; }

    FS_Archive archive = s_deviceMap[deviceName];
    const bool controlError =
        error::libctru(FSUSER_ControlArchive(archive, ARCHIVE_ACTION_COMMIT_SAVE_DATA, nullptr, 0, nullptr, 0));
    if (controlError) { return false; }
    return true;
}

bool fslib::close_device(std::u16string_view deviceName)
{
    if (!device_is_in_use(deviceName)) { return false; }

    FS_Archive archive    = s_deviceMap[deviceName];
    const bool closeError = error::libctru(FSUSER_CloseArchive(archive));
    if (closeError) { return false; }

    s_deviceMap.erase(deviceName);
    return true;
}

static inline bool device_is_in_use(std::u16string_view deviceName)
{
    return s_deviceMap.find(deviceName) != s_deviceMap.end();
}
