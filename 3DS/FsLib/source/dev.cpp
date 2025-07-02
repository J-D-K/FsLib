#include "fslib.hpp"
#include <fcntl.h>
#include <memory>
#include <string>
#include <sys/iosupport.h>
#include <unordered_map>

/*
    This is to help make FsLib work like a drop-in replacement for ctrulib's archive_dev. It's more of a compatibility layer to make libs work
    than a full replacement. It's basically a wrapper for newlib files -> FsLib files.
*/

// Declarations.
extern "C"
{
    static int fslibDevOpen(struct _reent *reent, void *fileID, const char *filePath, int flags, int mode);
    static int fslibDevClose(struct _reent *reent, void *fileID);
    static ssize_t fslibDevWrite(struct _reent *reent, void *fileID, const char *buffer, size_t bufferSize);
    static ssize_t fslibDevRead(struct _reent *reent, void *fileID, char *buffer, size_t bufferSize);
    static off_t fslibDevSeek(struct _reent *reent, void *fileID, off_t offset, int origin);
}

namespace
{
    // SD devoptab
    constexpr devoptab_t SDMC_DEVOPTAB = {.name = "sdmc",
                                          .structSize = sizeof(unsigned int),
                                          .open_r = fslibDevOpen,
                                          .close_r = fslibDevClose,
                                          .write_r = fslibDevWrite,
                                          .read_r = fslibDevRead,
                                          .seek_r = fslibDevSeek};
    // Map of open files.
    std::unordered_map<int, fslib::File> s_fileMap;
} // namespace

// This checks if the file exists in the map.
static inline bool fileIsValid(int fileID)
{
    return s_fileMap.find(fileID) != s_fileMap.end();
}

// This "installs" the SDMC_DEVOPTAB in place of archive_dev's
bool fslib::dev::initializeSDMC()
{
    if (AddDevice(&SDMC_DEVOPTAB) < 0)
    {
        return false;
    }
    return true;
}

extern "C"
{
    static int fslibDevOpen(struct _reent *reent, void *fileID, const char *filePath, int flags, int mode)
    {
        // This is how we'll keep track of the current file id.
        static int currentFileID = 0;

        // Path we're going to use. UTF-8 -> UTF-16 conversion is scoped so it's free asap.
        fslib::Path path;
        {
            char16_t pathBuffer[fslib::MAX_PATH] = {0};
            utf8_to_utf16(reinterpret_cast<uint16_t *>(pathBuffer),
                          reinterpret_cast<const uint8_t *>(filePath),
                          fslib::MAX_PATH);
            path = pathBuffer;
        }

        // I'm not rewriting this check.
        if (!path.isValid())
        {
            reent->_errno = ENOENT;
            return -1;
        }

        uint32_t openFlags = 0;
        switch (flags & O_ACCMODE)
        {
            case O_RDONLY:
            {
                openFlags = FS_OPEN_READ;
            }
            break;

            case O_WRONLY:
            {
                openFlags = FS_OPEN_WRITE;
            }
            break;

            case O_RDWR:
            {
                openFlags = FS_OPEN_READ | FS_OPEN_WRITE;
            }
            break;

            default:
            {
                reent->_errno = EINVAL;
                return -1;
            }
            break;
        }

        // The first condition is a precaution.
        if (flags & O_APPEND && !fslib::fileExists(path))
        {
            openFlags |= FS_OPEN_CREATE;
        }
        else if (flags & O_APPEND)
        {
            openFlags |= FS_OPEN_APPEND;
        }
        else if (flags & O_CREAT)
        {
            openFlags |= FS_OPEN_CREATE;
        }
        // Get and increment file id
        int newFileID = currentFileID++;
        // Set the pointer to it.
        *reinterpret_cast<int *>(fileID) = newFileID;

        // Open file in map.
        s_fileMap[newFileID].open(path, openFlags);
        if (!s_fileMap[newFileID].isOpen())
        {
            // Erase so map continues to work as intended.
            s_fileMap.erase(newFileID);
            return -1;
        }
        // Should be fine.
        return 0;
    }

    int fslibDevClose(struct _reent *reent, void *fileID)
    {
        // Cast and dereference pointer.
        int id = *reinterpret_cast<int *>(fileID);
        // Check to make sure we don't try to free and erase a non-existant file.
        if (!fileIsValid(id))
        {
            reent->_errno = EBADF;
            return -1;
        }
        // Just erasing from the map will cause the destructor to be called and the handle closed.
        s_fileMap.erase(id);
        return 0;
    }

    ssize_t fslibDevWrite(struct _reent *reent, void *fileID, const char *buffer, size_t bufferSize)
    {
        int id = *reinterpret_cast<int *>(fileID);
        if (!fileIsValid(id))
        {
            reent->_errno = EBADF;
            return -1;
        }
        return s_fileMap[id].write(buffer, bufferSize);
    }

    ssize_t fslibDevRead(struct _reent *reent, void *fileID, char *buffer, size_t bufferSize)
    {
        int id = *reinterpret_cast<int *>(fileID);
        if (!fileIsValid(id))
        {
            reent->_errno = EBADF;
            return -1;
        }
        return s_fileMap[id].read(buffer, bufferSize);
    }

    off_t fslibDevSeek(struct _reent *reent, void *fileID, off_t offset, int origin)
    {
        int id = *reinterpret_cast<int *>(fileID);
        if (!fileIsValid(id))
        {
            reent->_errno = EBADF;
            return -1;
        }

        switch (origin)
        {
            case SEEK_SET:
            {
                s_fileMap[id].seek(offset, s_fileMap[id].beginning);
            }
            break;

            case SEEK_CUR:
            {
                s_fileMap[id].seek(offset, s_fileMap[id].current);
            }
            break;

            case SEEK_END:
            {
                s_fileMap[id].seek(offset, s_fileMap[id].end);
            }
            break;
        }
        return s_fileMap[id].tell();
    }
}
