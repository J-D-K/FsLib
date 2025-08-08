#pragma once
#include <3ds.h>
#include <cstdint>

namespace fslib
{
    /// @brief Attempts to delete extra data with ExtraDataID
    /// @param mediaType MediaType. Either MEDIA_TYPE_SD or MEDIA_TYPE_NAND can be passed. Most Extra Data is SD.
    /// @param extraDataID ID of extra data to delete.
    /// @return True on success. False on failure.
    bool delete_extra_data(FS_MediaType mediaType, uint32_t extraDataID);
} // namespace fslib
