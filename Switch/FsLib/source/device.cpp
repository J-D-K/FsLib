#include "fslib.hpp"
#include "string.hpp"

namespace
{
    // I think you only really need one of these?
    FsDeviceOperator s_deviceOperator;
} // namespace

extern std::string g_fslibErrorString;

bool fslib::device::initialize(void)
{
    Result fsError = fsOpenDeviceOperator(&s_deviceOperator);
    if (R_FAILED(fsError))
    {
        g_fslibErrorString = string::getFormattedString("Error opening device operator: 0x%X.", fsError);
        return false;
    }
    return true;
}

void fslib::device::exit(void)
{
    // Maybe I should check if this is even open?
    fsDeviceOperatorClose(&s_deviceOperator);
}

bool fslib::device::sdIsInserted(void)
{
    bool sdInserted = false;
    Result fsError = fsDeviceOperatorIsSdCardInserted(&s_deviceOperator, &sdInserted);
    if (R_FAILED(fsError))
    {
        g_fslibErrorString = string::getFormattedString("Error detecting if SD card is inserted: 0x%X.", fsError);
        return false;
    }
    return sdInserted;
}

bool fslib::device::gameCardIsInserted(void)
{
    bool gameCardInserted = false;
    Result fsError = fsDeviceOperatorIsGameCardInserted(&s_deviceOperator, &gameCardInserted);
    if (R_FAILED(fsError))
    {
        g_fslibErrorString = string::getFormattedString("Error detecting is game card is inserted: 0x%X.", fsError);
        return false;
    }
    return gameCardInserted;
}
