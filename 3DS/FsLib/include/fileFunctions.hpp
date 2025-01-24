#pragma once
#include "Path.hpp"

namespace fslib
{
    /// @brief This is a shortcut function to create an empty file.
    /// @param filePath Path to file to create.
    /// @param fileSize Optional. Starting size of file.
    /// @return True on success. False on failure.
    bool createFile(const fslib::Path &filePath, uint64_t fileSize = 0);

    /// @brief Attempts to open FilePath for reading to check if it exists.
    /// @param filePath Path to file to check.
    /// @return True if file exists. False if it doesn't.
    bool fileExists(const fslib::Path &filePath);

    /// @brief Attempts to get the size of filePath.
    /// @param filePath Path to file.
    /// @param fileSizeOut uint64_t to write file size to.
    /// @return True on success. False on failure.
    bool getFileSize(const fslib::Path &filePath, uint64_t &fileSizeOut);

    /// @brief Attempts to rename file from OldPath to NewPath. Both must exist on the same device.
    /// @param oldPath Original path to target file.
    /// @param newPath New path of target file.
    /// @return True on success. False on failure.
    bool renameFile(const fslib::Path &oldPath, const fslib::Path &newPath);

    /// @brief Attempts to delete file.
    /// @param filePath Path of target file.
    /// @return True on success. False on failure.
    bool deleteFile(const fslib::Path &filePath);
} // namespace fslib
