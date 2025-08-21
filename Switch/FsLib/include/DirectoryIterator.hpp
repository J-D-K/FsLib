#pragma once
#include "Directory.hpp"

namespace fslib
{
    /// @brief Note: This is not for the end user to use. Open a directory using the Directory class and call .list() instead.
    class DirectoryIterator
    {
        public:
            /// @brief This makes some things easier to type later.
            using iterator = std::vector<fslib::DirectoryEntry>::const_iterator;

            /// @brief Constructor. Takes a pointer to the Directory that instantiated it.
            DirectoryIterator(fslib::Directory *directory);

            /// @brief Returns an iterator to the Directory's begin.
            fslib::DirectoryIterator::iterator begin() const;

            /// @brief Returns an iterator to the Directory;s end;
            fslib::DirectoryIterator::iterator end() const;

            /// @brief Operator needed for range based loops.
            fslib::DirectoryEntry &operator&() const;

            /// @brief Operator needed for range based loops.
            fslib::DirectoryEntry *operator*() const;

            /// @brief Increments the internal index.
            fslib::DirectoryIterator &operator++();

            /// @brief Returns if the iterator passed is not equal to this one.
            bool operator!=(const fslib::DirectoryIterator &iter) const;

        private:
            /// @brief Pointer to the directory that constructed this.
            fslib::Directory *m_directory{};

            /// @brief Internal index of the iterator.
            int m_index{};
    };
}
