#include "fslib.hpp"

#include "FsLibCore.hpp"
#include "dev.hpp"
#include "error.hpp"

#include <unordered_map>

namespace
{
    FsLibCore s_core{};
}

bool fslib::is_initialized() { return s_core.is_initialized(); }

bool fslib::map_file_system(std::string_view deviceName, FsFileSystem &filesystem)
{
    return s_core.map_file_system(deviceName, filesystem);
}

bool fslib::get_file_system_by_device_name(std::string_view deviceName, FsFileSystem **filesystemOut)
{
    return s_core.get_file_system_by_device_name(deviceName, filesystemOut);
}

bool fslib::close_file_system(std::string_view deviceName) { return s_core.close_file_system(deviceName); }
