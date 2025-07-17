#include "error.hpp"
#include "fslib.hpp"

namespace
{
    // I think you only really need one of these?
    FsDeviceOperator s_deviceOperator;
} // namespace

bool fslib::device::initialize()
{
    const bool openError = error::occurred(fsOpenDeviceOperator(&s_deviceOperator));
    return !openError;
}

void fslib::device::exit()
{
    // Maybe I should check if this is even open?
    fsDeviceOperatorClose(&s_deviceOperator);
}

bool fslib::device::sd_is_inserted()
{
    bool sdInserted      = false;
    const bool readError = error::occurred(fsDeviceOperatorIsSdCardInserted(&s_deviceOperator, &sdInserted));
    if(readError) { return false; }
    return sdInserted;
}

bool fslib::device::gamecard_is_inserted()
{
    bool gameCardInserted = false;
    const bool readError  = error::occurred(fsDeviceOperatorIsGameCardInserted(&s_deviceOperator, &gameCardInserted));
    if(readError) { return false; }
    return gameCardInserted;
}
