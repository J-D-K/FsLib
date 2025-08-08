#include "fslib.hpp"

#include <array>
#include <fcntl.h>
#include <memory>
#include <string>
#include <sys/iosupport.h>
#include <unordered_map>

/*
    This is to help make FsLib work like a drop-in replacement for ctrulib's archive_dev. It's more of a compatibility layer to
   make libs work than a full replacement. It's basically a wrapper for newlib files -> FsLib files.
*/

// Declarations.
extern "C"
{
    static int fslib_dev_open(struct _reent *reent, void *fileID, const char *filePath, int flags, int mode);
    static int fslib_dev_close(struct _reent *reent, void *fileID);
    static ssize_t fslib_dev_write(struct _reent *reent, void *fileID, const char *buffer, size_t bufferSize);
    static ssize_t fslib_dev_read(struct _reent *reent, void *fileID, char *buffer, size_t bufferSize);
    static off_t fslib_dev_seek(struct _reent *reent, void *fileID, off_t offset, int origin);
}

namespace
{
    // SD devoptab
    constexpr devoptab_t SDMC_DEVOPTAB = {.name       = "sdmc",
                                          .structSize = sizeof(unsigned int),
                                          .open_r     = fslib_dev_open,
                                          .close_r    = fslib_dev_close,
                                          .write_r    = fslib_dev_write,
                                          .read_r     = fslib_dev_read,
                                          .seek_r     = fslib_dev_seek};
    // Map of open files.
    std::unordered_map<int, fslib::File> s_fileMap;
} // namespace

// This checks if the file exists in the map.
static inline bool file_is_valid(int fileID) { return s_fileMap.find(fileID) != s_fileMap.end(); }

// This "installs" the SDMC_DEVOPTAB in place of archive_dev's
bool fslib::dev::initializeSDMC()
{
    if (AddDevice(&SDMC_DEVOPTAB) < 0) { return false; }
    return true;
}

extern "C"
{
    static int fslib_dev_open(struct _reent *reent, void *fileID, const char *filePath, int flags, int mode)
    {
        // This is how we'll keep track of the current file id.
        static int currentID{};

        // Path we're going to use. UTF-8 -> UTF-16 conversion is scoped so it's free asap.
        fslib::Path path{};
        {
            std::array<uint16_t, fslib::MAX_PATH> pathBuffer = {0};
            const uint8_t *pathData                          = reinterpret_cast<const uint8_t *>(filePath);
            const ssize_t unitCount                          = utf8_to_utf16(pathBuffer.data(), pathData, fslib::MAX_PATH);
            path                                             = pathBuffer.data();
        }

        if (!path.is_valid())
        {
            reent->_errno = ENOENT;
            return -1;
        }

        uint32_t openFlags{};
        switch (flags & O_ACCMODE)
        {
            case O_RDONLY: openFlags = FS_OPEN_READ; break;
            case O_WRONLY: openFlags = FS_OPEN_WRITE; break;
            case O_RDWR: openFlags = FS_OPEN_READ | FS_OPEN_WRITE; break;
            default: return -1;
        }

        const bool exists       = fslib::file_exists(path);
        const bool append       = flags & O_APPEND;
        const bool appendCreate = append && !exists;
        const bool create       = flags & O_CREAT;

        if (appendCreate) { openFlags |= FS_OPEN_CREATE }
        else if (append) { openFlags |= FS_OPEN_APPEND; }
        else if (create) { openFlags |= FS_OPEN_CREATE; }

        const int newID                  = currentID++;
        *reinterpret_cast<int *>(fileID) = newID;

        fslib::File &newFile = s_fileMap[newID];
        newFile.open(path, openFlags);
        if (!newFile.is_open())
        {
            s_fileMap.erase(newID);
            return -1;
        }
        // Should be fine.
        return 0;
    }

    int fslib_dev_close(struct _reent *reent, void *fileID)
    {
        const int id = *reinterpret_cast<int *>(fileID);
        if (!file_is_valid(id))
        {
            reent->_errno = EBADF;
            return -1;
        }

        s_fileMap.erase(id);
        return 0;
    }

    ssize_t fslib_dev_write(struct _reent *reent, void *fileID, const char *buffer, size_t bufferSize)
    {
        const int id = *reinterpret_cast<int *>(fileID);
        if (!file_is_valid(id))
        {
            reent->_errno = EBADF;
            return -1;
        }

        fslib::File &file = s_fileMap.at(id);
        return file.write(buffer, bufferSize);
    }

    ssize_t fslib_dev_read(struct _reent *reent, void *fileID, char *buffer, size_t bufferSize)
    {
        const int id = *reinterpret_cast<int *>(fileID);
        if (!file_is_valid(id))
        {
            reent->_errno = EBADF;
            return -1;
        }

        fslib::File &file = s_fileMap.at(id);
        return file.read(buffer, bufferSize);
    }

    off_t fslib_dev_seek(struct _reent *reent, void *fileID, off_t offset, int origin)
    {
        const int id = *reinterpret_cast<int *>(fileID);
        if (!file_is_valid(id))
        {
            reent->_errno = EBADF;
            return -1;
        }

        fslib::File &file = s_fileMap.at(id);
        switch (origin)
        {
            case SEEK_SET: file.seek(offset, file.BEGINNING); break;
            case SEEK_CUR: file.seek(offset, file.CURRENT); break;
            case SEEK_END: file.seek(offset, file.END); break;
        }
        return file.tell();
    }
}
