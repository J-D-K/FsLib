#include "error.hpp"
#include "fslib.hpp"
#include "string.hpp"

#include <3ds.h>
#include <string>

bool fslib::delete_extra_data(FS_MediaType mediaType, uint32_t extraDataID)
{
    const FS_ExtSaveDataInfo extraInfo = {.mediaType = mediaType,
                                          .unknown   = 0,
                                          .reserved1 = 0,
                                          .saveId    = extraDataID,
                                          .reserved2 = 0};

    const bool deleteError = error::libctru(FSUSER_DeleteExtSaveData(extraInfo));
    if (deleteError) { return false; }
    return true;
}
