#pragma once
#include "DirectoryEntry.hpp"
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
            /// @brief This makes stuff slightly easier to type.
            using iterator = std::vector<fslib::DirectoryEntry>::const_iterator;

            /// @brief Default initializer for FsLib::Directory.
            Directory() = default;

            /// @brief Opens the directory at DirectoryPath and gets a listing. IsOpen can be checked to see if this was
            /// successful.
            /// @param directoryPath Path to directory as either FsLib::Path or UTF-16 formatted string. Ex: u"sdmc:/"
            /// @param sortEntries Optional. Whether or not entries should be sorted Dir->File, then alphabetically. This is
            /// done by default.
            Directory(const fslib::Path &directoryPath, bool sortEntries = true);

            /// @brief Move constructor.
            Directory(Directory &&directory);

            /// @brief Move operator.
            Directory &operator=(Directory &&directory);

            // We don't like these kinds round dese parts.
            Directory(const Directory &)            = delete;
            Directory &operator=(const Directory &) = delete;

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

            /// @brief Returns Entry at index as a UTF-16 AKA u16_string view.
            /// @param index Index of entry to retrieve.
            /// @return Entry at index or empty if out of bounds.
            const fslib::DirectoryEntry &get_entry(int index) const;

            /// @brief Operator to return the name of the entry at index.
            /// @param Index Index of entry to get.
            /// @return Name of entry. If out of bounds, nullptr.
            const fslib::DirectoryEntry &operator[](int index) const;

            /// @brief Begin for range based for loops.
            Directory::iterator begin();

            /// @brief End for range based for loops.
            Directory::iterator end() const;

            /// @brief Operator for range based for loops.
            const fslib::DirectoryEntry &operator*() const;

            /// @brief Operator for range based for loops.
            const fslib::DirectoryEntry *operator->() const;

            /// @brief Operator for range based for loops. Increments the internal index.
            Directory &operator++();

            /// @brief Compares the iter passed.
            bool operator!=(const fslib::Directory &iter) const;

        private:
            /// @brief Directory handle.
            Handle m_handle{};

            /// @brief Whether or not Directory::Open was successful.
            bool m_wasOpened{};

            /// @brief Vector of 3DS FS_DirectoryEntry's. 3DS has no way of retrieving a count first or this wouldn't be a
            /// vector.
            std::vector<fslib::DirectoryEntry> m_list{};

            /// @brief This keeps track of the internals for range loops.
            int m_iterIndex{};

            /// @brief Closes directory handle. The directory is read in its entirety when open is called. Public access is not
            /// needed.
            bool close();

            inline bool index_check(int index) const
            {
                const int listSize = m_list.size();
                return index >= 0 && index < listSize;
            }
    };
} // namespace fslib
