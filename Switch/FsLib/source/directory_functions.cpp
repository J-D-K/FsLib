#include "directory_functions.hpp"
#include "errorCommon.h"
#include "fslib.hpp"
#include "string.hpp"
#include <switch.h>

extern std::string g_fslibErrorString;

bool fslib::create_directory(const fslib::Path &directoryPath)
{
    if (!directoryPath.is_valid())
    {
        g_fslibErrorString = ERROR_INVALID_PATH;
        return false;
    }

    FsFileSystem *fileSystem;
    if (!fslib::get_file_system_by_device_name(directoryPath.get_device_name(), &fileSystem))
    {
        g_fslibErrorString = ERROR_DEVICE_NOT_FOUND;
        return false;
    }

    Result fsError = fsFsCreateDirectory(fileSystem, directoryPath.get_path());
    if (R_FAILED(fsError))
    {
        g_fslibErrorString = string::get_formatted_string("Error creating directory: 0x%X.", fsError);
        return false;
    }

    return true;
}

bool fslib::create_directories_recursively(const fslib::Path &directoryPath)
{
    size_t slashPosition = directoryPath.find_first_of('/') + 1;
    do
    {
        // Get next slash position.
        slashPosition = directoryPath.find_first_of('/', slashPosition);
        // Get sub_path up to that slash.
        fslib::Path currentDirectory = directoryPath.sub_path(slashPosition);
        // Try to create it, but check to see if it exists first to prevent false failures.
        if (!fslib::directory_exists(currentDirectory) && !fslib::create_directory(currentDirectory))
        {
            return false;
        }
        ++slashPosition;
    } while (slashPosition < directoryPath.get_length());
    return true;
}

bool fslib::delete_directory(const fslib::Path &directoryPath)
{
    if (!directoryPath.is_valid())
    {
        g_fslibErrorString = ERROR_INVALID_PATH;
        return false;
    }

    FsFileSystem *fileSystem;
    if (!fslib::get_file_system_by_device_name(directoryPath.get_device_name(), &fileSystem))
    {
        g_fslibErrorString = ERROR_DEVICE_NOT_FOUND;
        return false;
    }

    Result fsError = fsFsDeleteDirectory(fileSystem, directoryPath.get_path());
    if (R_FAILED(fsError))
    {
        g_fslibErrorString = string::get_formatted_string("Error deleting directory: 0x%X.", fsError);
        return false;
    }
    return true;
}

bool fslib::delete_directory_recursively(const fslib::Path &directoryPath)
{
    fslib::Directory targetDirectory(directoryPath);
    if (!targetDirectory.is_open())
    {
        g_fslibErrorString =
            string::get_formatted_string("Error deleting directory recursively: %s", g_fslibErrorString.c_str());
        return false;
    }

    for (int64_t i = 0; i < targetDirectory.get_count(); i++)
    {
        fslib::Path targetPath = directoryPath / targetDirectory[i];
        if (targetDirectory.is_directory(i) && !fslib::delete_directory_recursively(targetPath))
        {
            return false;
        }
        else if (!targetDirectory.is_directory(i) && !fslib::delete_file(targetPath))
        {
            return false;
        }
    }

    // This will prevent this function from trying to delete the root (device:/) and reporting failure. Nintendo's doesn't.
    const char *pathBegin = std::char_traits<char>::find(directoryPath.c_string(), directoryPath.get_length(), '/');
    if (std::char_traits<char>::length(pathBegin) > 1 && !fslib::delete_directory(directoryPath))
    {
        return false;
    }
    return true;
}

bool fslib::directory_exists(const fslib::Path &directoryPath)
{
    if (!directoryPath.is_valid())
    {
        return false;
    }

    FsFileSystem *fileSystem;
    if (!fslib::get_file_system_by_device_name(directoryPath.get_device_name(), &fileSystem))
    {
        return false;
    }

    FsDir directoryHandle;
    Result fsError = fsFsOpenDirectory(fileSystem,
                                       directoryPath.get_path(),
                                       FsDirOpenMode_ReadDirs | FsDirOpenMode_ReadFiles,
                                       &directoryHandle);
    if (R_FAILED(fsError))
    {
        return false;
    }
    fsDirClose(&directoryHandle);
    return true;
}

bool fslib::rename_directory(const fslib::Path &oldPath, const fslib::Path &newPath)
{
    if (!oldPath.is_valid() || !newPath.is_valid() || oldPath.get_device_name() != newPath.get_device_name())
    {
        g_fslibErrorString = ERROR_INVALID_PATH;
        return false;
    }

    FsFileSystem *fileSystem;
    if (!fslib::get_file_system_by_device_name(oldPath.get_device_name(), &fileSystem))
    {
        g_fslibErrorString = ERROR_DEVICE_NOT_FOUND;
        return false;
    }

    Result fsError = fsFsRenameDirectory(fileSystem, oldPath.get_path(), newPath.get_path());
    if (R_FAILED(fsError))
    {
        g_fslibErrorString = string::get_formatted_string("Error renaming directory: 0x%X.", fsError);
        return false;
    }
    return true;
}
