#pragma once
#include "Path.hpp"
#include "Stream.hpp"
#include <switch.h>

/// @brief This is an added OpenMode flag for FsLib on Switch so File::Open knows for sure it's supposed to create the file.
static constexpr uint32_t FsOpenMode_Create = BIT(8);

namespace fslib
{
    /// @brief Class for opening, reading, and writing to files.
    class File : public fslib::Stream
    {
        public:
            /// @brief Default file constructor.
            File(void) = default;

            /**
             * @brief Attempts to open file at FilePath with OpenFlags. IsOpen can be used to check if this was successful.
             *
             * @param filePath Path to file.
             * @param openFlags Flags from LibNX to use to open the file with.
             * @param fileSize Optional. Creates the file with a starting size defined.
             */
            File(const fslib::Path &filePath, uint32_t openFlags, int64_t fileSize = 0);

            /// @brief Closes file handle if it's still open.
            ~File();

            /// @brief Attempts to open file at FilePath with OpenFlags.
            /// @param filePath Path to file.
            /// @param openFlags Flags from LibNX to use to open the file with.
            /// @param fileSize Optional. Creates the file with a starting size defined.
            void open(const fslib::Path &filePath, uint32_t openFlags, int64_t fileSize = 0);

            /// @brief Closes file handle if needed. Destructor takes care of this for you normally.
            void close(void);

            /// @brief Returns if file was successfully opened.
            /// @return
            bool is_open(void) const;

            /// @brief Attempts to read ReadSize bytes into Buffer from file.
            /// @param buffer Buffer to write into.
            /// @param readSize Buffer's capacity.
            /// @return Number of bytes read.
            ssize_t read(void *buffer, size_t bufferSize);

            /// @brief Attempts to read a line from file until `\n` or `\r` is reached.
            /// @param lineOut Buffer to read line into.
            /// @param lineLength Size of line buffer.
            /// @return True on success. False on failure or line exceeding LineLength.
            bool read_line(char *lineOut, size_t lineLength);

            /// @brief Attempts to read a single character or byte from file.
            /// @return Byte read.
            signed char get_byte(void);

            /// @brief Attempts to write Buffer of BufferSize bytes to file.
            /// @param buffer Buffer containing data.
            /// @param bufferSize Size of Buffer.
            /// @return Number of bytes (assumed to be) written to file. -1 on error.
            ssize_t write(const void *buffer, size_t bufferSize);

            /// @brief Attempts to write a formatted string to file.
            /// @param format Format of string.
            /// @param arguments Arguments.
            /// @return True on success. False on failure.
            bool writef(const char *format, ...);

            /// @brief Writes a single byte to file.
            /// @param byte Byte to write.
            /// @return True on success. False on failure.
            bool put_byte(char byte);

            /// @brief Operator for quick string writing.
            /// @param string String to write.
            /// @return Reference to file.
            File &operator<<(const char *string);

            /// @brief Operator for quick string writing.
            /// @param string String to write.
            /// @return Reference to file.
            File &operator<<(const std::string &string);

            /// @brief Flushes file.
            /// @return True on success. False on failure.
            bool flush(void);

        private:
            /// @brief File handle.
            FsFile m_fileHandle;

            /// @brief Stores flags used to open file.
            uint32_t m_openFlags = 0;

            /// @brief Private: Resizes file if Buffer is too large to fit in remaining space.
            /// @param bufferSize Size of buffer.
            /// @return True on success. False on failure.
            bool resize_if_needed(size_t bufferSize);

            /// @brief Private: Returns if file has flag set to read.
            /// @return True if flags are correct. False if not.
            inline bool is_open_for_reading(void) const
            {
                return (m_openFlags & FsOpenMode_Read);
            }

            /// @brief Private: Returns if file has flag set to write.
            /// @return True if flags are correct. False if not.
            inline bool is_open_for_writing(void) const
            {
                return (m_openFlags & FsOpenMode_Write) || (m_openFlags & FsOpenMode_Append);
            }
    };
} // namespace fslib
