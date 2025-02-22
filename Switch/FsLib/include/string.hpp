#pragma once
#include <string>

/// @brief Contains the function for constructing error strings.
namespace string
{
    /// @brief This is just used internally to make setting the error string easier.
    /// @param format Format of string.
    /// @param arguments
    /// @return Formatted string?
    std::string get_formatted_string(const char *Format, ...);
} // namespace string
