#pragma once
#include "Directory.hpp"
#include "File.hpp"
#include "Path.hpp"
#include "SaveInfoReader.hpp"
#include "Storage.hpp"
#include "bis_file_system.hpp"
#include "dev.hpp"
#include "device.hpp"
#include "directory_functions.hpp"
#include "file_functions.hpp"
#include "save_file_system.hpp"
#include <string_view>
#include <switch.h>

namespace fslib
{
    /// @brief Initializes FsLib.
    /// @note Once FsLib::Dev is implemented for Switch this will get more interesting.
    /// @return True on success. False on failure.
    bool initialize(void);

    /// @brief Exits FsLib closing any remaining open devices.
    void exit(void);

    /// @brief Returns the internal error string for slightly more descriptive errors than a bool.
    /// @return Internal error string.
    const char *get_error_string(void);

    /**
     * @brief Maps FileSystem to DeviceName internally.
     *
     * @param deviceName Name to use for Device.
     * @param fileSystem FileSystem to map to DeviceName.
     * @return True on success. False on failure.
     * @note If a FileSystem is already mapped to DeviceName, it <b>will</b> be unmounted and replaced with FileSystem instead of just
     * returning NULL like fs_dev. There is also <b>no</b> real limit to how many devices you can have open besides the Switch handle limit.
     * fs_dev only allows 32 at a time.
     */
    bool map_file_system(std::string_view deviceName, FsFileSystem *fileSystem);

    /// @brief Attempts to find Device in map.
    /// @param deviceName Name of the Device to locate.
    /// @param fileSystemOut Set to pointer to FileSystem handle mapped to DeviceName.
    /// @return True if DeviceName is found, false if it isn't.
    /// @note This isn't really useful outside of internal FsLib functions, but I don't want to hide it like archive_dev does in ctrulib.
    bool get_file_system_by_device_name(std::string_view deviceName, FsFileSystem **fileSystemOut);

    /// @brief Attempts to commit data to DeviceName.
    /// @param deviceName Name of device to commit data to.
    /// @return True on success. False on failure.
    bool commit_data_to_file_system(std::string_view deviceName);

    /**
     * @brief Attempts to get the free space available on Device passed.
     *
     * @param deviceRoot Root of device.
     * @param sizeOut The size retrieved if successful.
     * @return True on success. False on failure.
     * @note This function requires a path to work. DeviceRoot should be `sdmc:/` instead of `sdmc`, for example.
     */
    bool get_device_free_space(const fslib::Path &deviceRoot, int64_t &sizeOut);

    /**
     * @brief Attempts to get the total space of Device passed.
     *
     * @param deviceRoot Root of device.
     * @param sizeOut The size retrieved if successful.
     * @return True on success. False on failure.
     * @note This function requires a path to work. DeviceRoot should be `sdmc:/` instead of `sdmc`, for example.
     */
    bool get_device_total_space(const fslib::Path &deviceRoot, int64_t &sizeOut);

    /// @brief Closes filesystem mapped to DeviceName
    /// @param deviceName Name of device to close.
    /// @return True on success. False on Failure or device not found.
    bool close_file_system(std::string_view deviceName);
} // namespace fslib
