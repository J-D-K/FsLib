#include "string.hpp"
#include <cstdarg>

namespace
{
    // Buffer size for va_list strings.
    constexpr int VA_BUFFER_SIZE = 0x1000;
} // namespace

std::string string::get_formatted_string(const char *format, ...)
{
    char vaBuffer[VA_BUFFER_SIZE] = {0};

    std::va_list vaList;
    va_start(vaList, format);
    vsnprintf(vaBuffer, VA_BUFFER_SIZE, format, vaList);
    va_end(vaList);

    return std::string(vaBuffer);
}
