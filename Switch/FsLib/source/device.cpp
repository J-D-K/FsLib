#include "fslib.hpp"
#include "string.hpp"

namespace
{
    // I think you only really need one of these?
    FsDeviceOperator s_deviceOperator;
} // namespace

extern std::string g_fslibErrorString;

bool fslib::device::initialize()
{
    Result fsError = fsOpenDeviceOperator(&s_deviceOperator);
    if (R_FAILED(fsError))
    {
        g_fslibErrorString = string::get_formatted_string("Error opening device operator: 0x%X.", fsError);
        return false;
    }
    return true;
}

void fslib::device::exit()
{
    // Maybe I should check if this is even open?
    fsDeviceOperatorClose(&s_deviceOperator);
}

bool fslib::device::sd_is_inserted()
{
    bool sdInserted = false;
    Result fsError = fsDeviceOperatorIsSdCardInserted(&s_deviceOperator, &sdInserted);
    if (R_FAILED(fsError))
    {
        g_fslibErrorString = string::get_formatted_string("Error detecting if SD card is inserted: 0x%X.", fsError);
        return false;
    }
    return sdInserted;
}

bool fslib::device::gamecard_is_inserted()
{
    bool gameCardInserted = false;
    Result fsError = fsDeviceOperatorIsGameCardInserted(&s_deviceOperator, &gameCardInserted);
    if (R_FAILED(fsError))
    {
        g_fslibErrorString = string::get_formatted_string("Error detecting is game card is inserted: 0x%X.", fsError);
        return false;
    }
    return gameCardInserted;
}
