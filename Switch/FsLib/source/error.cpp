#include "error.hpp"

#include <cstdarg>

namespace
{
    // Buffer size for va_list strings.
    constexpr int VA_BUFFER_SIZE = 0x1000;

    /// @brief This is the internal error string.
    constinit char s_errorBuffer[VA_BUFFER_SIZE] = {0};
} // namespace

const char *fslib::error::get_string() { return s_errorBuffer; }

bool fslib::error::occurred(Result code, const std::source_location &location)
{
    if (code == 0) { return false; }

    // I just want the source file. Not the whole path.
    std::string_view filename = location.file_name();
    size_t nameBegin          = filename.find_last_of('/');
    if (nameBegin != filename.npos) { filename = filename.substr(nameBegin + 1); }

    // I don't want the return type for this.
    std::string_view functionName = location.function_name();
    size_t functionBegin          = functionName.find_first_of(' ');
    if (functionBegin != functionName.npos) { functionName = functionName.substr(functionBegin + 1); }

    std::snprintf(s_errorBuffer,
                  VA_BUFFER_SIZE,
                  "fslib::%s::%s::%i:%X",
                  filename.data(),
                  functionName.data(),
                  location.line(),
                  code);

    return true;
}
