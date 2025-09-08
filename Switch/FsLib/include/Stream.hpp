#pragma once
#include <cstddef>
#include <cstdint>
#include <sys/types.h>

namespace fslib
{
    /// @brief This is the base class all File and storage types are derived from.
    class Stream
    {
        public:
            /// @brief Seeking origins.
            enum class Origin
            {
                BEGINNING,
                CURRENT,
                END
            };

            /// @brief Default Stream constructor.
            Stream() = default;

            Stream(Stream &&stream) noexcept;
            Stream &operator=(Stream &&string) noexcept;

            Stream(const Stream &)            = delete;
            Stream &operator=(const Stream &) = delete;

            /// @brief Checks if stream was successfully opened.
            /// @return True on success. False on failure.
            bool is_open() const;

            /// @brief Gets the current offset in the stream.
            /// @return Current offset of the stream.
            int64_t tell() const;

            /// @brief Gets the size of the current stream.
            /// @return Stream's size.
            int64_t get_size() const;

            /// @brief Returns if the end of the stream has been reached.
            /// @return True if end of stream has been reached. False if it hasn't.
            bool end_of_stream() const;

            /**
             * @brief Seeks to Offset relative to Origin
             *
             * @param offset Offset to seek to.
             * @param origin Origin from whence to seek.
             * @note Origin can be one of the following:
             *      1. fslib::Stream::Origin::BEGINNING
             *      2. fslib::Stream::Origin::CURRENT
             *      3. fslib::Stream::Origin::END
             */
            virtual void seek(int64_t offset, Stream::Origin origin);

            /// @brief Operator that can be used like isOpen().
            operator bool() const;

            /// @brief These serve as shortcuts.
            static constexpr Stream::Origin BEGINNING = Origin::BEGINNING;
            static constexpr Stream::Origin CURRENT   = Origin::CURRENT;
            static constexpr Stream::Origin END       = Origin::END;

            /// @brief Used to check if and error has occurred reading or writing from a stream.
            static constexpr ssize_t ERROR = -1;

        protected:
            /// @brief Current offset in stream.
            int64_t m_offset{};

            /// @brief Total size of the stream being accessed.
            int64_t m_streamSize{};

            /// @brief Whether or not opening the stream was successful.
            /// @note This is handled by derived classes.
            bool m_isOpen{};

            /// @brief Ensures offset isn't out of bounds after a seek is performed.
            void ensure_offset_is_valid();
    };
} // namespace fslib
