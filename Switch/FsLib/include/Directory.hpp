#pragma once
#include "Path.hpp"
#include <memory>
#include <switch.h>

namespace fslib
{
    /// @brief Class for opening and reading entries from directories.
    class Directory
    {
        public:
            /// @brief Default constructor for Directory.
            Directory(void) = default;

            /// @brief Attempts to open Directory path and read all entries. IsOpen can be used to check if this was successful.
            /// @param directoryPath Path to directory.
            /// @param sortListing Optional. Whether or not the listing is sorted Directories->Files and then alphabetically. This is done by default.
            Directory(const fslib::Path &directoryPath, bool sortListing = true);

            /// @brief Attempts to open Directory path and read all entries. IsOpen can be used to check if this was successful.
            /// @param directoryPath Path to directory.
            /// @param sortListing Optional. Whether or not to sort the listing. This is done by default.
            void open(const fslib::Path &directoryPath, bool sortListing = true);

            /// @brief Returns if directory was successfully opened.
            /// @return True if it was. False if it wasn't.
            bool is_open(void) const;

            /// @brief Returns total number of entries read from directory.
            /// @return Total numbers of entries read from directory.
            int64_t get_count(void) const;

            /// @brief Returns the size of the entry at Index.
            /// @param Index Index of entry.
            /// @return Size of entry. 0 if Index is out of bounds.
            int64_t get_entry_size(int index) const;

            /// @brief Returns the name of the entry at Index.
            /// @param Index Index of entry.
            /// @return Name of the entry. nullptr if Index is out of bounds.
            const char *get_entry(int index) const;

            /// @brief Returns whether or not the entry at Index is a directory.
            /// @param Index Index of entry.
            /// @return True if the item is a directory. False if it is not or out of bounds.
            bool is_directory(int index) const;

            /// @brief This can be used like isOpen.
            operator bool(void) const;

            /// @brief Returns entry name at Index.
            /// @param Index Index of entry.
            /// @return Entry's name. If out of bounds, nullptr.
            const char *operator[](int index) const;

        private:
            /// @brief Saves whether or not the directory was successfully opened and read.
            bool m_wasRead = false;

            /// @brief Handle to the directory.
            FsDir m_directoryHandle;

            /// @brief Total number of entries read from the directory.
            int64_t m_entryCount = 0;

            /// @brief Entry array.
            std::unique_ptr<FsDirectoryEntry[]> m_directoryList;

            /// @brief Closes directory handle. Directory is never kept open. Not needed outside of class.
            void close(void);
    };
} // namespace fslib
