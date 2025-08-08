#pragma once
#include <cstdint>

namespace fslib
{
    /// @brief Attempts to retrieve the secure value of the UniqueID passed.
    /// @param uniqueID UniqueID, usually the lower title ID >> 8, of the value to retrieve.
    /// @param secureValue uint64_t to which the value is written.
    /// @return True on success. False on failure/value doesn't exist.
    bool get_secure_value_for_title(uint32_t uniqueID, uint64_t &secureValue);

    /// @brief Attempts to set the secure value the UniqueID passed.
    /// @param uniqueID UniqueID of the target title.
    /// @param secureValue Value of secure value.
    /// @return True on success, false on failure.
    bool set_secure_value_for_title(uint32_t uniqueID, uint64_t secureValue);
} // namespace fslib
