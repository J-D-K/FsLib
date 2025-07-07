#include "Directory.hpp"
#include "error.hpp"
#include "fslib.hpp"
#include <algorithm>
#include <cstring>
#include <string>

/// @brief Function used to sort by directories->alphabetically.
/// @param entryA
/// @param entryB
/// @return
static bool compare_entries(const FsDirectoryEntry &entryA, const FsDirectoryEntry &entryB);

fslib::Directory::Directory(const fslib::Path &directoryPath, bool sortedListing)
{
    Directory::open(directoryPath, sortedListing);
}

fslib::Directory::Directory(Directory &&directory) noexcept
{
    *this = std::move(directory);
}

fslib::Directory &fslib::Directory::operator=(Directory &&directory) noexcept
{
    // Start by copying this to make sure we have EVERYTHING~
    std::memcpy(&m_directoryHandle, &directory.m_directoryHandle, sizeof(FsDir));
    m_directoryList = std::move(directory.m_directoryList);
    m_entryCount = directory.m_entryCount;
    m_wasRead = directory.m_wasRead;

    std::memset(&directory.m_directoryHandle, 0x00, sizeof(FsDir));
    m_directoryList = nullptr; // Not really sure if this is needed after std::move, but jic.
    m_entryCount = 0;
    m_wasRead = 0;

    return *this;
}

void fslib::Directory::open(const fslib::Path &directoryPath, bool sortedListing)
{
    // This so directories can be reused.
    m_wasRead = false;

    if (!directoryPath.is_valid())
    {
        error::occurred(error::codes::INVALID_PATH);
        return;
    }

    FsFileSystem *filesystem;
    if (!fslib::get_file_system_by_device_name(directoryPath.get_device_name(), &filesystem))
    {
        error::occurred(error::codes::DEVICE_NOT_FOUND);
        return;
    }

    if (error::occurred(fsFsOpenDirectory(filesystem,
                                          directoryPath.get_path(),
                                          FsDirOpenMode_ReadDirs | FsDirOpenMode_ReadFiles,
                                          &m_directoryHandle)))
    {
        return;
    }

    if (error::occurred(fsDirGetEntryCount(&m_directoryHandle, &m_entryCount)))
    {
        return;
    }

    // Allocate the array. Reallocating this should free a previous array if there is one.
    m_directoryList = std::make_unique<FsDirectoryEntry[]>(m_entryCount);

    // This is how many entries the function says are read.
    int64_t totalEntriesRead = 0;
    if (error::occurred(fsDirRead(&m_directoryHandle, &totalEntriesRead, m_entryCount, m_directoryList.get())) ||
        totalEntriesRead != m_entryCount)
    {
        return;
    }

    // Sort if requested.
    if (sortedListing)
    {
        std::sort(m_directoryList.get(), m_directoryList.get() + m_entryCount, compare_entries);
    }

    // Close and success?
    Directory::close();
    m_wasRead = true;
}

bool fslib::Directory::is_open() const noexcept
{
    return m_wasRead;
}

int64_t fslib::Directory::get_count() const noexcept
{
    return m_entryCount;
}

int64_t fslib::Directory::get_entry_size(int index) const noexcept
{
    if (index < 0 || index >= m_entryCount)
    {
        return 0;
    }
    return m_directoryList[index].file_size;
}

const char *fslib::Directory::get_entry(int index) const noexcept
{
    if (index < 0 || index >= m_entryCount)
    {
        return nullptr;
    }
    return m_directoryList[index].name;
}

bool fslib::Directory::is_directory(int index) const noexcept
{
    if (index < 0 || index >= m_entryCount)
    {
        return false;
    }
    return m_directoryList[index].type == FsDirEntryType_Dir;
}

fslib::Directory::operator bool() const noexcept
{
    return m_wasRead;
}

const char *fslib::Directory::operator[](int index) const noexcept
{
    if (index < 0 || index >= m_entryCount)
    {
        return nullptr;
    }
    return m_directoryList[index].name;
}

void fslib::Directory::close() noexcept
{
    fsDirClose(&m_directoryHandle);
}

static bool compare_entries(const FsDirectoryEntry &entryA, const FsDirectoryEntry &entryB)
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
