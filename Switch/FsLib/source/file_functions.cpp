#include "file_functions.hpp"
#include "errorCommon.h"
#include "fslib.hpp"
#include "string.hpp"
#include <switch.h>

extern std::string g_fslibErrorString;

bool fslib::create_file(const fslib::Path &filePath, int64_t fileSize)
{
    if (!filePath.is_valid())
    {
        g_fslibErrorString = ERROR_INVALID_PATH;
        return false;
    }

    FsFileSystem *fileSystem;
    if (!fslib::get_file_system_by_device_name(filePath.get_device_name(), &fileSystem))
    {
        g_fslibErrorString = ERROR_DEVICE_NOT_FOUND;
        return false;
    }

    Result fsError = fsFsCreateFile(fileSystem, filePath.get_path(), fileSize, 0);
    if (R_FAILED(fsError))
    {
        g_fslibErrorString = string::get_formatted_string("Error creating file: 0x%X.", fsError);
        return false;
    }

    return true;
}

bool fslib::file_exists(const fslib::Path &filePath)
{
    if (!filePath.is_valid())
    {
        g_fslibErrorString = ERROR_INVALID_PATH;
        return false;
    }

    FsFileSystem *fileSystem;
    if (!fslib::get_file_system_by_device_name(filePath.get_device_name(), &fileSystem))
    {
        g_fslibErrorString = ERROR_DEVICE_NOT_FOUND;
        return false;
    }

    FsFile fileHandle;
    Result fsError = fsFsOpenFile(fileSystem, filePath.get_path(), FsOpenMode_Read, &fileHandle);
    if (R_FAILED(fsError))
    {
        return false;
    }
    fsFileClose(&fileHandle);
    return true;
}

bool fslib::delete_file(const fslib::Path &filePath)
{
    if (!filePath.is_valid())
    {
        g_fslibErrorString = ERROR_INVALID_PATH;
        return false;
    }

    FsFileSystem *fileSystem;
    if (!fslib::get_file_system_by_device_name(filePath.get_device_name(), &fileSystem))
    {
        g_fslibErrorString = ERROR_DEVICE_NOT_FOUND;
        return false;
    }

    Result fsError = fsFsDeleteFile(fileSystem, filePath.get_path());
    if (R_FAILED(fsError))
    {
        g_fslibErrorString = string::get_formatted_string("Error deleting file: 0x%X.", fsError);
        return false;
    }
    return true;
}

int64_t fslib::get_file_size(const fslib::Path &filePath)
{
    if (!filePath.is_valid())
    {
        g_fslibErrorString = ERROR_INVALID_PATH;
        return -1;
    }

    FsFileSystem *fileSystem;
    if (!fslib::get_file_system_by_device_name(filePath.get_device_name(), &fileSystem))
    {
        g_fslibErrorString = ERROR_DEVICE_NOT_FOUND;
        return -1;
    }

    FsFile fileHandle;
    Result fsError = fsFsOpenFile(fileSystem, filePath.get_path(), FsOpenMode_Read, &fileHandle);
    if (R_FAILED(fsError))
    {
        g_fslibErrorString = string::get_formatted_string("Error opening file to get size: 0x%X.", fsError);
        return -1;
    }

    int64_t fileSize = 0;
    fsError = fsFileGetSize(&fileHandle, &fileSize);
    if (R_FAILED(fsError))
    {
        fsFileClose(&fileHandle);
        g_fslibErrorString = string::get_formatted_string("Error getting file size: 0x%X.", fsError);
        return -1;
    }
    fsFileClose(&fileHandle);
    return fileSize;
}

bool fslib::rename_file(const fslib::Path &oldPath, const fslib::Path &newPath)
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

    Result fsError = fsFsRenameFile(fileSystem, oldPath.get_path(), newPath.get_path());
    if (R_FAILED(fsError))
    {
        g_fslibErrorString = string::get_formatted_string("Error renaming file: 0x%X.", fsError);
        return false;
    }
    return true;
}
