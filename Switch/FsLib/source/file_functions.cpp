#include "file_functions.hpp"
#include "error.hpp"
#include "fslib.hpp"
#include <switch.h>

bool fslib::create_file(const fslib::Path &filePath, int64_t fileSize)
{
    if (!filePath.is_valid())
    {
        return false;
    }

    FsFileSystem *filesystem;
    if (!fslib::get_file_system_by_device_name(filePath.get_device_name(), &filesystem))
    {
        return false;
    }

    if (error::occurred(fsFsCreateFile(filesystem, filePath.get_path(), fileSize, 0)))
    {
        return false;
    }

    return true;
}

bool fslib::file_exists(const fslib::Path &filePath)
{
    if (!filePath.is_valid())
    {
        return false;
    }

    FsFileSystem *filesystem;
    if (!fslib::get_file_system_by_device_name(filePath.get_device_name(), &filesystem))
    {
        return false;
    }

    FsFile handle;
    if (error::occurred(fsFsOpenFile(filesystem, filePath.get_path(), FsOpenMode_Read, &handle)))
    {
        return false;
    }

    fsFileClose(&handle);
    return true;
}

bool fslib::delete_file(const fslib::Path &filePath)
{
    if (!filePath.is_valid())
    {
        return false;
    }

    FsFileSystem *filesystem;
    if (!fslib::get_file_system_by_device_name(filePath.get_device_name(), &filesystem))
    {
        return false;
    }

    if (error::occurred(fsFsDeleteFile(filesystem, filePath.get_path())))
    {
        return false;
    }

    return true;
}

int64_t fslib::get_file_size(const fslib::Path &filePath)
{
    if (!filePath.is_valid())
    {
        return -1;
    }

    FsFileSystem *filesystem;
    if (!fslib::get_file_system_by_device_name(filePath.get_device_name(), &filesystem))
    {
        return -1;
    }

    FsFile handle;
    if (error::occurred(fsFsOpenFile(filesystem, filePath.get_path(), FsOpenMode_Read, &handle)))
    {
        return -1;
    }

    int64_t size = 0;
    if (error::occurred(fsFileGetSize(&handle, &size)))
    {
        return -1;
    }

    fsFileClose(&handle);
    return size;
}

bool fslib::rename_file(const fslib::Path &oldPath, const fslib::Path &newPath)
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

    if (error::occurred(fsFsRenameFile(filesystem, oldPath.get_path(), newPath.get_path())))
    {
        return false;
    }

    return true;
}
