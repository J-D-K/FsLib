#pragma once
#include "Directory.hpp"
#include "Path.hpp"

#include <span>
#include <switch.h>

namespace fslib
{
    class DirectoryIterator
    {
        public:
            /// @brief Directory iterator constructor.
            /// @param path Path of the directory to open.
            DirectoryIterator(const fslib::Path &path);

            /// @brief Returns the beginning of the array
            FsDirectoryEntry *begin() const;

            /// @brief Returns the end of the array.
            FsDirectoryEntry *end() const;

            /// @brief Required for this to work.
            FsDirectoryEntry &operator*();

            /// @brief Required operator for this to work.
            FsDirectoryEntry *operator->();

            /// @brief Increments the current index.
            DirectoryIterator &operator++();

            bool operator!=(const DirectoryIterator &iter);

        private:
            /// @brief Current index of the iterator.
            int m_index{};

            /// @brief This is the main directory instance.
            fslib::Directory m_directory{};
    };
}
