#pragma once
#include "Directory.hpp"
#include "File.hpp"
#include "Path.hpp"
#include "SaveInfoReader.hpp"
#include "Storage.hpp"
#include "bis_file_system.hpp"
#include "commit.hpp"
#include "dev.hpp"
#include "device.hpp"
#include "device_space.hpp"
#include "directory_functions.hpp"
#include "error.hpp"
#include "file_functions.hpp"
#include "save_file_system.hpp"

#include <string_view>
#include <switch.h>

namespace fslib
{
    /// @brief Returns whether or not FsLibCore was initialized successfully.
    /// @return
    bool is_initialized();

    /**
     * @brief Maps FileSystem to DeviceName internally.
     *
     * @param deviceName Name to use for Device.
     * @param filesystem FileSystem to map to DeviceName.
     * @return True on success. False on failure.
     * @note If a FileSystem is already mapped to DeviceName, it <b>will</b> be unmounted and replaced with FileSystem instead
     * of just returning NULL like fs_dev. There is also <b>no</b> real limit to how many devices you can have open besides the
     * Switch handle limit. fs_dev only allows 32 at a time.
     */
    bool map_file_system(std::string_view deviceName, FsFileSystem &filesystem);

    /// @brief Attempts to find Device in map.
    /// @param deviceName Name of the Device to locate.
    /// @param filesystemOut Set to pointer to FileSystem handle mapped to DeviceName.
    /// @return True if DeviceName is found, false if it isn't.
    /// @note This isn't really useful outside of internal FsLib functions, but I don't want to hide it like archive_dev does in
    /// ctrulib.
    bool get_file_system_by_device_name(std::string_view deviceName, FsFileSystem **filesystem);

    /// @brief Closes filesystem mapped to DeviceName
    /// @param deviceName Name of device to close.
    /// @return True on success. False on Failure or device not found.
    bool close_file_system(std::string_view deviceName);
} // namespace fslib
