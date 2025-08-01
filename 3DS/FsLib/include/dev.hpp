#pragma once

namespace fslib
{
    /// @brief Contains the function for overriding archive_dev.
    namespace dev
    {
        /**
         * @brief Initializes a bare-bones compatibility layer so devkitPro libraries still work with the SD card of the 3DS.
         * @return True on success. False on failure.
         * @note In order to use this correctly, libctru's <b>__appInit()</b> and <b>__appExit()</b> must be overriden to not include
         * <b>archiveMountSdmc()</b> and <b>archiveUnmountAll</b>. This will only function for the sdmc and only for reading and writing files.
         * This is only in place so devkitPro libraries can still function and access files on sdmc. All other operations should use the
         * functions FsLib provides, FsLib::File, and FsLib::Directory.
         */
        bool initializeSDMC();
    } // namespace dev
} // namespace fslib
