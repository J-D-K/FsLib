#pragma once
#include "Directory.hpp"

namespace fslib
{
    class DirectoryIterator
    {
        public:
            /// @brief Constructor. Opens creates and new DirectoryIterator.
            /// @param path Path of the directory to open.
            DirectoryIterator(const fslib::Path &path);

            /// @brief Begin. Needed for range based for loops.
            const FS_DirectoryEntry *begin() const;

            /// @brief End. Needed for range based for loops.
            const FS_DirectoryEntry *end() const;

            /// @brief Dereference operator.
            FS_DirectoryEntry &operator*();

            /// @brief Pointer operator.
            FS_DirectoryEntry *operator->();

            /// @brief Increment operator.
            DirectoryIterator &operator++();

            /// @brief Comparison operator.
            /// @param iter Iterator to compare.
            bool operator!=(const DirectoryIterator &iter) const;

        private:
            /// @brief Current index of the iterator.
            int m_index{};

            /// @brief Underlying directory.
            fslib::Directory m_directory{};
    };
}
