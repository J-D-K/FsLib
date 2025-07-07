#include "error.hpp"
#include "fslib.hpp"

namespace
{
    // I think you only really need one of these?
    FsDeviceOperator s_deviceOperator;
} // namespace

bool fslib::device::initialize()
{
    if (error::occurred(fsOpenDeviceOperator(&s_deviceOperator)))
    {
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
    if (error::occurred(fsDeviceOperatorIsSdCardInserted(&s_deviceOperator, &sdInserted)))
    {
        return false;
    }
    return sdInserted;
}

bool fslib::device::gamecard_is_inserted()
{
    bool gameCardInserted = false;
    if (error::occurred(fsDeviceOperatorIsGameCardInserted(&s_deviceOperator, &gameCardInserted)))
    {
        return false;
    }
    return gameCardInserted;
}
