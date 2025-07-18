#include "directory_functions.hpp"

#include "error.hpp"
#include "fslib.hpp"

#include <string_view>
#include <switch.h>

bool fslib::create_directory(const fslib::Path &directoryPath)
{
    if (!directoryPath.is_valid()) { return false; }

    FsFileSystem *filesystem{};
    const std::string_view device = directoryPath.get_device_name();
    const char *path              = directoryPath.get_path();
    const bool deviceFound        = fslib::get_file_system_by_device_name(device, &filesystem);
    const bool dirError           = deviceFound && error::occurred(fsFsCreateDirectory(filesystem, path));
    if (!deviceFound || dirError) { return false; }

    return true;
}

bool fslib::create_directories_recursively(const fslib::Path &directoryPath)
{
    size_t slashPosition = directoryPath.find_first_of('/');
    if (!directoryPath.is_valid() || slashPosition == directoryPath.NOT_FOUND) { return false; }

    ++slashPosition;
    do {
        slashPosition                      = directoryPath.find_first_of('/', slashPosition);
        const fslib::Path currentDirectory = directoryPath.sub_path(slashPosition);
        const bool directoryExists         = fslib::directory_exists(currentDirectory);
        const bool createFailed            = !directoryExists && !fslib::create_directory(currentDirectory);
        if (!directoryExists && createFailed) { return false; }
        ++slashPosition;
    } while (slashPosition < directoryPath.get_length());
    return true;
}

bool fslib::delete_directory(const fslib::Path &directoryPath)
{
    if (!directoryPath.is_valid()) { return false; }

    FsFileSystem *filesystem{};
    const std::string_view device = directoryPath.get_device_name();
    const char *path              = directoryPath.get_path();
    const bool deviceFound        = fslib::get_file_system_by_device_name(device, &filesystem);
    const bool deleteError        = deviceFound && error::occurred(fsFsDeleteDirectory(filesystem, path));
    if (!deviceFound || deleteError) { return false; }
    return true;
}

bool fslib::delete_directory_recursively(const fslib::Path &directoryPath)
{
    fslib::Directory targetDirectory{directoryPath};
    if (!targetDirectory.is_open()) { return false; }

    const int64_t count = targetDirectory.get_count();
    for (int64_t i = 0; i < count; i++)
    {
        const fslib::Path targetPath = directoryPath / targetDirectory[i];
        const bool isDirectory       = targetDirectory.is_directory(i);
        const bool dirDeleted        = isDirectory && fslib::delete_directory_recursively(targetPath);
        const bool fileDeleted       = !isDirectory && fslib::delete_file(targetPath);
        if (!dirDeleted && !fileDeleted) { return false; }
    }

    // This is to prevent failure from trying to delete the root. I think Nintendo's own implementation doesn't do this?
    const size_t pathLength  = std::char_traits<char>::length(directoryPath.get_path());
    const bool attemptDelete = pathLength > 1;
    const bool deleteFailed  = attemptDelete && !fslib::delete_directory(directoryPath);
    if (attemptDelete && deleteFailed) { return false; }
    return true;
}

bool fslib::directory_exists(const fslib::Path &directoryPath)
{
    static constexpr uint32_t FLAGS_DIR_OPEN = FsDirOpenMode_ReadDirs | FsDirOpenMode_ReadFiles;

    if (!directoryPath.is_valid()) { return false; }

    FsFileSystem *filesystem{};
    FsDir handle{};
    const std::string_view device = directoryPath.get_device_name();
    const char *path              = directoryPath.get_path();
    const bool deviceFound        = fslib::get_file_system_by_device_name(device, &filesystem);
    const bool dirError = deviceFound && error::occurred(fsFsOpenDirectory(filesystem, path, FLAGS_DIR_OPEN, &handle));
    if (!deviceFound || dirError) { return false; }
    fsDirClose(&handle);
    return true;
}

bool fslib::rename_directory(const fslib::Path &oldPath, const fslib::Path &newPath)
{
    const bool pathsValid = oldPath.is_valid() && newPath.is_valid();
    const bool sameDevice = oldPath.get_device_name() == newPath.get_device_name();
    if (!pathsValid || !sameDevice) { return false; }

    FsFileSystem *filesystem{};
    const std::string_view device = oldPath.get_device_name();
    const char *pathA             = oldPath.get_path();
    const char *pathB             = newPath.get_path();
    const bool deviceFound        = fslib::get_file_system_by_device_name(device, &filesystem);
    const bool renameError        = deviceFound && error::occurred(fsFsRenameDirectory(filesystem, pathA, pathB));
    if (!deviceFound || renameError) { return false; }
    return true;
}
