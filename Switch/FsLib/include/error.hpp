#pragma once
#include <source_location>
#include <string>
#include <switch.h>

/// @brief Contains the function for constructing error strings.

namespace fslib
{
    namespace error
    {
        /// @brief These are codes specific to fslib.
        namespace codes
        {
            static constexpr uint32_t MAP_DEVICE           = 1;
            static constexpr uint32_t DEVICE_NOT_FOUND     = 2;
            static constexpr uint32_t INVALID_PATH         = 3;
            static constexpr uint32_t SDMC_RESERVED        = 4;
            static constexpr uint32_t DEVICE_NAME_IN_USE   = 5;
            static constexpr uint32_t NOT_OPEN_FOR_READING = 6;
            static constexpr uint32_t NOT_OPEN_FOR_WRITING = 7;
            static constexpr uint32_t UNABLE_TO_RESIZE     = 8;
        } // namespace codes

        /// @brief Returns the internal error string.
        const char *get_string();

        /// @brief Creates an error string.
        bool occurred(Result code, const std::source_location &location = std::source_location::current());
    } // namespace error
} // namespace fslib
