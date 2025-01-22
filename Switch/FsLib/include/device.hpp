#pragma once

namespace fslib
{
    namespace device
    {
        /// @brief Initializes operations that require an FsDeviceOperator.
        /// @return True on success. False on failure.
        bool initialize(void);

        /// @brief Cleans up fslib::device
        void exit(void);

        /// @brief Returns whether or not an sd card is inserted.
        /// @return True if one is. False if one isn't or failure.
        bool sdIsInserted(void);

        /// @brief Returns whether a game card is inserted or not.
        /// @return True if one is. False if not or error.
        bool gameCardIsInserted(void);
    } // namespace device
} // namespace fslib
