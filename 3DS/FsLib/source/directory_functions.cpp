#include "error.hpp"
#include "fslib.hpp"
#include "string.hpp"

#include <3ds.h>

bool fslib::directory_exists(const fslib::Path &directoryPath)
{
    FS_Archive archive{};
    const bool found = fslib::get_archive_by_device_name(directoryPath.get_device(), archive);
    if (!found) { return false; }

    Handle dirHandle{};
    const bool openError = error::libctru(FSUSER_OpenDirectory(&dirHandle, archive, directoryPath.get_fs_path()));
    if (openError) { return false; }

    // Gonna record this cause it could be important. VERY IMPORTANT!
    error::libctru(FSDIR_Close(dirHandle));
    return true;
}

bool fslib::create_directory(const fslib::Path &directoryPath)
{
    FS_Archive archive{};
    const bool found = fslib::get_archive_by_device_name(directoryPath.get_device(), archive);
    if (!found) { return false; }

    const bool createError = error::libctru(FSUSER_CreateDirectory(archive, directoryPath.get_fs_path(), 0));
    if (createError) { return false; }
    return true;
}

bool fslib::create_directory_recursively(const fslib::Path &directoryPath)
{
    static constexpr char16_t CHAR16_SLASH = u'/';

    size_t slash = directoryPath.find_first_of(CHAR16_SLASH);
    if (slash == directoryPath.NOT_FOUND) { return false; }
    ++slash;

    const size_t pathLength = directoryPath.get_length();
    do {
        slash = directoryPath.find_first_of(CHAR16_SLASH, slash);
        if (slash == directoryPath.NOT_FOUND) { break; }

        const fslib::Path currentDir = directoryPath.sub_path(slash);
        const bool exists            = fslib::directory_exists(currentDir);
        const bool createError       = !exists && !fslib::create_directory(currentDir);
        if (!exists && createError) { return false; }

        ++slash;
    } while (slash < pathLength);

    return true;
}

bool fslib::rename_directory(const fslib::Path &oldPath, const fslib::Path &newPath)
{
    FS_Archive archiveA{}, archiveB{};
    const bool archiveAExists = fslib::get_archive_by_device_name(oldPath.get_device(), archiveA);
    const bool archiveBExists = fslib::get_archive_by_device_name(newPath.get_device(), archiveB);
    const bool archivesMatch  = archiveAExists && archiveBExists && archiveA == archiveB;
    if (!archiveAExists || !archiveBExists || !archivesMatch) { return false; }

    const bool error = error::libctru(FSUSER_RenameDirectory(archiveA, oldPath.get_fs_path(), archiveB, newPath.get_fs_path()));
    if (error) { return false; }
    return true;
}

bool fslib::delete_directory(const fslib::Path &directoryPath)
{
    FS_Archive archive{};
    const bool found = fslib::get_archive_by_device_name(directoryPath.get_device(), archive);
    if (!found) { return false; }

    const bool error = error::libctru(FSUSER_DeleteDirectory(archive, directoryPath.get_fs_path()));
    if (error) { return false; }

    return true;
}

bool fslib::delete_directory_recursively(const fslib::Path &directoryPath)
{
    static constexpr char16_t CHAR16_SLASH = u'/';

    fslib::Directory target{directoryPath};
    if (!target.is_open()) { return false; }

    const int dirCount = target.get_count();
    for (int i = 0; i < dirCount; i++)
    {
        const fslib::Path newTarget{directoryPath / target[i]};

        const bool isDir      = target.is_directory(i);
        const bool deleteDir  = isDir && fslib::delete_directory_recursively(newTarget);
        const bool deleteFile = !isDir && fslib::delete_file(newTarget);
        if (!deleteDir && !deleteFile) { return false; }
    }

    const char16_t *path      = directoryPath.full_path();
    const size_t length       = directoryPath.get_length();
    const char16_t *pathBegin = std::char_traits<char16_t>::find(path, length, CHAR16_SLASH);

    const bool notRoot    = std::char_traits<char16_t>::length(pathBegin) > 1;
    const bool deleteRoot = notRoot && fslib::delete_directory(directoryPath);
    if (notRoot && !deleteRoot) { return false; }

    return true;
}
