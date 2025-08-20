#pragma once
#include <3ds.h>
#include <string>

namespace fslib
{
    class DirectoryEntry
    {
        public:
            /// @brief Constructor. Takes the useful information from entry.
            DirectoryEntry(const FS_DirectoryEntry &entry);

            /// @brief Returns whether or not the entry is a directory.
            bool is_directory() const;

            /// @brief Returns the filename as a UTF-16 C string.
            const char16_t *get_filename() const;

            /// @brief Returns the extension or nullptr if on isn't found.
            const char16_t *get_extension() const;

        private:
            /// @brief Stores whether or not the entry is a directory.
            bool m_isDirectory{};

            /// @brief This stores the file's name.
            std::u16string m_filename{};
    };
}
