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

            DirectoryEntry(DirectoryEntry &&entry) noexcept;
            DirectoryEntry &operator=(DirectoryEntry &&entry) noexcept;

            DirectoryEntry(const DirectoryEntry &)            = delete;
            DirectoryEntry &operator=(const DirectoryEntry &) = delete;

            /// @brief Returns whether or not the entry is a directory.
            bool is_directory() const noexcept;

            /// @brief Returns the file's name.
            const char *get_filename() const noexcept;

            /// @brief Returns the extension of the file.
            const char *get_extension() const noexcept;

            /// @brief Returns the size of the entry.
            int64_t get_size() const noexcept;

        private:
            /// @brief Whether or not the entry is a directory.
            bool m_directory{};

            /// @brief The entry's name.
            std::string m_filename{};

            /// @brief The entry's size.
            int64_t m_size{};
    };
}
