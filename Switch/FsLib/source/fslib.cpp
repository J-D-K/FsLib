#include "fslib.hpp"

#include "dev.hpp"
#include "error.hpp"

#include <map>

namespace
{
    // This is always what the sd card is mounted to.
    constexpr std::string_view SD_CARD_DEVICE = "sdmc";
    // FsFileSystems paired with their mount point.
    std::map<std::string_view, FsFileSystem> s_deviceMap;
} // namespace

// This is for opening functions to search and make sure there are no duplicate uses of the same device name.
static inline bool device_name_is_in_use(std::string_view deviceName)
{
    return s_deviceMap.find(deviceName) != s_deviceMap.end();
}

bool fslib::initialize()
{
    FsFileSystem sdmc{};
    // I called it anyway. Call the cops devkitpro.
    const bool sdError = error::occurred(fsOpenSdCardFileSystem(&sdmc));
    if (sdError) { return false; }

    s_deviceMap[SD_CARD_DEVICE] = sdmc;
    return true;
}

void fslib::exit()
{
    for (auto &[deviceName, filesystem] : s_deviceMap)
    {
        // This is called directly instead of closeFileSystem because that guards against closing the SD.
        fsFsClose(&filesystem);
    }
}

bool fslib::map_file_system(std::string_view deviceName, FsFileSystem *filesystem)
{
    const bool deviceIsSD  = deviceName == SD_CARD_DEVICE;
    const bool deviceIsUse = device_name_is_in_use(deviceName);
    if (deviceIsSD || deviceIsUse)
    {
        error::occurred(error::codes::DEVICE_NAME_IN_USE);
        return false;
    }
    s_deviceMap[deviceName] = *filesystem;
    return true;
}

bool fslib::get_file_system_by_device_name(std::string_view deviceName, FsFileSystem **filesystemOut)
{
    if (!device_name_is_in_use(deviceName))
    {
        error::occurred(error::codes::DEVICE_NOT_FOUND);
        return false;
    }
    *filesystemOut = &s_deviceMap[deviceName];
    return true;
}

bool fslib::commit_data_to_file_system(std::string_view deviceName)
{
    const bool deviceInUse = device_name_is_in_use(deviceName);
    const bool commitError = deviceInUse && error::occurred(fsFsCommit(&s_deviceMap[deviceName]));
    if (!deviceInUse || commitError) { return false; }
    return true;
}

int64_t fslib::get_device_free_space(const fslib::Path &deviceRoot)
{
    if (!deviceRoot.is_valid()) { return -1; }

    int64_t freeSpace{};
    const std::string_view device = deviceRoot.get_device_name();
    const char *path              = deviceRoot.get_path();
    const bool deviceInUse        = device_name_is_in_use(device);
    const bool spaceError         = deviceInUse && error::occurred(fsFsGetFreeSpace(&s_deviceMap[device], path, &freeSpace));
    if (!deviceInUse || spaceError) { return -1; }

    return freeSpace;
}

int64_t fslib::get_device_total_space(const fslib::Path &deviceRoot)
{
    if (!deviceRoot.is_valid()) { return -1; }

    int64_t totalSpace{};
    const std::string_view device = deviceRoot.get_device_name();
    const char *path              = deviceRoot.get_path();
    const bool deviceInUse        = device_name_is_in_use(device);
    const bool spaceError         = deviceInUse && error::occurred(fsFsGetTotalSpace(&s_deviceMap[device], path, &totalSpace));
    if (!deviceInUse || spaceError) { return -1; }

    return totalSpace;
}

bool fslib::close_file_system(std::string_view deviceName)
{
    // Block closing sdmc. Only exiting FsLib can do that.
    const bool isSD  = deviceName == SD_CARD_DEVICE;
    const bool inUse = device_name_is_in_use(deviceName);
    if (isSD || !inUse) { return false; }
    fsFsClose(&s_deviceMap[deviceName]);
    s_deviceMap.erase(deviceName);
    return true;
}
