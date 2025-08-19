#include "error.hpp"

#include "string.hpp"

#include <string>

namespace
{
    std::string s_errorString = "No errors encountered.";
}

// Defined at bottom.
static void prep_locations(const std::source_location &location, std::string_view &file, std::string_view &function);

const char *fslib::error::get_string() { return s_errorString.c_str(); }

bool fslib::error::libctru(Result code, const std::source_location &location)
{
    if (code == 0) { return false; }

    std::string_view filename{}, function{};
    prep_locations(location, filename, function);
    s_errorString = string::get_formatted("%s::%s::%u::%u:%08X",
                                          filename.data(),
                                          function.data(),
                                          location.line(),
                                          location.column(),
                                          code);

    return true;
}

void fslib::error::set_code(uint32_t code, const std::source_location &location)
{
    std::string_view filename{}, function{};
    prep_locations(location, filename, function);
    s_errorString = string::get_formatted("%s::%s::%u::%u:%08X",
                                          filename.data(),
                                          function.data(),
                                          location.line(),
                                          location.column(),
                                          code);
}

static void prep_locations(const std::source_location &location, std::string_view &file, std::string_view &function)
{
    file                   = location.file_name();
    const size_t nameBegin = file.find_last_of('/');
    if (nameBegin != file.npos) { file = file.substr(nameBegin + 1); }

    function                   = location.function_name();
    const size_t functionBegin = function.find_first_of(' ');
    if (functionBegin != function.npos) { function = function.substr(functionBegin + 1); }
}
