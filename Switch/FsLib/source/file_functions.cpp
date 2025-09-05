#include "file_functions.hpp"

#include "error.hpp"
#include "fslib.hpp"

#include <switch.h>

bool fslib::create_file(const fslib::Path &filePath, int64_t fileSize)
{
    FsFileSystem *filesystem{};
    const bool isValid = filePath.is_valid();
    const bool found   = isValid && fslib::get_file_system_by_device_name(filePath.get_device_name(), &filesystem);
    if (!isValid || !found) { return false; }

    const bool createError = error::occurred(fsFsCreateFile(filesystem, filePath.get_path(), fileSize, 0));
    if (createError) { return false; }
    return true;
}

bool fslib::file_exists(const fslib::Path &filePath)
{
    FsFileSystem *filesystem{};
    const bool isValid = filePath.is_valid();
    const bool found   = isValid && fslib::get_file_system_by_device_name(filePath.get_device_name(), &filesystem);
    if (!isValid || !found) { return false; }

    FsFile handle{};
    const bool openError = error::occurred(fsFsOpenFile(filesystem, filePath.get_path(), FsOpenMode_Read, &handle));
    if (openError) { return false; }

    fsFileClose(&handle);
    return true;
}

bool fslib::delete_file(const fslib::Path &filePath)
{
    FsFileSystem *filesystem{};
    const bool isValid = filePath.is_valid();
    const bool found   = isValid && fslib::get_file_system_by_device_name(filePath.get_device_name(), &filesystem);
    if (!isValid || !found) { return false; }

    const bool deleteError = error::occurred(fsFsDeleteFile(filesystem, filePath.get_path()));
    if (deleteError) { return false; }
    return true;
}

int64_t fslib::get_file_size(const fslib::Path &filePath)
{
    FsFileSystem *filesystem{};
    const bool isValid = filePath.is_valid();
    const bool found   = isValid && fslib::get_file_system_by_device_name(filePath.get_device_name(), &filesystem);
    if (!isValid || !found) { return -1; }

    int64_t size{};
    FsFile handle{};
    const bool openError = error::occurred(fsFsOpenFile(filesystem, filePath.get_path(), FsOpenMode_Read, &handle));
    const bool sizeError = !openError && error::occurred(fsFileGetSize(&handle, &size));
    if (openError || sizeError) { return -1; }

    fsFileClose(&handle);
    return size;
}

bool fslib::rename_file(const fslib::Path &oldPath, const fslib::Path &newPath)
{
    FsFileSystem *filesystem{};
    const bool validPaths  = oldPath.is_valid() && newPath.is_valid();
    const bool deviceMatch = validPaths && oldPath.get_device_name() == newPath.get_device_name();
    const bool found       = deviceMatch && fslib::get_file_system_by_device_name(oldPath.get_device_name(), &filesystem);
    if (!validPaths || !deviceMatch || !found) { return false; }

    const bool renameError = error::occurred(fsFsRenameFile(filesystem, oldPath.get_path(), newPath.get_path()));
    if (renameError) { return false; }
    return true;
}

bool fslib::get_file_timestamp(const fslib::Path &filePath, FsTimeStampRaw &stampOut)
{
    FsFileSystem *filesystem{};
    const bool isValid = filePath.is_valid();
    const bool found   = isValid && fslib::get_file_system_by_device_name(filePath.get_device_name(), &filesystem);
    if (!isValid || !found) { return false; }

    const bool stampError = error::occurred(fsFsGetFileTimeStampRaw(filesystem, filePath.get_path(), &stampOut));
    if (stampError) { return false; }

    return true;
}