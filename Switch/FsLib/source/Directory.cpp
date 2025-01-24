#include "Directory.hpp"
#include "errorCommon.h"
#include "fslib.hpp"
#include "string.hpp"
#include <algorithm>
#include <cstring>
#include <string>

// fslib global error string.
extern std::string g_fslibErrorString;

// Sorts by entry type, then alphabetically.
static bool compareEntries(const FsDirectoryEntry &entryA, const FsDirectoryEntry &entryB)
{
    if (entryA.type != entryB.type)
    {
        return entryA.type == FsDirEntryType_Dir;
    }

    size_t entryALength = std::char_traits<char>::length(entryA.name);
    size_t entryBLength = std::char_traits<char>::length(entryB.name);
    size_t shortestEntry = entryALength < entryBLength ? entryALength : entryBLength;
    for (size_t i = 0; i < shortestEntry;)
    {
        int charA = std::tolower(entryA.name[i]);
        int charB = std::tolower(entryB.name[i]);
        if (charA != charB)
        {
            return charA < charB;
        }
    }
    return false;
}

fslib::Directory::Directory(const fslib::Path &directoryPath, bool sortedListing)
{
    Directory::open(directoryPath, sortedListing);
}

void fslib::Directory::open(const fslib::Path &directoryPath, bool sortedListing)
{
    // This so directories can be reused.
    m_wasRead = false;

    if (!directoryPath.isValid())
    {
        g_fslibErrorString = ERROR_INVALID_PATH;
        return;
    }

    FsFileSystem *fileSystem;
    if (!fslib::getFileSystemByDeviceName(directoryPath.getDeviceName(), &fileSystem))
    {
        g_fslibErrorString = ERROR_DEVICE_NOT_FOUND;
        return;
    }

    Result fsError =
        fsFsOpenDirectory(fileSystem, directoryPath.getPath(), FsDirOpenMode_ReadDirs | FsDirOpenMode_ReadFiles, &m_directoryHandle);
    if (R_FAILED(fsError))
    {
        g_fslibErrorString = string::getFormattedString("Error opening directory: 0x%X.", fsError);
        return;
    }

    fsError = fsDirGetEntryCount(&m_directoryHandle, &m_entryCount);
    if (R_FAILED(fsError))
    {
        g_fslibErrorString = string::getFormattedString("Error opening reading entry count: 0x%X.", fsError);
        return;
    }

    // Allocate the array. Reallocating this should free a previous array if there is one.
    m_directoryList = std::make_unique<FsDirectoryEntry[]>(m_entryCount);

    // This is how many entries the function says are read.
    int64_t totalEntriesRead = 0;
    fsError = fsDirRead(&m_directoryHandle, &totalEntriesRead, m_entryCount, m_directoryList.get());
    if (R_FAILED(fsError) || totalEntriesRead != m_entryCount)
    {
        Directory::close();
        g_fslibErrorString = string::getFormattedString("Error reading entries: 0x%X. %02d/%02d entries were read.");
        return;
    }

    // Sort if requested.
    if (sortedListing)
    {
        std::sort(m_directoryList.get(), m_directoryList.get() + m_entryCount, compareEntries);
    }
    // Close and success?
    Directory::close();
    m_wasRead = true;
}

bool fslib::Directory::isOpen(void) const
{
    return m_wasRead;
}

int64_t fslib::Directory::getCount(void) const
{
    return m_entryCount;
}

int64_t fslib::Directory::getEntrySize(int index) const
{
    if (index < 0 || index >= m_entryCount)
    {
        return 0;
    }
    return m_directoryList[index].file_size;
}

const char *fslib::Directory::getEntry(int index) const
{
    if (index < 0 || index >= m_entryCount)
    {
        return nullptr;
    }
    return m_directoryList[index].name;
}

bool fslib::Directory::isDirectory(int index) const
{
    if (index < 0 || index >= m_entryCount)
    {
        return false;
    }
    return m_directoryList[index].type == FsDirEntryType_Dir;
}

const char *fslib::Directory::operator[](int index) const
{
    if (index < 0 || index >= m_entryCount)
    {
        return nullptr;
    }
    return m_directoryList[index].name;
}

void fslib::Directory::close(void)
{
    fsDirClose(&m_directoryHandle);
}
