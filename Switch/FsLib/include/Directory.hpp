#pragma once
#include "DirectoryEntry.hpp"
#include "Path.hpp"

#include <switch.h>
#include <vector>

namespace fslib
{
    /// @brief Forward declaration to avoid clashes.
    class DirectoryIterator;

    /// @brief Class for opening and reading entries from directories.
    class Directory final
    {
        public:
            /// @brief Default constructor for Directory.
            Directory() = default;

            /// @brief Attempts to open Directory path and read all entries. IsOpen can be used to check if this was successful.
            /// @param directoryPath Path to directory.
            /// @param sortListing Optional. Whether or not the listing is sorted Directories->Files and then alphabetically.
            /// This is done by default.
            Directory(const fslib::Path &directoryPath, bool sortListing = true);

            /// @brief Move constructor for directory.
            /// @param directory Directory to move.
            Directory(Directory &&directory);

            /// @brief Move operator for directory.
            /// @param directory Directory to move.
            Directory &operator=(Directory &&directory);

            // None of this nonsense round dese parts!
            Directory(const Directory &)            = delete;
            Directory &operator=(const Directory &) = delete;

            /// @brief Attempts to open Directory path and read all entries. IsOpen can be used to check if this was successful.
            /// @param directoryPath Path to directory.
            /// @param sortListing Optional. Whether or not to sort the listing. This is done by default.
            void open(const fslib::Path &directoryPath, bool sortListing = true);

            /// @brief Returns if directory was successfully opened.
            /// @return True if it was. False if it wasn't.
            bool is_open() const noexcept;

            /// @brief Returns total number of entries read from directory.
            /// @return Total numbers of entries read from directory.
            int64_t get_count() const noexcept;

            /// @brief Returns the name of the entry at Index.
            /// @param Index Index of entry.
            /// @return Name of the entry. nullptr if Index is out of bounds.
            const fslib::DirectoryEntry &get_entry(int index) const;

            /// @brief Returns entry name at Index.
            /// @param Index Index of entry.
            /// @return Entry's name. If out of bounds, nullptr.
            const fslib::DirectoryEntry &operator[](int index) const;

            /// @brief Returns a Directory iterator for use with range based for loops.
            fslib::DirectoryIterator list();

            /// @brief This is so the DirectoryIterator class can access the private members here.
            friend class fslib::DirectoryIterator;

        private:
            /// @brief Saves whether or not the directory was successfully opened and read.
            bool m_wasRead{};

            /// @brief Handle to the directory.
            FsDir m_handle{};

            /// @brief Total number of entries read from the directory.
            int64_t m_entryCount{};

            /// @brief Entry vector.
            std::vector<fslib::DirectoryEntry> m_directoryList{};

            /// @brief Closes directory handle. Directory is never kept open. Not needed outside of class.
            void close() noexcept;
    };
} // namespace fslib
