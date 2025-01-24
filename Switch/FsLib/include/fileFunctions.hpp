#pragma once
#include "Path.hpp"
#include <cstdint>

namespace fslib
{
    /// @brief Attempts to create file with FileSize.
    /// @param filePath Path to file to create.
    /// @param fileSize Optional. The size to use when creating the file.
    /// @return True on success. False on failure.
    bool createFile(const fslib::Path &filePath, int64_t fileSize = 0);

    /// @brief Checks to see if the file exists. Can also be used to check if item is a file.
    /// @param filePath Path of target file.
    /// @return True if it exists. False if it doesn't.
    bool fileExists(const fslib::Path &filePath);

    /// @brief Attempts to delete file.
    /// @param filePath Path of target file.
    /// @return True on success. False on failure.
    bool deleteFile(const fslib::Path &filePath);

    /// @brief Attempts to get file's size.
    /// @param filePath Path of target file.
    /// @return File's size on success. -1 on error.
    int64_t getFileSize(const fslib::Path &filePath);

    /// @brief Attempts to rename OldPath to NewPath.
    /// @param oldPath Original path of target file.
    /// @param newPath New path of target file.
    /// @return True on success. False on failure.
    bool renameFile(const fslib::Path &oldPath, const fslib::Path &newPath);
} // namespace fslib
