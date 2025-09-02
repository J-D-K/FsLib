#pragma once
#include <switch.h>

class FileSystem final
{
    public:
        /// @brief Constructor.
        /// @param filesystem Reference to FsFileSystem to manage.
        FileSystem(FsFileSystem &filesystem) noexcept;

        /// @brief Closes the managed FileSystem.
        ~FileSystem() noexcept;

        /// @brief Returns a pointer to the FsFileSystem for use with the Switch.
        FsFileSystem *get() noexcept;

    private:
        /// @brief Underlying, managed filesystem.
        FsFileSystem m_filesystem;
};