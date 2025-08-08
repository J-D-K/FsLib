#pragma once
#include <string>

/// @brief Contains the function for constructing error strings.
namespace string
{
    /// @brief Used internally to create error strings. This is not exposed to the user of the library.
    /// @param format Format of strings
    /// @param arguments Arguments
    /// @return String containing formatted output.
    std::string get_formatted(const char *format, ...);
} // namespace string
