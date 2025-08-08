#include "string.hpp"

#include <array>
#include <cstdarg>

namespace
{
    // Size of buffer for va args.
    constexpr int VA_BUFFER_SIZE = 0x1000;
} // namespace

std::string string::get_formatted(const char *format, ...)
{
    std::array<char, VA_BUFFER_SIZE> vaBuffer{0};

    std::va_list vaList{};
    va_start(vaList, format);
    std::vsnprintf(vaBuffer.data(), VA_BUFFER_SIZE, format, vaList);
    va_end(vaList);

    return std::string(vaBuffer.data());
}
