#include "fslib.hpp"
#include "dev.hpp"
#include "errorCommon.h"
#include "string.hpp"
#include <cstring>
#include <unordered_map>

namespace
{
    // This is always what the sd card is mounted to.
    constexpr std::string_view SD_CARD_DEVICE = "sdmc";
    // FsFileSystems paired with their mount point.
    std::unordered_map<std::string_view, FsFileSystem> s_deviceMap;
} // namespace

// This error string is shared globally, but I didn't want it extern'd in the header.
std::string g_fslibErrorString = "No errors encountered.";

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
    for (auto &[deviceName, fileSystem] : s_deviceMap)
    {
        // This is call directly instead of closeFileSystem because that guards against closing the SD.
        fsFsClose(&fileSystem);
    }
}

const char *fslib::get_error_string()
{
    return g_fslibErrorString.c_str();
}

bool fslib::map_file_system(std::string_view deviceName, FsFileSystem *fileSystem)
{
    if (deviceName == SD_CARD_DEVICE)
    {
        g_fslibErrorString = "Error: sdmc is a reserved device name.";
        return false;
    }

    if (device_name_is_in_use(deviceName))
    {
        fslib::close_file_system(deviceName);
    }

    std::memcpy(&s_deviceMap[deviceName], fileSystem, sizeof(FsFileSystem));

    return true;
}

bool fslib::get_file_system_by_device_name(std::string_view deviceName, FsFileSystem **fileSystemOut)
{
    if (!device_name_is_in_use(deviceName))
    {
        return false;
    }
    *fileSystemOut = &s_deviceMap[deviceName];
    return true;
}

bool fslib::commit_data_to_file_system(std::string_view deviceName)
{
    if (!device_name_is_in_use(deviceName))
    {
        g_fslibErrorString = ERROR_DEVICE_NOT_FOUND;
        return false;
    }

    Result fsError = fsFsCommit(&s_deviceMap[deviceName]);
    if (R_FAILED(fsError))
    {
        g_fslibErrorString = string::get_formatted_string("Error committing data to device: 0x%X.", fsError);
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
        g_fslibErrorString = ERROR_INVALID_PATH;
        return false;
    }

    if (!device_name_is_in_use(deviceRoot.get_device_name()))
    {
        g_fslibErrorString = ERROR_DEVICE_NOT_FOUND;
        return false;
    }

    Result fsError = fsFsGetFreeSpace(&s_deviceMap[deviceRoot.get_device_name()], deviceRoot.get_path(), &sizeOut);
    if (R_FAILED(fsError))
    {
        g_fslibErrorString = string::get_formatted_string("Error getting device free space: 0x%X.", fsError);
        return false;
    }

    return true;
}

bool fslib::get_device_total_space(const fslib::Path &deviceRoot, int64_t &sizeOut)
{
    sizeOut = -1;

    if (!deviceRoot.is_valid())
    {
        g_fslibErrorString = ERROR_INVALID_PATH;
        return false;
    }

    if (!device_name_is_in_use(deviceRoot.get_device_name()))
    {
        g_fslibErrorString = ERROR_DEVICE_NOT_FOUND;
        return false;
    }

    Result fsError = fsFsGetTotalSpace(&s_deviceMap[deviceRoot.get_device_name()], deviceRoot.get_path(), &sizeOut);
    if (R_FAILED(fsError))
    {
        g_fslibErrorString = string::get_formatted_string("Error getting device total space: 0x%X.", fsError);
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
    // Close file system.
    fsFsClose(&s_deviceMap[deviceName]);
    // Erase from map so everything works right.
    s_deviceMap.erase(deviceName);
    // Done
    return true;
}
