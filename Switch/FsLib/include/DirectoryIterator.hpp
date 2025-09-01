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
            DirectoryIterator(fslib::Directory *directory) noexcept;

            /// @brief Returns an iterator to the Directory's begin.
            fslib::DirectoryIterator::iterator begin() const noexcept;

            /// @brief Returns an iterator to the Directory;s end;
            fslib::DirectoryIterator::iterator end() const noexcept;

        private:
            /// @brief Pointer to the directory that constructed this.
            fslib::Directory *m_directory{};
    };
}
