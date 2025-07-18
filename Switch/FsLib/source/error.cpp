#include "error.hpp"

#include <cstdarg>

namespace
{
    // Buffer size for va_list strings.
    constexpr int VA_BUFFER_SIZE = 0x1000;

    /// @brief This is the internal error string.
    std::string s_errorString{};
} // namespace

const char *fslib::error::get_string() { return s_errorString.c_str(); }

bool fslib::error::occurred(Result code, const std::source_location &location)
{
    if (code != 0)
    {
        // I just want the source file. Not the whole path.
        std::string_view filename = location.file_name();
        size_t nameBegin          = filename.find_last_of('/');
        if (nameBegin != filename.npos) { filename = filename.substr(nameBegin + 1); }

        // I don't want the return type for this.
        std::string_view functionName = location.function_name();
        size_t functionBegin          = functionName.find_first_of(' ');
        if (functionBegin != functionName.npos) { functionName = functionName.substr(functionBegin + 1); }

        char errorBuffer[VA_BUFFER_SIZE] = {0};
        std::snprintf(errorBuffer,
                      VA_BUFFER_SIZE,
                      "fslib::%s::%s::%i:%X",
                      filename.data(),
                      functionName.data(),
                      location.line(),
                      code);

        s_errorString.assign(errorBuffer);
    }
    return code != 0;
}
