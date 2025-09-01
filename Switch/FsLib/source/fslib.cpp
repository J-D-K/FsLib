#include "fslib.hpp"

#include "dev.hpp"
#include "error.hpp"

#include <unordered_map>

namespace
{
    // This is always what the sd card is mounted to.
    constexpr std::string_view SD_CARD_DEVICE = "sdmc";

    // clang-format off
    struct StringViewHash
    {
        using is_transparent = void;
        size_t operator()(std::string_view view) const noexcept { return std::hash<std::string_view>{}(view); }
    };

    struct StringViewEqual
    {
        using is_transparent = void;
        bool operator()(std::string_view viewA, std::string_view viewB) const noexcept  { return viewA == viewB; }
    };
    // clang-format on

    // FsFileSystems paired with their mount point.
    std::unordered_map<std::string, FsFileSystem, StringViewHash, StringViewEqual> s_deviceMap;
} // namespace

bool fslib::initialize()
{
    FsFileSystem sdmc{};
    // I called it anyway. Call the cops devkitpro.
    const bool sdError = error::occurred(fsOpenSdCardFileSystem(&sdmc));
    if (sdError) { return false; }

    s_deviceMap.emplace(SD_CARD_DEVICE, sdmc);
    return true;
}

void fslib::exit()
{
    // This calls fsFsClose directly since close_filesystem guards against SD closure.
    for (auto &[deviceName, filesystem] : s_deviceMap) { fsFsClose(&filesystem); }
}

bool fslib::map_file_system(std::string_view deviceName, FsFileSystem *filesystem)
{
    const bool deviceIsSD = deviceName == SD_CARD_DEVICE;
    const bool inUse      = s_deviceMap.find(deviceName) != s_deviceMap.end();
    if (deviceIsSD || inUse)
    {
        error::occurred(error::codes::DEVICE_NAME_IN_USE);
        return false;
    }

    s_deviceMap.emplace(deviceName, *filesystem);
    return true;
}

bool fslib::get_file_system_by_device_name(std::string_view deviceName, FsFileSystem **filesystemOut)
{
    const auto device = s_deviceMap.find(deviceName);
    const bool inUse  = device != s_deviceMap.end();
    if (!inUse) { return false; }

    *filesystemOut = &device->second;
    return true;
}

bool fslib::commit_data_to_file_system(std::string_view deviceName)
{
    const auto device      = s_deviceMap.find(deviceName);
    const bool deviceInUse = device != s_deviceMap.end();
    const bool commitError = deviceInUse && error::occurred(fsFsCommit(&device->second));
    if (!deviceInUse || commitError) { return false; }

    return true;
}

int64_t fslib::get_device_free_space(const fslib::Path &deviceRoot)
{
    if (!deviceRoot.is_valid()) { return -1; }

    int64_t freeSpace{};
    const std::string_view deviceName = deviceRoot.get_device_name();
    const auto device                 = s_deviceMap.find(deviceName);
    const bool deviceInUse            = device != s_deviceMap.end();
    const bool spaceError =
        deviceInUse && error::occurred(fsFsGetFreeSpace(&device->second, deviceRoot.get_path(), &freeSpace));
    if (!deviceInUse || spaceError) { return -1; }

    return freeSpace;
}

int64_t fslib::get_device_total_space(const fslib::Path &deviceRoot)
{
    if (!deviceRoot.is_valid()) { return -1; }

    int64_t totalSpace{};
    const std::string_view deviceName = deviceRoot.get_device_name();
    const auto device                 = s_deviceMap.find(deviceName);
    const bool inUse                  = device != s_deviceMap.end();
    const bool spaceError = inUse && error::occurred(fsFsGetTotalSpace(&device->second, deviceRoot.get_path(), &totalSpace));
    if (!inUse || spaceError) { return -1; }

    return totalSpace;
}

bool fslib::close_file_system(std::string_view deviceName)
{
    // Block closing sdmc. Only exiting FsLib can do that.
    const bool isSD   = deviceName == SD_CARD_DEVICE;
    const auto device = s_deviceMap.find(deviceName);
    if (isSD || device == s_deviceMap.end()) { return false; }

    fsFsClose(&device->second);
    s_deviceMap.erase(device);
    return true;
}
