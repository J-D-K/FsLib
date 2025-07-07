#include "directory_functions.hpp"
#include "error.hpp"
#include "fslib.hpp"
#include <switch.h>

bool fslib::create_directory(const fslib::Path &directoryPath)
{
    if (!directoryPath.is_valid())
    {
        return false;
    }

    FsFileSystem *filesystem;
    if (!fslib::get_file_system_by_device_name(directoryPath.get_device_name(), &filesystem))
    {
        return false;
    }

    if (error::occurred(fsFsCreateDirectory(filesystem, directoryPath.get_path())))
    {
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
        return false;
    }

    FsFileSystem *filesystem;
    if (!fslib::get_file_system_by_device_name(directoryPath.get_device_name(), &filesystem))
    {
        return false;
    }

    if (error::occurred(fsFsDeleteDirectory(filesystem, directoryPath.get_path())))
    {
        return false;
    }

    return true;
}

bool fslib::delete_directory_recursively(const fslib::Path &directoryPath)
{
    fslib::Directory targetDirectory{directoryPath};
    if (!targetDirectory.is_open())
    {
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
    const char *pathBegin = std::char_traits<char>::find(directoryPath.full_path(), directoryPath.get_length(), '/');
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

    FsFileSystem *filesystem;
    if (!fslib::get_file_system_by_device_name(directoryPath.get_device_name(), &filesystem))
    {
        return false;
    }

    FsDir directoryHandle;
    Result fsError = fsFsOpenDirectory(filesystem,
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
        return false;
    }

    FsFileSystem *filesystem;
    if (!fslib::get_file_system_by_device_name(oldPath.get_device_name(), &filesystem))
    {
        return false;
    }

    if (error::occurred(fsFsRenameDirectory(filesystem, oldPath.get_path(), newPath.get_path())))
    {
        return false;
    }

    return true;
}
