#pragma once
#include "Path.hpp"

#include <3ds.h>
#include <string>
#include <vector>

namespace fslib
{
    /// @brief Opens and reads directories.
    class Directory
    {
        public:
            /// @brief Default initializer for FsLib::Directory.
            Directory() = default;

            /// @brief Opens the directory at DirectoryPath and gets a listing. IsOpen can be checked to see if this was
            /// successful.
            /// @param directoryPath Path to directory as either FsLib::Path or UTF-16 formatted string. Ex: u"sdmc:/"
            /// @param sortEntries Optional. Whether or not entries should be sorted Dir->File, then alphabetically. This is
            /// done by default.
            Directory(const fslib::Path &directoryPath, bool sortEntries = true);

            /// @brief Opens the directory at DirectoryPath and gets a listing. IsOpen can be checked to see if this was
            /// successful.
            /// @param directoryPath Path to directory as either FsLib::Path or UTF-16 formatted string. Ex: u"sdmc:/"
            /// @param sortEntries Optional. Whether or not entries should be sorted Dir->File, then alphabetically.
            void open(const fslib::Path &directoryPath, bool sortEntries = true);

            /// @brief Returns whether or not opening the directory and reading its contents was successful.
            /// @return True on success. False on failure.
            bool is_open() const;

            /// @brief Returns the number of entries successfully read from the directory.
            /// @return Number of entries read from directory.
            size_t get_count() const;

            /// @brief Returns whether or not the entry at Index in directory listing is a directory or not.
            /// @param index Index of entry to check.
            /// @return True if the entry is a directory. False if not or Index is out of bounds.
            bool is_directory(int index) const;

            /// @brief Returns Entry at index as a UTF-16 AKA u16_string view.
            /// @param index Index of entry to retrieve.
            /// @return Entry at index or empty if out of bounds.
            const char16_t *get_entry(int index) const;

            /// @brief Operator to return the name of the entry at index.
            /// @param Index Index of entry to get.
            /// @return Name of entry. If out of bounds, nullptr.
            const char16_t *operator[](int index) const;

        private:
            /// @brief Directory handle.
            Handle m_handle{};

            /// @brief Whether or not Directory::Open was successful.
            bool m_wasOpened = false;

            /// @brief Vector of 3DS FS_DirectoryEntry's. 3DS has no way of retrieving a count first or this wouldn't be a
            /// vector.
            std::vector<FS_DirectoryEntry> m_list;

            /// @brief Closes directory handle. The directory is read in its entirety when open is called. Public access is not
            /// needed.
            bool close();

            inline bool index_check(int index) const
            {
                const int listSize = m_list.size();
                return index < 0 || index >= listSize;
            }
    };
} // namespace fslib
