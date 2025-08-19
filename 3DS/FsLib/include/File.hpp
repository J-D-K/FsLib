#pragma once
#include "Path.hpp"

#include <3ds.h>
#include <cstdint>

// This is to make this easier.
static constexpr uint32_t FS_OPEN_APPEND = BIT(3);

namespace fslib
{
    /// @brief Class for reading and writing to files.
    class File
    {
        public:
            /// @brief Seeking origin.
            enum class Origin
            {
                BEGINNING,
                CURRENT,
                END
            };

            /// @brief Default FsLib::File constructor.
            File() = default;

            /**
             * @brief Attempts to open FilePath with OpenFlags. FileSize is optional unless trying to create new Extra Data type
             * files.
             * @param filePath Path to target file.
             * @param openFlags Flags to use to open the file. Can be any flag from Ctrulib. FsLib provides a new FS_OPEN_APPEND
             * if needed.
             * @param fileSize Optional. Size used to create the file. This is only needed when creating and writing to Extra
             * Data type archives.
             */
            File(const fslib::Path &filePath, uint32_t openFlags, uint64_t fileSize = 0);

            /// @brief Move constructor.
            /// @param file File to move.
            File(File &&file);

            /// @brief Move operator.
            /// @param file File to move
            File &operator=(File &&file);

            // None of this nonsense round these parts.
            File(const File &)            = delete;
            File &operator=(const File &) = delete;

            /// @brief Automatically closes handle when freed or goes out of scope.
            ~File();

            /**
             * @brief Attempts to open FilePath with OpenFlags. FileSize is optional unless trying to create new Extra Data type
             * files.
             * @param filePath Path to target file.
             * @param openFlags Flags to use to open the file. Can be any flag from Ctrulib. FsLib provides a new FS_OPEN_APPEND
             * if needed.
             * @param fileSize Optional. Size used to create the file. This is only needed when creating and writing to Extra
             * Data type archives.
             */
            void open(const fslib::Path &filePath, uint32_t openFlags, uint64_t fileSize = 0);

            /// @brief Can be used to manually close the file handle if needed.
            void close();

            /// @brief Returns whether opening the file was successful or not.
            /// @return True on success. False on failure.
            bool is_open() const;

            /// @brief Returns the current offset of the file.
            /// @return Current file offset.
            uint64_t tell() const;

            /// @brief Returns the size of the file.
            /// @return File's current size.
            uint64_t get_size() const;

            /// @brief Returns whether or not the end of the file has been reached.
            /// @return True if the end is reached. False if not.
            bool end_of_file() const;

            /// @brief Seeks to a position in file. Offsets are bounds checked.
            /// @param offset Offset to seek to.
            /// @param origin Position to seek from.
            void seek(int64_t Offset, File::Origin origin);

            /// @brief Attempts to read from file. Certain read errors are corrected for.
            /// @param buffer Buffer to read into.
            /// @param readSize Size of the buffer to read into.
            /// @return Number of bytes read on success. -1 on complete failure. FsLib::GetError string can be used to get
            /// slightly more information.
            ssize_t read(void *buffer, size_t readSize);

            /// @brief Attempts to read a line until `\n` or `\r`, or bufferSize is hit.
            /// @param buffer Buffer to read into.
            /// @param bufferSize Size of buffer.
            /// @return True on success. False on failure or bufferSize is too small.
            bool read_line(char *buffer, size_t bufferSize);

            /// @brief Attempts to read a line, or until `\n` or `\r` is hit.
            /// @param line C++ string to read line to.
            /// @return True on success, false on end of file or read error.
            bool read_line(std::string &line);

            /// @brief Reads one char or byte from file.
            /// @return Byte read on success. -1 on failure.
            signed char get_byte();

            /// @brief Attempts to write Buffer to File. File is automatically resized to fit Buffer if needed.
            /// @param buffer Buffer to write to file.
            /// @param bufferSize Size of Buffer
            /// @return Number of bytes written on success. -1 on complete failure.
            ssize_t write(const void *buffer, size_t bufferSize);

            /// @brief Attempts to write a formatted string to file.
            /// @param format Format of string.
            /// @param arguments
            /// @return True on success. False on failure.
            bool writef(const char *format, ...);

            /// @brief std style operators for quick, easy string writing.
            /// @param string String to write to file.
            /// @return Reference to current file.
            File &operator<<(const char *string);
            File &operator<<(const std::string &string);

            /// @brief Attempts to write a single char or byte to file.
            /// @param byte Byte to write.
            /// @return True on success. False on failure.
            bool put_byte(char byte);

            /// @brief Flushes the file.
            /// @return True on success. False on failure.
            bool flush();

            static constexpr File::Origin BEGINNING = File::Origin::BEGINNING;
            static constexpr File::Origin CURRENT   = File::Origin::CURRENT;
            static constexpr File::Origin END       = File::Origin::END;

        protected:
            /// @brief Handle to file.
            Handle m_handle{};

            /// @brief Stores whether open was successful or not.
            bool m_isOpen{};

            /// @brief Stores flags passed to open.
            uint32_t m_openFlags{};

            /// @brief Store the current offset in the file and the size of the file.
            int64_t m_offset{}, m_size{};

            /// @brief Attempts to resize a file if the buffer size is too large to fit in the remaining space.
            /// @param BufferSize Size of buffer to check.
            /// @return True on success. False on failure.
            bool resize_if_needed(size_t bufferSize);

            /// @brief Returns whether or not the file is open for reading by checking m_Flags.
            /// @return True if it is. False if it isn't.
            inline bool is_open_for_reading() const { return m_openFlags & FS_OPEN_READ; }

            /// @brief Returns whether or not the file is open for writing by checking m_Flags.
            /// @return True if it is. False if it isn't.
            inline bool is_open_for_writing() const { return m_openFlags & FS_OPEN_WRITE; }
    };
} // namespace fslib
