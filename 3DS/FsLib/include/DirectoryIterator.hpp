#pragma once
#include "Directory.hpp"

namespace fslib
{
    /// @brief Note: This is not for the end user to use. Open a directory using the Directory class and call .list() instead.
    class DirectoryIterator
    {
        public:
            /// @brief This makes stuff easier to type.
            using iterator = std::vector<fslib::DirectoryEntry>::const_iterator;

            /// @brief Constructor.
            /// @param directory Pointer to the directory that instantiated the instance.
            DirectoryIterator(fslib::Directory *directory);

            /// @brief Returns the begin of the directory passed.
            fslib::DirectoryIterator::iterator begin() const;

            /// @brief Returns the end of the directory passed.
            fslib::DirectoryIterator::iterator end() const;

            /// @brief Operator needed for range based for loops.
            fslib::DirectoryEntry &operator&() const;

            /// @brief Operator needed for range based for loops.
            fslib::DirectoryEntry *operator*() const;

            /// @brief Operator needed for range based for loops.
            fslib::DirectoryIterator &operator++();

            /// @brief Operator needed for range based for loops.
            bool operator!=(const fslib::DirectoryIterator &iter);

        private:
            /// @brief Pointer to the directory that instantiated this.
            fslib::Directory *m_directory{};

            /// @brief Internal index of the iterator.
            int m_index{};
    };
}
