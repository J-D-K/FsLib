#pragma once
#include "Directory.hpp"
#include "File.hpp"
#include "Path.hpp"
#include "dev.hpp"
#include "directoryFunctions.hpp"
#include "extData.hpp"
#include "fileFunctions.hpp"
#include "saveDataArchive.hpp"
#include "secureValue.hpp"
#include <3ds.h>
#include <string>

namespace fslib
{
    /// @brief Opens and mounts SD card to u"sdmc:/"
    /// @return True on success. False on failure.
    bool initialize();

    /// @brief Exits and closes all open handles.
    void exit();

    /// @brief Returns internal error string with slightly more information than just a bool can provide.
    /// @return Error string.
    const char *getErrorString();

    /// @brief Adds Archive to devices.
    /// @param deviceName Name of the device. Ex: u"sdmc".
    /// @param archive Archive to map.
    /// @return True on success. False on failure.
    bool mapArchiveToDevice(std::u16string_view deviceName, FS_Archive archive);

    /// @brief Attempts to retrieve the archive mapped to DeviceName.
    /// @param deviceName Name of the archive to retrieve. Ex: u"sdmc"
    /// @param archiveOut Pointer to Archive to write to.
    /// @return True if the archive is found. False if it is not.
    bool getArchiveByDeviceName(std::u16string_view deviceName, FS_Archive *archiveOut);

    /// @brief Processes and returns if the path is valid and the archive exists and is open.
    /// @param path Path to process.
    /// @param archiveOut FS_Archive retrieved from map.
    /// @return True on success. False on failure.
    bool processDeviceAndPath(const fslib::Path &path, FS_Archive *archiveOut);

    /// @brief Performs control on DeviceName AKA commits data to it. This is not required for Extra Data types or SDMC.
    /// @param deviceName Name of the device to control.
    /// @return True on success. False on failure.
    bool controlDevice(std::u16string_view deviceName);

    /// @brief Closes the archive mapped to DeviceName.
    /// @param deviceName Name of the device to close.
    /// @return True on success. False on failure.
    bool closeDevice(std::u16string_view deviceName);
} // namespace fslib
