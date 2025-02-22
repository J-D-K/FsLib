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
    static int fslib_dev_open(struct _reent *reent, void *fileID, const char *path, int flags, int mode);
    static int fslib_dev_close(struct _reent *reent, void *fileID);
    static ssize_t fslib_dev_write(struct _reent *reent, void *fileID, const char *buffer, size_t bufferSize);
    static ssize_t fslib_dev_read(struct _reent *reent, void *fileID, char *buffer, size_t bufferSize);
    static ssize_t fslib_dev_seek(struct _reent *reent, void *fileID, off_t offset, int direction);
}

namespace
{
    // Map of files paired with their ID.
    std::unordered_map<int, fslib::File> s_fileMap;

    // This is how we get stdio calls to the sdmc and redirect them to FsLib files instead.
    constexpr devoptab_t SDMC_DEVOPT = {.name = "sdmc",
                                        .structSize = sizeof(int),
                                        .open_r = fslib_dev_open,
                                        .close_r = fslib_dev_close,
                                        .write_r = fslib_dev_write,
                                        .read_r = fslib_dev_read,
                                        .seek_r = fslib_dev_seek};
} // namespace

bool fslib::dev::initialize_sdmc(void)
{
    // This should kill fs_dev.
    fsdevUnmountAll();

    // Add my own SD device to newlib.
    if (AddDevice(&SDMC_DEVOPT) < 0)
    {
        return false;
    }

    return true;
}

// This will return if the file id exists in the map.
static inline bool file_is_valid(int fileID)
{
    if (s_fileMap.find(fileID) == s_fileMap.end())
    {
        return false;
    }
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
        fslib::Path filePath = path;
        if (!filePath.is_valid())
        {
            reent->_errno = ENOENT;
            return -1;
        }

        switch (flags & O_ACCMODE)
        {
            case O_RDONLY:
            {
                openFlags = FsOpenMode_Read;
            }
            break;

            case O_WRONLY:
            {
                openFlags = FsOpenMode_Write;
            }
            break;

            case O_RDWR:
            {
                openFlags = FsOpenMode_Read | FsOpenMode_Write;
            }
            break;

            default:
            {
                reent->_errno = EINVAL;
                return -1;
            }
            break;
        }

        if (flags & O_APPEND && !fslib::file_exists(filePath))
        {
            openFlags |= FsOpenMode_Create;
        }
        else if (flags & O_APPEND)
        {
            openFlags |= FsOpenMode_Append;
        }
        else if (flags & O_CREAT)
        {
            openFlags |= FsOpenMode_Create;
        }

        // Get ID for new file.
        int newFileID = currentFileID++;

        // Assign passed memory pointer.
        *reinterpret_cast<int *>(fileID) = newFileID;

        // Attempt to open and map it at same time.
        s_fileMap[newFileID].open(filePath, openFlags);
        if (!s_fileMap[newFileID].is_open())
        {
            // It failed. Return failure.
            s_fileMap.erase(newFileID);
            return -1;
        }
        return 0;
    }

    static int fslib_dev_close(struct _reent *reent, void *fileID)
    {
        // Dereference pointer to int.
        int targetFileID = *reinterpret_cast<int *>(fileID);

        // Check to make sure it exists in map.
        if (!file_is_valid(targetFileID))
        {
            reent->_errno = EBADF;
            return -1;
        }
        // Erasing from the map will cause the destructor to get called and take care of everything.
        s_fileMap.erase(targetFileID);
        // Assume everything went as expected.
        return 0;
    }

    static ssize_t fslib_dev_write(struct _reent *reent, void *fileID, const char *buffer, size_t bufferSize)
    {
        // Same as above
        int targetFileID = *reinterpret_cast<int *>(fileID);
        if (!file_is_valid(targetFileID))
        {
            reent->_errno = EBADF;
            return -1;
        }
        return s_fileMap.at(targetFileID).write(buffer, bufferSize);
    }

    static ssize_t fslib_dev_read(struct _reent *reent, void *fileID, char *buffer, size_t bufferSize)
    {
        int targetFileID = *reinterpret_cast<int *>(fileID);
        if (!file_is_valid(targetFileID))
        {
            reent->_errno = EBADF;
            return -1;
        }
        return s_fileMap.at(targetFileID).read(buffer, bufferSize);
    }

    static ssize_t fslib_dev_seek(struct _reent *reent, void *fileID, off_t offset, int direction)
    {
        int targetFileID = *reinterpret_cast<int *>(fileID);
        if (!file_is_valid(targetFileID))
        {
            reent->_errno = EBADF;
            return -1;
        }

        switch (direction)
        {
            case SEEK_SET:
            {
                s_fileMap.at(targetFileID).seek(offset, s_fileMap.at(targetFileID).BEGINNING);
            }
            break;

            case SEEK_CUR:
            {
                s_fileMap.at(targetFileID).seek(offset, s_fileMap.at(targetFileID).CURRENT);
            }
            break;

            case SEEK_END:
            {
                s_fileMap.at(targetFileID).seek(offset, s_fileMap.at(targetFileID).END);
            }
            break;
        }
        return s_fileMap.at(targetFileID).tell();
    }
}
