#include "FsLibCore.hpp"

#include "error.hpp"

namespace
{
    constexpr std::string_view SDMC_DEVICE = "sdmc";
}

bool FsLibCore::is_initialized() const noexcept { return m_initialized; }

FsLibCore::FsLibCore()
{
    FsFileSystem sdmc{};

    const bool sdmcError = fslib::error::occurred(fsOpenSdCardFileSystem(&sdmc));
    if (sdmcError) { return; }

    const auto emplaced = m_deviceMap.try_emplace(SDMC_DEVICE.data(), sdmc);
    if (!emplaced.second) { return; }

    m_initialized = true;
}

bool FsLibCore::map_file_system(std::string_view deviceName, FsFileSystem &filesystem)
{
    const bool inUse = m_deviceMap.find(deviceName) != m_deviceMap.end();
    if (inUse)
    {
        fslib::error::occurred(fslib::error::codes::DEVICE_NAME_IN_USE);
        return false;
    }

    const auto emplaced = m_deviceMap.try_emplace(deviceName.data(), filesystem);
    if (!emplaced.second) { return false; }

    return true;
}

bool FsLibCore::get_file_system_by_device_name(std::string_view deviceName, FsFileSystem **filesystem)
{
    const auto findDevice = m_deviceMap.find(deviceName);
    if (findDevice == m_deviceMap.end()) { return false; }

    *filesystem = findDevice->second.get();
    return true;
}

bool FsLibCore::close_file_system(std::string_view deviceName)
{
    const bool sdmcGuard  = deviceName == SDMC_DEVICE;
    const auto findDevice = m_deviceMap.find(deviceName);
    if (sdmcGuard || findDevice == m_deviceMap.end()) { return false; }

    m_deviceMap.erase(findDevice);
    return true;
}