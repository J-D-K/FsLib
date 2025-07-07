#include "fslib.hpp"
#include "dev.hpp"
#include "error.hpp"
#include <cstring>
#include <unordered_map>

namespace
{
    // This is always what the sd card is mounted to.
    constexpr std::string_view SD_CARD_DEVICE = "sdmc";
    // FsFileSystems paired with their mount point.
    std::unordered_map<std::string_view, FsFileSystem> s_deviceMap;
} // namespace

// This is for opening functions to search and make sure there are no duplicate uses of the same device name.
static inline bool device_name_is_in_use(std::string_view deviceName)
{
    return s_deviceMap.find(deviceName) != s_deviceMap.end();
}

bool fslib::initialize()
{
    // Init sdmc.
    FsFileSystem sdmc;
    // I called it anyway. Call the cops.
    Result fsError = fsOpenSdCardFileSystem(&sdmc);
    if (R_FAILED(fsError))
    {
        return false;
    }

    // Memcpy the handle to be 100% sure we have it 1:1.
    std::memcpy(&s_deviceMap[SD_CARD_DEVICE], &sdmc, sizeof(FsFileSystem));

    return true;
}

void fslib::exit()
{
    // Loop through and close all open devices in map.
    for (auto &[deviceName, filesystem] : s_deviceMap)
    {
        // This is call directly instead of closeFileSystem because that guards against closing the SD.
        fsFsClose(&filesystem);
    }
}

bool fslib::map_file_system(std::string_view deviceName, FsFileSystem *filesystem)
{
    if (deviceName == SD_CARD_DEVICE)
    {
        error::occurred(error::codes::SDMC_RESERVED);
        return false;
    }

    if (device_name_is_in_use(deviceName))
    {
        error::occurred(error::codes::DEVICE_NAME_IN_USE);
        return false;
    }

    std::memcpy(&s_deviceMap[deviceName], filesystem, sizeof(FsFileSystem));

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
    if (!device_name_is_in_use(deviceName))
    {
        return false;
    }

    if (error::occurred(fsFsCommit(&s_deviceMap[deviceName])))
    {
        return false;
    }
    return true;
}

bool fslib::get_device_free_space(const fslib::Path &deviceRoot, int64_t &sizeOut)
{
    // Preset to negative one in the event of an error.
    sizeOut = -1;

    if (!deviceRoot.is_valid())
    {
        return false;
    }

    if (!device_name_is_in_use(deviceRoot.get_device_name()))
    {
        return false;
    }

    if (error::occurred(fsFsGetFreeSpace(&s_deviceMap[deviceRoot.get_device_name()], deviceRoot.get_path(), &sizeOut)))
    {
        return false;
    }

    return true;
}

bool fslib::get_device_total_space(const fslib::Path &deviceRoot, int64_t &sizeOut)
{
    sizeOut = -1;

    if (!deviceRoot.is_valid())
    {
        return false;
    }

    if (!device_name_is_in_use(deviceRoot.get_device_name()))
    {
        return false;
    }

    if (error::occurred(fsFsGetTotalSpace(&s_deviceMap[deviceRoot.get_device_name()], deviceRoot.get_path(), &sizeOut)))
    {
        return false;
    }

    return true;
}

bool fslib::close_file_system(std::string_view deviceName)
{
    // Block closing sdmc. Only exiting FsLib can do that.
    if (deviceName == SD_CARD_DEVICE)
    {
        return false;
    }

    if (!device_name_is_in_use(deviceName))
    {
        return false;
    }
    fsFsClose(&s_deviceMap[deviceName]);
    s_deviceMap.erase(deviceName);

    return true;
}
