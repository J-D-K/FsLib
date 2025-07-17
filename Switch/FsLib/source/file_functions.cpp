#include "file_functions.hpp"

#include "error.hpp"
#include "fslib.hpp"

#include <switch.h>

bool fslib::create_file(const fslib::Path &filePath, int64_t fileSize)
{
    if(!filePath.is_valid()) { return false; }

    FsFileSystem *filesystem{};
    const std::string_view device = filePath.get_device_name();
    const char *path              = filePath.get_path();
    const bool deviceFound        = fslib::get_file_system_by_device_name(device, &filesystem);
    const bool createError        = deviceFound && error::occurred(fsFsCreateFile(filesystem, path, fileSize, 0));
    if(!deviceFound || createError) { return false; }
    return true;
}

bool fslib::file_exists(const fslib::Path &filePath)
{
    if(!filePath.is_valid()) { return false; }

    FsFileSystem *filesystem{};
    FsFile handle{};
    const std::string_view device = filePath.get_device_name();
    const char *path              = filePath.get_path();
    const bool deviceFound        = fslib::get_file_system_by_device_name(device, &filesystem);
    const bool openError          = deviceFound && error::occurred(fsFsOpenFile(filesystem, path, FsOpenMode_Read, &handle));
    if(!deviceFound || openError) { return false; }

    fsFileClose(&handle);
    return true;
}

bool fslib::delete_file(const fslib::Path &filePath)
{
    if(!filePath.is_valid()) { return false; }

    FsFileSystem *filesystem{};
    const std::string_view device = filePath.get_device_name();
    const char *path              = filePath.get_path();
    const bool deviceFound        = fslib::get_file_system_by_device_name(device, &filesystem);
    const bool deleteError        = deviceFound && error::occurred(fsFsDeleteFile(filesystem, path));
    if(!deviceFound || deleteError) { return false; }
    return true;
}

int64_t fslib::get_file_size(const fslib::Path &filePath)
{
    if(!filePath.is_valid()) { return -1; }

    FsFileSystem *filesystem{};
    const std::string_view device = filePath.get_device_name();
    const char *path              = filePath.get_path();
    const bool deviceFound        = fslib::get_file_system_by_device_name(device, &filesystem);
    if(!deviceFound) { return -1; }

    FsFile handle{};
    int64_t size{};
    const bool openError = error::occurred(fsFsOpenFile(filesystem, path, FsOpenMode_Read, &handle));
    const bool sizeError = !openError && error::occurred(fsFileGetSize(&handle, &size));
    if(openError || sizeError) { return -1; }

    fsFileClose(&handle);
    return size;
}

bool fslib::rename_file(const fslib::Path &oldPath, const fslib::Path &newPath)
{
    const bool validPaths  = oldPath.is_valid() && newPath.is_valid();
    const bool deviceMatch = oldPath.get_device_name() == newPath.get_device_name();
    if(!validPaths || !deviceMatch) { return false; }

    FsFileSystem *filesystem{};
    const std::string_view device = oldPath.get_device_name();
    const char *pathA             = oldPath.get_path();
    const char *pathB             = newPath.get_path();
    const bool deviceFound        = fslib::get_file_system_by_device_name(device, &filesystem);
    const bool renameError        = deviceFound && error::occurred(fsFsRenameFile(filesystem, pathA, pathB));
    if(!deviceFound || renameError) { return false; }
    return true;
}
