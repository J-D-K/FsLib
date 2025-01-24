#pragma once
#include "Path.hpp"

namespace fslib
{
    /// @brief Attempts to open directory at Directory path to see if it exists and is a directory.
    /// @param directoryPath Path to test.
    /// @return True if the directory exists and can be opened. False if it does not.
    bool directoryExists(const fslib::Path &directoryPath);

    /// @brief Attempts to create directory with DirectoryPath.
    /// @param directoryPath Path to create.
    /// @return True on success. False on failure.
    bool createDirectory(const fslib::Path &directoryPath);

    /// @brief Attempts to create every directory in Directory path. Does not need a trailing slash. Warning: This is a bit slow.
    /// @return True on success. False on failure.
    bool createDirectoriesRecursively(const fslib::Path &directoryPath);

    /// @brief Attempts to rename directory from OldPath to NewPath. Both must be on the same device.
    /// @param oldPath Original name of target directory.
    /// @param newPath New name of target directory.
    /// @return True on success. False on failure.
    bool renameDirectory(const fslib::Path &oldPath, const fslib::Path &newPath);

    /// @brief Attempts to delete the directory passed in DirectoryPath.
    /// @param directoryPath Target directory path.
    /// @return True on success. False on failure.
    bool deleteDirectory(const fslib::Path &directoryPath);

    /// @brief Attempts to recursively delete DirectoryPath
    /// @param directoryPath Path to the directory to delete.
    /// @return True on success. False on failure.
    bool deleteDirectoryRecursively(const fslib::Path &directoryPath);
} // namespace fslib
