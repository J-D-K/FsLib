#pragma once
#include "Directory.hpp"
#include "File.hpp"
#include "Path.hpp"
#include "dev.hpp"
#include "directory_functions.hpp"
#include "error.hpp"
#include "extra_data.hpp"
#include "file_functions.hpp"
#include "save_data_archive.hpp"
#include "secure_value.hpp"

#include <3ds.h>
#include <string_view>

namespace fslib
{
    /// @brief Opens and mounts SD card to u"sdmc:/"
    bool initialize();

    /// @brief Exits and closes all open handles.
    void exit();

    /// @brief Adds Archive to devices.
    /// @param deviceName Name of the device. Ex: u"sdmc".
    /// @param archive Archive to map.
    bool map_archive(std::u16string_view deviceName, FS_Archive archive);

    /// @brief Attempts to retrieve the archive mapped to DeviceName.
    /// @param deviceName Name of the archive to retrieve. Ex: u"sdmc"
    /// @param archiveOut Pointer to Archive to write to.
    bool get_archive_by_device_name(std::u16string_view deviceName, FS_Archive &archiveOut);

    /// @brief Performs control on DeviceName AKA commits data to it. This is not required for Extra Data types or SDMC.
    /// @param deviceName Name of the device to control.
    bool control_device(std::u16string_view deviceName);

    /// @brief Closes the archive mapped to DeviceName.
    /// @param deviceName Name of the device to close.
    bool close_device(std::u16string_view deviceName);
} // namespace fslib
