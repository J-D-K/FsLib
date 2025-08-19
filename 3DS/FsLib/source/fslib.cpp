#include "fslib.hpp"

#include "EmptyPath.hpp"
#include "string.hpp"

#include <3ds.h>
#include <map>
#include <string_view>

namespace
{
    // 3DS can use UTF-16 paths so that's what we're using.
    std::map<std::u16string, FS_Archive, std::less<>> s_deviceMap{};

    /// @brief This is reserved.
    constexpr std::u16string_view SDMC_DEVICE_NAME = u"sdmc";
} // namespace

// Checks if device is already in map.
static inline bool device_is_in_use(std::u16string_view deviceName);

bool fslib::initialize()
{
    const bool fsError = error::libctru(fsInit());
    if (fsError) { return false; }

    const std::u16string sdmcDevice{SDMC_DEVICE_NAME};
    FS_Archive &sdmc     = s_deviceMap[sdmcDevice];
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

    const std::u16string device{deviceName};
    s_deviceMap[device] = archive;
    return true;
}

bool fslib::get_archive_by_device_name(std::u16string_view deviceName, FS_Archive &archiveOut)
{
    if (!device_is_in_use(deviceName))
    {
        fslib::error::set_code(fslib::error::codes::DEVICE_NOT_FOUND);
        return false;
    }

    const std::u16string device{deviceName};
    archiveOut = s_deviceMap[device];
    return true;
}

bool fslib::control_device(std::u16string_view deviceName)
{
    if (!device_is_in_use(deviceName)) { return false; }

    const std::u16string device{deviceName};
    FS_Archive archive = s_deviceMap[device];
    const bool error = error::libctru(FSUSER_ControlArchive(archive, ARCHIVE_ACTION_COMMIT_SAVE_DATA, nullptr, 0, nullptr, 0));
    if (error) { return false; }

    return true;
}

bool fslib::close_device(std::u16string_view deviceName)
{
    if (!device_is_in_use(deviceName)) { return false; }

    const std::u16string device{deviceName};
    FS_Archive archive    = s_deviceMap[device];
    const bool closeError = error::libctru(FSUSER_CloseArchive(archive));
    if (closeError) { return false; }

    s_deviceMap.erase(device);
    return true;
}

static inline bool device_is_in_use(std::u16string_view deviceName)
{
    return s_deviceMap.find(deviceName) != s_deviceMap.end();
}
