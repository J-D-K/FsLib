#include "error.hpp"
#include <cstdarg>

namespace
{
    // Buffer size for va_list strings.
    constexpr int VA_BUFFER_SIZE = 0x1000;

    /// @brief This is the internal error string.
    std::string s_errorString{};
} // namespace

const char *fslib::error::get_string() noexcept
{
    return s_errorString.c_str();
}

bool fslib::error::occurred(Result code, const std::source_location &location)
{
    if (code != 0)
    {
        // I just want the source file. Not the whole path.
        std::string_view filename = location.file_name();
        size_t nameBegin = filename.find_last_of('/');

        char errorBuffer[VA_BUFFER_SIZE] = {0};
        std::snprintf(errorBuffer,
                      VA_BUFFER_SIZE,
                      "%s::%s::%i : 0x%X",
                      filename.substr(nameBegin + 1).data(),
                      location.function_name(),
                      location.line(),
                      code);

        s_errorString.assign(errorBuffer);
    }
    return code != 0;
}
