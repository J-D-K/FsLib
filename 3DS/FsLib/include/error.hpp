#pragma once
#include <3ds.h>
#include <source_location>

namespace fslib
{
    namespace error
    {
        /// @brief Returns the internal error string.
        const char *get_string();

        /// @brief Returns whether or not an error occurred while using a libctru function. If so, it's recorded to the internal
        /// error string.
        /// @param code Code to check.
        bool libctru(Result code, const std::source_location &location = std::source_location::current());
    }
}
