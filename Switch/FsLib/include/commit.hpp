#pragma once
#include <string_view>

namespace fslib
{
    /// @brief Attempts to commit data to DeviceName.
    /// @param deviceName Name of device to commit data to.
    /// @return True on success. False on failure.
    bool commit_data_to_file_system(std::string_view deviceName);
}