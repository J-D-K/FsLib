#include "dev.hpp"

#include "File.hpp"
#include "file_functions.hpp"

#include <fcntl.h>
#include <string_view>
#include <switch.h>
#include <sys/iosupport.h>
#include <unordered_map>

/*
    This file is a mess, but it kind of has to be :(
*/

// Declarations for newlib.
extern "C"
{
    static int fslib_dev_open(struct _reent *reent, void *id, const char *path, int flags, int mode);
    static int fslib_dev_close(struct _reent *reent, void *id);
    static ssize_t fslib_dev_write(struct _reent *reent, void *id, const char *buffer, size_t bufferSize);
    static ssize_t fslib_dev_read(struct _reent *reent, void *id, char *buffer, size_t bufferSize);
    static ssize_t fslib_dev_seek(struct _reent *reent, void *id, off_t offset, int direction);
}

namespace
{
    // Map of files paired with their ID.
    std::unordered_map<int, fslib::File> s_fileMap;

    // This is how we get stdio calls to the sdmc and redirect them to FsLib files instead.
    constexpr devoptab_t SDMC_DEVOPT = {.name       = "sdmc",
                                        .structSize = sizeof(int),
                                        .open_r     = fslib_dev_open,
                                        .close_r    = fslib_dev_close,
                                        .write_r    = fslib_dev_write,
                                        .read_r     = fslib_dev_read,
                                        .seek_r     = fslib_dev_seek};
} // namespace

bool fslib::dev::initialize_sdmc()
{
    // fs_dev is a mess and I don't want to use it. Kill it.
    fsdevUnmountAll();

    // Add my own SD device to newlib.
    if (AddDevice(&SDMC_DEVOPT) < 0) { return false; }

    return true;
}

// Defintions of functions above.
extern "C"
{
    static int fslib_dev_open(struct _reent *reent, void *fileID, const char *path, int flags, int mode)
    {
        // This is to keep track of which file id we're on.
        static int currentFileID = 0;

        // Switch FS flags used to open file.
        uint32_t openFlags = 0;

        // This is our path so we don't need to constantly construct a path.
        const fslib::Path filePath{path};
        if (!filePath.is_valid())
        {
            reent->_errno = ENOENT;
            return -1;
        }

        switch (flags & O_ACCMODE)
        {
            case O_RDONLY: openFlags = FsOpenMode_Read; break;
            case O_WRONLY: openFlags = FsOpenMode_Write; break;
            case O_RDWR: openFlags = FsOpenMode_Read | FsOpenMode_Write; break;
            default:
            {
                reent->_errno = EINVAL;
                return -1;
            }
            break;
        }

        const bool append     = (flags & O_APPEND);
        const bool create     = (flags & O_CREAT);
        const bool fileExists = fslib::file_exists(filePath);
        if (append && !fileExists) { openFlags |= FsOpenMode_Create; }
        else if (append) { openFlags |= FsOpenMode_Append; }
        else if (create) { openFlags |= FsOpenMode_Create; }

        const int newFileID         = currentFileID++;
        *static_cast<int *>(fileID) = newFileID;
        auto emplaced               = s_fileMap.try_emplace(newFileID, filePath, openFlags);
        if (!emplaced.second || !emplaced.first->second.is_open())
        {
            s_fileMap.erase(newFileID);
            return -1;
        }

        return 0;
    }

    static int fslib_dev_close(struct _reent *reent, void *fileID)
    {
        // Dereference pointer to int.
        const int id = *static_cast<int *>(fileID);

        const auto findFile = s_fileMap.find(id);
        if (findFile == s_fileMap.end())
        {
            reent->_errno = EBADF;
            return -1;
        }

        s_fileMap.erase(findFile);
        return 0;
    }

    static ssize_t fslib_dev_write(struct _reent *reent, void *fileID, const char *buffer, size_t bufferSize)
    {
        const int id = *static_cast<int *>(fileID);

        const auto findFile = s_fileMap.find(id);
        if (findFile == s_fileMap.end())
        {
            reent->_errno = EBADF;
            return -1;
        }

        return findFile->second.write(buffer, bufferSize);
    }

    static ssize_t fslib_dev_read(struct _reent *reent, void *fileID, char *buffer, size_t bufferSize)
    {
        const int id        = *static_cast<int *>(fileID);
        const auto findFile = s_fileMap.find(id);
        if (findFile == s_fileMap.end())
        {
            reent->_errno = EBADF;
            return -1;
        }

        return findFile->second.read(buffer, bufferSize);
    }

    static ssize_t fslib_dev_seek(struct _reent *reent, void *fileID, off_t offset, int direction)
    {
        const int id        = *static_cast<int *>(fileID);
        const auto findFile = s_fileMap.find(id);
        if (findFile == s_fileMap.end())
        {
            reent->_errno = EBADF;
            return -1;
        }

        fslib::File &target = findFile->second;
        switch (direction)
        {
            case SEEK_SET: target.seek(offset, target.BEGINNING); break;
            case SEEK_CUR: target.seek(offset, target.CURRENT); break;
            case SEEK_END: target.seek(offset, target.END); break;
        }

        return target.tell();
    }
}
