#pragma once
#include <string_view>
#include <switch.h>

namespace fslib
{
    /// @brief Opens partition with ID passed.
    /// @param deviceName Name to map to partition.
    /// @param partitionID ID of BIS partition.
    /// @return True on success. False on failure.
    bool open_bis_filesystem(std::string_view deviceName, FsBisPartitionId partitionID);
} // namespace fslib
