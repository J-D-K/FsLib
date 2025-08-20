#pragma once
#include <string>
#include <switch.h>

namespace fslib
{
    class DirectoryEntry
    {
        public:
            /// @brief Constructor.
            /// @param entry Entry to copy.
            DirectoryEntry(const FsDirectoryEntry &entry);

            /// @brief Returns whether or not the entry is a directory.
            bool is_directory() const;

            /// @brief Returns the file's name.
            const char *get_filename() const;

            /// @brief Returns the extension of the file.
            const char *get_extension() const;

            /// @brief Returns the size of the entry.
            int64_t get_size() const;

        private:
            /// @brief Whether or not the entry is a directory.
            bool m_isDirectory{};

            /// @brief The name of entry.
            std::string m_filename{};

            /// @brief The size of the entry.
            int64_t m_size{};
    };
}
