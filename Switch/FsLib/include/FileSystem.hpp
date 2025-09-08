#pragma once
#include <switch.h>

class FileSystem final
{
    public:
        /// @brief Constructor.
        /// @param filesystem Reference to FsFileSystem to manage.
        FileSystem(FsFileSystem &filesystem) noexcept;

        FileSystem(FileSystem &&filesystem) noexcept;
        FileSystem &operator=(FileSystem &&filesystem) noexcept;

        FileSystem(const FileSystem &)            = delete;
        FileSystem &operator=(const FileSystem &) = delete;

        /// @brief Closes the managed FileSystem.
        ~FileSystem() noexcept;

        /// @brief Returns a pointer to the FsFileSystem for use with the Switch.
        FsFileSystem *get() noexcept;

    private:
        /// @brief Underlying, managed filesystem.
        FsFileSystem m_filesystem;
};