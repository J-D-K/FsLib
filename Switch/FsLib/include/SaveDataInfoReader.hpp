#pragma once
#include <switch.h>

namespace fslib
{
    /// @brief Wrapper around FsSaveDataInfo reader.
    class SaveInfoReader
    {
        public:
            /// @brief Default constructor for save data info reader.
            SaveInfoReader(void) = default;

            /// @brief Constructor for SaveInfoReader. isOpen() can be checked if this was successful.
            /// @param saveDataSpaceID Save data space to open.
            SaveInfoReader(FsSaveDataSpaceId saveDataSpaceID);

            /// @brief Closes the info reader.
            ~SaveInfoReader();

            /// @brief Opens a save data info reader with saveDataSpaceID. isOpen() can be checked is this was successful.
            /// @param saveDataSpaceID Save data space to open.
            void open(FsSaveDataSpaceId saveDataSpaceID);

            /// @brief Closes the save data info reader. This is called in the destructor too.
            void close(void);

            /// @brief Returns if save data info reader was successfully opened.
            /// @return True on success. False on failure.
            bool isOpen(void) const;

            /// @brief Reads the next save data info entry.
            /// @return True on success. False on failure.
            bool read(void);

            /// @brief Returns a reference to the currently held FsSaveDataInfo struct.
            /// @return Reference to save data info.
            FsSaveDataInfo &get(void);

        private:
            /// @brief Underlying FsSaveDataInfoReader.
            FsSaveDataInfoReader m_infoReader;
            /// @brief Saves whether opening the reader was successful.
            bool m_isOpen = false;
            /// @brief Save data info struct.
            FsSaveDataInfo m_saveInfo = {0};
    };
}; // namespace fslib
