#include "directory_functions.hpp"

#include "error.hpp"
#include "fslib.hpp"

#include <string_view>
#include <switch.h>

namespace
{
    constexpr uint32_t OPEN_DIR_FLAGS = FsDirOpenMode_ReadDirs | FsDirOpenMode_ReadFiles;
}

bool fslib::create_directory(const fslib::Path &directoryPath)
{
    FsFileSystem *filesystem{};
    const bool isValid = directoryPath.is_valid();
    const bool found   = isValid && fslib::get_file_system_by_device_name(directoryPath.get_device_name(), &filesystem);
    if (!isValid || !found) { return false; }

    const bool dirError = error::occurred(fsFsCreateDirectory(filesystem, directoryPath.get_path()));
    if (dirError) { return false; }

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
    FsFileSystem *filesystem{};
    const bool isValid = directoryPath.is_valid();
    const bool found   = isValid && fslib::get_file_system_by_device_name(directoryPath.get_device_name(), &filesystem);
    if (!isValid || !found) { return false; }

    const bool deleteError = error::occurred(fsFsDeleteDirectory(filesystem, directoryPath.get_path()));
    if (deleteError) { return false; }
    return true;
}

bool fslib::delete_directory_recursively(const fslib::Path &directoryPath)
{
    fslib::Directory targetDirectory{directoryPath};
    if (!targetDirectory.is_open()) { return false; }

    for (const fslib::DirectoryEntry &entry : targetDirectory)
    {
        const fslib::Path targetPath{directoryPath / entry};
        const bool isDirectory = entry.is_directory();
        const bool dirDeleted  = isDirectory && fslib::delete_directory_recursively(targetPath);
        const bool fileDeleted = !isDirectory && fslib::delete_file(targetPath);
        if (!dirDeleted && !fileDeleted) { return false; }
    }

    // This is to prevent failure from trying to delete the root.
    const size_t pathLength  = std::char_traits<char>::length(directoryPath.get_path());
    const bool attemptDelete = pathLength > 1;
    const bool deleteFailed  = attemptDelete && !fslib::delete_directory(directoryPath);
    if (attemptDelete && deleteFailed) { return false; }

    return true;
}

bool fslib::directory_exists(const fslib::Path &directoryPath)
{
    FsFileSystem *filesystem{};
    const bool isValid = directoryPath.is_valid();
    const bool found   = isValid && fslib::get_file_system_by_device_name(directoryPath.get_device_name(), &filesystem);
    if (!isValid || !found) { return false; }

    FsDir handle{};
    const bool dirError = error::occurred(fsFsOpenDirectory(filesystem, directoryPath.get_path(), OPEN_DIR_FLAGS, &handle));
    if (dirError) { return false; }

    fsDirClose(&handle);
    return true;
}

bool fslib::rename_directory(const fslib::Path &oldPath, const fslib::Path &newPath)
{
    FsFileSystem *filesystem{};
    const bool pathsValid = oldPath.is_valid() && newPath.is_valid();
    const bool sameDevice = pathsValid && oldPath.get_device_name() == newPath.get_device_name();
    const bool found      = sameDevice && fslib::get_file_system_by_device_name(oldPath.get_device_name(), &filesystem);
    if (!pathsValid || !sameDevice || !found) { return false; }

    const bool renameError = error::occurred(fsFsRenameDirectory(filesystem, oldPath.get_path(), newPath.get_path()));
    if (renameError) { return false; }
    return true;
}

bool fslib::get_directory_entry_count(const fslib::Path &directoryPath, int64_t &countOut)
{
    FsFileSystem *filesystem{};
    const bool pathValid = directoryPath.is_valid();
    const bool found     = fslib::get_file_system_by_device_name(directoryPath.get_device_name(), &filesystem);
    if (!pathValid || !found) { return false; }

    FsDir handle{};
    const bool openError  = error::occurred(fsFsOpenDirectory(filesystem, directoryPath.get_path(), OPEN_DIR_FLAGS, &handle));
    const bool countError = !openError && error::occurred(fsDirGetEntryCount(&handle, &countOut));
    if (openError || countError)
    {
        fsDirClose(&handle);
        return false;
    }

    fsDirClose(&handle);
    return true;
}