#include "fslib.hpp"
#include "string.hpp"
#include <3ds.h>
#include <unordered_map>

namespace
{
    // 3DS can use UTF-16 paths so that's what we're using.
    std::unordered_map<std::u16string_view, FS_Archive> s_deviceMap;
    // This only works because the string is so short.
    constexpr std::u16string_view SDMC_DEVICE_NAME = u"sdmc";
} // namespace

// Globally shared error string.
std::string g_fslibErrorString = "No errors encountered.";

// Checks if device is already in map.
static inline bool deviceNameIsInUse(std::u16string_view deviceName)
{
    return s_deviceMap.find(deviceName) != s_deviceMap.end();
}

bool fslib::initialize()
{
    // Try to init FS just in case;
    Result fsError = fsInit();
    if (R_FAILED(fsError))
    {
        g_fslibErrorString = string::getFormattedString("Error initializing FS service: 0x%08X.", fsError);
        return false;
    }
    // Open sdmc
    fsError = FSUSER_OpenArchive(&s_deviceMap[SDMC_DEVICE_NAME], ARCHIVE_SDMC, {PATH_EMPTY, 0x00, NULL});
    if (R_FAILED(fsError))
    {
        g_fslibErrorString = string::getFormattedString("Error opening SDMC archive: 0x%08X.", fsError);
    }
    return true;
}

void fslib::exit()
{
    for (auto &[deviceName, archive] : s_deviceMap)
    {
        FSUSER_CloseArchive(archive);
    }
    fsExit();
}

const char *fslib::getErrorString()
{
    return g_fslibErrorString.c_str();
}

bool fslib::mapArchiveToDevice(std::u16string_view deviceName, FS_Archive archive)
{
    // Guard against overwriting sdmc
    if (deviceName == u"sdmc")
    {
        g_fslibErrorString = "Error mapping device: sdmc is reserved device name.";
        return false;
    }

    // Close it so we don't leave an archive dangling.
    if (deviceNameIsInUse(deviceName))
    {
        fslib::closeDevice(deviceName);
    }
    // This is just a uint64_t, so I'm not going to bother memcpying like on Switch.
    s_deviceMap[deviceName] = archive;

    return true;
}

bool fslib::getArchiveByDeviceName(std::u16string_view deviceName, FS_Archive *archiveOut)
{
    if (!deviceNameIsInUse(deviceName))
    {
        return false;
    }
    *archiveOut = s_deviceMap[deviceName];

    return true;
}

bool fslib::processDeviceAndPath(const fslib::Path &path, FS_Archive *archiveOut)
{
    if (!path.isValid() || !deviceNameIsInUse(path.getDevice()))
    {
        g_fslibErrorString = "Error fetching device: Invalid path supplied or device not found.";
        return false;
    }
    *archiveOut = s_deviceMap[path.getDevice()];

    return true;
}

bool fslib::controlDevice(std::u16string_view deviceName)
{
    if (!deviceNameIsInUse(deviceName))
    {
        return false;
    }

    Result fsError = FSUSER_ControlArchive(s_deviceMap[deviceName], ARCHIVE_ACTION_COMMIT_SAVE_DATA, NULL, 0, NULL, 0);
    if (R_FAILED(fsError))
    {
        g_fslibErrorString = string::getFormattedString("Error committing save data to device: 0x%08X.", fsError);
        return false;
    }

    return true;
}

bool fslib::closeDevice(std::u16string_view deviceName)
{
    if (!deviceNameIsInUse(deviceName))
    {
        return false;
    }

    Result fsError = FSUSER_CloseArchive(s_deviceMap[deviceName]);
    if (R_FAILED(fsError))
    {
        g_fslibErrorString = string::getFormattedString("Error closeing archive: 0x%08X.", fsError);
        return false;
    }
    // Erase the device from map so deviceNameIsInUse works correctly.
    s_deviceMap.erase(deviceName);

    return true;
}
