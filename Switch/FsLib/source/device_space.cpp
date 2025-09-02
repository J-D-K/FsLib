#include "device_space.hpp"

#include "error.hpp"
#include "fslib.hpp"

int64_t fslib::get_device_free_space(const fslib::Path &deviceRoot)
{
    FsFileSystem *filesystem{};
    const bool isValid = deviceRoot.is_valid();
    const bool found   = isValid && fslib::get_file_system_by_device_name(deviceRoot.get_device_name(), &filesystem);
    if (!isValid || !found) { return -1; }

    int64_t freeSpace{};
    const bool spaceError = error::occurred(fsFsGetFreeSpace(filesystem, deviceRoot.get_path(), &freeSpace));
    if (spaceError) { return -1; }

    return freeSpace;
}

int64_t fslib::get_device_total_space(const fslib::Path &deviceRoot)
{
    FsFileSystem *filesystem{};
    const bool isValid = deviceRoot.is_valid();
    const bool found   = isValid && fslib::get_file_system_by_device_name(deviceRoot.get_device_name(), &filesystem);
    if (!isValid || !found) { return -1; }

    int64_t totalSpace{};
    const bool spaceError = error::occurred(fsFsGetTotalSpace(filesystem, deviceRoot.get_path(), &totalSpace));
    if (spaceError) { return -1; }

    return totalSpace;
}