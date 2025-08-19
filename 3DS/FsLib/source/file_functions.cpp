#include "fslib.hpp"
#include "string.hpp"

bool fslib::create_file(const fslib::Path &filePath, uint64_t fileSize)
{
    FS_Archive archive{};
    const bool found = fslib::get_archive_by_device_name(filePath.get_device(), archive);
    if (!found) { return false; }

    const bool createError = error::libctru(FSUSER_CreateFile(archive, filePath.get_fs_path(), 0, fileSize));
    if (createError) { return false; }
    return true;
}

bool fslib::file_exists(const fslib::Path &filePath)
{
    FS_Archive archive{};
    const bool found = fslib::get_archive_by_device_name(filePath.get_device(), archive);
    if (!found) { return false; }

    Handle handle{};
    const bool openError = error::libctru(FSUSER_OpenFile(&handle, archive, filePath.get_fs_path(), FS_OPEN_READ, 0));
    if (openError) { return false; }

    // Gonna record this, but not fatal it.
    error::libctru(FSFILE_Close(handle));

    return true;
}

bool fslib::get_file_size(const fslib::Path &filePath, uint64_t &sizeOut)
{
    FS_Archive archive{};
    const bool found = fslib::get_archive_by_device_name(filePath.get_device(), archive);
    if (!found) { return false; }

    Handle handle{};
    const bool openError = error::libctru(FSUSER_OpenFile(&handle, archive, filePath.get_fs_path(), FS_OPEN_READ, 0));
    const bool sizeError = !openError && error::libctru(FSFILE_GetSize(handle, &sizeOut));
    if (openError || sizeError)
    {
        FSFILE_Close(handle);
        return false;
    }

    FSFILE_Close(handle);
    return true;
}

bool fslib::rename_file(const fslib::Path &oldPath, const fslib::Path &newPath)
{
    FS_Archive archiveA{}, archiveB{};
    const bool foundA = fslib::get_archive_by_device_name(oldPath.get_device(), archiveA);
    const bool foundB = fslib::get_archive_by_device_name(newPath.get_device(), archiveB);
    if (!foundA || !foundB || archiveA != archiveB) { return false; }

    const bool error = error::libctru(FSUSER_RenameFile(archiveA, oldPath.get_fs_path(), archiveB, newPath.get_fs_path()));
    if (error) { return false; }
    return true;
}

bool fslib::delete_file(const fslib::Path &filePath)
{
    FS_Archive archive{};
    const bool found = fslib::get_archive_by_device_name(filePath.get_device(), archive);
    if (!found) { return false; }

    const bool deleteError = error::libctru(FSUSER_DeleteFile(archive, filePath.get_fs_path()));
    if (deleteError) { return false; }
    return true;
}
