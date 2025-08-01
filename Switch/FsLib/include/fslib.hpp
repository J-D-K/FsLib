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
#include "error.hpp"
#include "file_functions.hpp"
#include "save_file_system.hpp"

#include <string_view>
#include <switch.h>

namespace fslib
{
    /// @brief Initializes FsLib.
    /// @note Once FsLib::Dev is implemented for Switch this will get more interesting.
    /// @return True on success. False on failure.
    bool initialize();

    /// @brief Exits FsLib closing any remaining open devices.
    void exit();

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
    bool map_file_system(std::string_view deviceName, FsFileSystem *filesystem);

    /// @brief Attempts to find Device in map.
    /// @param deviceName Name of the Device to locate.
    /// @param filesystemOut Set to pointer to FileSystem handle mapped to DeviceName.
    /// @return True if DeviceName is found, false if it isn't.
    /// @note This isn't really useful outside of internal FsLib functions, but I don't want to hide it like archive_dev does in
    /// ctrulib.
    bool get_file_system_by_device_name(std::string_view deviceName, FsFileSystem **filesystemOut);

    /// @brief Attempts to commit data to DeviceName.
    /// @param deviceName Name of device to commit data to.
    /// @return True on success. False on failure.
    bool commit_data_to_file_system(std::string_view deviceName);

    /**
     * @brief Attempts to get the free space available on Device passed.
     *
     * @param deviceRoot Root of device.
     * @param sizeOut The size retrieved if successful.
     * @return Size on success. -1 on failure.
     * @note This function requires a path to work. DeviceRoot should be `sdmc:/` instead of `sdmc`, for example.
     */
    int64_t get_device_free_space(const fslib::Path &deviceRoot);

    /**
     * @brief Attempts to get the total space of Device passed.
     *
     * @param deviceRoot Root of device.
     * @param sizeOut The size retrieved if successful.
     * @return Size on success. -1 on failure.
     * @note This function requires a path to work. DeviceRoot should be `sdmc:/` instead of `sdmc`, for example.
     */
    int64_t get_device_total_space(const fslib::Path &deviceRoot);

    /// @brief Closes filesystem mapped to DeviceName
    /// @param deviceName Name of device to close.
    /// @return True on success. False on Failure or device not found.
    bool close_file_system(std::string_view deviceName);
} // namespace fslib
