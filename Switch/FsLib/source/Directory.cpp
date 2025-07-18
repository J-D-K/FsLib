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

fslib::Directory::Directory(Directory &&directory) { *this = std::move(directory); }

fslib::Directory &fslib::Directory::operator=(Directory &&directory)
{
    // Start by copying this to make sure we have EVERYTHING~
    m_directoryHandle = directory.m_directoryHandle;
    m_directoryList   = std::move(directory.m_directoryList);
    m_entryCount      = directory.m_entryCount;
    m_wasRead         = directory.m_wasRead;

    directory.m_directoryHandle = {0};
    m_directoryList             = nullptr; // Not really sure if this is needed after std::move, but jic.
    m_entryCount                = 0;
    m_wasRead                   = 0;

    return *this;
}

void fslib::Directory::open(const fslib::Path &directoryPath, bool sortedListing)
{
    static constexpr uint32_t FLAGS_DIR_OPEN = FsDirOpenMode_ReadDirs | FsDirOpenMode_ReadFiles;

    // This so directories can be reused.
    m_wasRead = false;
    if (!directoryPath.is_valid()) { return; }

    FsFileSystem *filesystem{};
    const std::string_view device = directoryPath.get_device_name();
    const char *path              = directoryPath.get_path();
    const bool deviceFound        = fslib::get_file_system_by_device_name(device, &filesystem);
    if (!deviceFound) { return; }

    const bool dirError   = error::occurred(fsFsOpenDirectory(filesystem, path, FLAGS_DIR_OPEN, &m_directoryHandle));
    const bool countError = !dirError && error::occurred(fsDirGetEntryCount(&m_directoryHandle, &m_entryCount));
    if (dirError || countError) { return; }

    // Reallocating this should free a previous array if there is one.
    m_directoryList = std::make_unique<FsDirectoryEntry[]>(m_entryCount);

    // This is how many entries the function says are read.
    int64_t totalEntries{};
    const bool readError = error::occurred(fsDirRead(&m_directoryHandle, &totalEntries, m_entryCount, m_directoryList.get()));
    const bool entriesMatch = totalEntries == m_entryCount;
    if (readError || !entriesMatch) { return; }

    if (sortedListing) { std::sort(m_directoryList.get(), m_directoryList.get() + m_entryCount, compare_entries); }
    Directory::close();
    m_wasRead = true;
}

bool fslib::Directory::is_open() const { return m_wasRead; }

int64_t fslib::Directory::get_count() const { return m_entryCount; }

int64_t fslib::Directory::get_entry_size(int index) const
{
    if (!Directory::index_check(index)) { return 0; }
    return m_directoryList[index].file_size;
}

const char *fslib::Directory::get_entry(int index) const
{
    if (!Directory::index_check(index)) { return nullptr; }
    return m_directoryList[index].name;
}

bool fslib::Directory::is_directory(int index) const
{
    if (!Directory::index_check(index)) { return false; }
    return m_directoryList[index].type == FsDirEntryType_Dir;
}

fslib::Directory::operator bool() const { return m_wasRead; }

const char *fslib::Directory::operator[](int index) const
{
    if (!Directory::index_check(index)) { return nullptr; }
    return m_directoryList[index].name;
}

bool fslib::Directory::index_check(int index) const { return index >= 0 && index < m_entryCount; }

void fslib::Directory::close() { fsDirClose(&m_directoryHandle); }

static bool compare_entries(const FsDirectoryEntry &entryA, const FsDirectoryEntry &entryB)
{
    if (entryA.type != entryB.type) { return entryA.type == FsDirEntryType_Dir; }

    size_t entryALength  = std::char_traits<char>::length(entryA.name);
    size_t entryBLength  = std::char_traits<char>::length(entryB.name);
    size_t shortestEntry = entryALength < entryBLength ? entryALength : entryBLength;
    for (size_t i = 0; i < shortestEntry;)
    {
        int charA = std::tolower(entryA.name[i]);
        int charB = std::tolower(entryB.name[i]);
        if (charA != charB) { return charA < charB; }
    }
    return false;
}
