#pragma once
#include <cstddef>
#include <sys/types.h>

namespace fslib
{
    /// @brief This is the base class all File and storage types are derived from.
    class Stream
    {
        public:
            /// @brief Default Stream constructor.
            Stream(void) = default;

            /// @brief Checks if stream was successfully opened.
            /// @return True on success. False on failure.
            bool isOpen(void) const;

            /// @brief Gets the current offset in the stream.
            /// @return Current offset of the stream.
            int64_t tell(void) const;

            /// @brief Gets the size of the current stream.
            /// @return Stream's size.
            int64_t getSize(void) const;

            /// @brief Returns if the end of the stream has been reached.
            /// @return True if end of stream has been reached. False if it hasn't.
            bool endOfStream(void) const;

            /**
             * @brief Seeks to Offset relative to Origin
             *
             * @param offset Offset to seek to.
             * @param origin Origin from whence to seek.
             * @note Origin can be one of the following:
             *      1. FsLib::SeekOrigin::Beginning
             *      2. FsLib::SeekOrigin::Current
             *      3. FsLib::SeekOrigin::End
             */
            void seek(int64_t offset, uint8_t origin);

            /// @brief Used to seek from the beginning of the stream.
            static constexpr uint8_t beginning = 0;
            /// @brief Used to seek from the current offset of the stream.
            static constexpr uint8_t current = 1;
            /// @brief Used to seek from the end of the stream.
            static constexpr uint8_t end = 2;
            /// @brief Used to check if and error has occurred reading or writing from a stream.
            static constexpr ssize_t error = -1;

        protected:
            /// @brief Current offset in stream.
            int64_t m_offset = 0;

            /// @brief Total size of the stream being accessed.
            int64_t m_streamSize = 0;

            /// @brief Whether or not opening the stream was successful.
            /// @note This is handled by derived classes.
            bool m_isOpen = false;

            /// @brief Ensures offset isn't out of bounds after a seek is performed.
            void ensureOffsetIsValid(void);
    };
} // namespace fslib
