#pragma once
#include <memory>
#include <switch.h>

namespace fslib
{
    /// @brief Wrapper around FsSaveDataInfo reader.
    class SaveInfoReader final
    {
        public:
            /// @brief Default constructor for save data info reader.
            SaveInfoReader() = default;

            /// @brief Constructor for SaveInfoReader. isOpen() can be checked if this was successful.
            /// @param saveDataSpaceID Save data space to open.
            /// @param bufferCount Number of FsSaveDataInfo entries to buffer while reading.
            SaveInfoReader(FsSaveDataSpaceId saveDataSpaceID, size_t bufferCount);

            /// @brief Opens a SaveInfoReader filtered the passed account ID.
            /// @param accountID Account ID to use to filter.
            /// @param bufferCount Number of FsSaveDataInfo entries to buffer while reading.
            SaveInfoReader(FsSaveDataSpaceId saveSpaceID, AccountUid accountID, size_t bufferCount);

            /// @brief Opens and SaveInfoReader filtered with the save data type passed.
            /// @param saveType Save data type to use to filter.
            /// @param bufferCount Number of FsSaveDataInfo entries to buffer while reading.
            SaveInfoReader(FsSaveDataSpaceId saveSpaceID, FsSaveDataType saveType, size_t buffer);

            /// @brief Closes the info reader.
            ~SaveInfoReader();

            /// @brief Opens a save data info reader with saveDataSpaceID. isOpen() can be checked is this was successful.
            /// @param saveDataSpaceID Save data space to open.
            /// @param bufferCount Number of FsSaveDataInfo entries to buffer while reading.
            void open(FsSaveDataSpaceId saveDataSpaceID, size_t bufferCount);

            /// @brief Opens a SaveInfoReader filtered with the passed account ID.
            /// @param accountID Account ID to use to filter.
            void open(FsSaveDataSpaceId saveSpaceID, AccountUid accountID, size_t bufferCount);

            /// @brief Opens a SaveInfoReader filtered with the save data type passed.
            /// @param saveType Save data type to use to filter.
            void open(FsSaveDataSpaceId saveSpaceID, FsSaveDataType saveType, size_t bufferCount);

            /// @brief Closes the save data info reader. This is called in the destructor too.
            void close() noexcept;

            /// @brief Returns if save data info reader was successfully opened.
            /// @return True on success. False on failure.
            bool is_open() const noexcept;

            /// @brief Reads the next save data info entry.
            /// @return True on success. False on failure.
            bool read() noexcept;

            /// @brief Returns the number of FsSaveDataInfo entries read from the system.
            /// @return Number of FsSaveDataInfo entries read from the system.
            int64_t get_read_count() const noexcept;

            /// @brief Returns a reference to the FsSaveDataInfo at index. Bounds checking is performed.
            FsSaveDataInfo &at(int index);

            /// @brief Operator used to retrieve references to the FsSavDataInfo array.
            /// @param index Index of the FsSaveDataInfo struct to get.
            /// @return Reference to the FsSaveDataInfo struct at index.
            FsSaveDataInfo &operator[](int index);

            /// @brief Returns the beginning of the array.
            const FsSaveDataInfo *begin() const noexcept;

            /// @brief Returns the last valid element of the array.
            const FsSaveDataInfo *end() const noexcept;

        private:
            /// @brief Underlying FsSaveDataInfoReader.
            FsSaveDataInfoReader m_infoReader;

            /// @brief Saves whether opening the reader was successful.
            bool m_isOpen{};

            /// @brief Number of FsSaveDataInfo struct buffers to allocate for reading.
            size_t m_bufferCount{};

            /// @brief Number of entries read when read() is called.
            int64_t m_readCount{};

            /// @brief SaveDataInfo buffer array.
            std::unique_ptr<FsSaveDataInfo[]> m_saveInfoBuffer{};

            /// @brief Private function that allocates the buffer array and records the count.
            /// @param bufferCount Number of FsSaveDataInfo structs to allocate.
            void allocate_save_info_array(size_t bufferCount);
    };
}; // namespace fslib
