#include "Directory.hpp"

#include "error.hpp"
#include "fslib.hpp"

#include <algorithm>
#include <cstring>
#include <string>

/// @brief Function used to sort by directories->alphabetically.
static bool compare_entries(const fslib::DirectoryEntry &entryA, const fslib::DirectoryEntry &entryB);

fslib::Directory::Directory(const fslib::Path &directoryPath, bool sortedListing)
{
    Directory::open(directoryPath, sortedListing);
}

fslib::Directory::Directory(Directory &&directory) { *this = std::move(directory); }

fslib::Directory &fslib::Directory::operator=(Directory &&directory)
{
    // Start by copying this to make sure we have EVERYTHING~
    m_handle        = directory.m_handle;
    m_directoryList = std::move(directory.m_directoryList);
    m_entryCount    = directory.m_entryCount;
    m_wasRead       = directory.m_wasRead;

    directory.m_handle = {0};
    m_directoryList.clear(); // Not really sure if this is needed after std::move, but jic.
    m_entryCount = 0;
    m_wasRead    = 0;

    return *this;
}

void fslib::Directory::open(const fslib::Path &directoryPath, bool sortedListing)
{
    static constexpr uint32_t FLAGS_DIR_OPEN = FsDirOpenMode_ReadDirs | FsDirOpenMode_ReadFiles;

    // Oops. Need this too!
    m_directoryList.clear();

    // This so directories can be reused.
    m_wasRead = false;
    if (!directoryPath.is_valid()) { return; }

    FsFileSystem *filesystem{};
    const std::string_view device = directoryPath.get_device_name();
    const char *path              = directoryPath.get_path();
    const bool deviceFound        = fslib::get_file_system_by_device_name(device, &filesystem);
    if (!deviceFound) { return; }

    const bool dirError   = error::occurred(fsFsOpenDirectory(filesystem, path, FLAGS_DIR_OPEN, &m_handle));
    const bool countError = !dirError && error::occurred(fsDirGetEntryCount(&m_handle, &m_entryCount));
    if (dirError || countError) { return; }

    m_directoryList.reserve(m_entryCount);
    auto entryBuffer = std::make_unique<FsDirectoryEntry[]>(m_entryCount);

    // This is how many entries the function says are read.
    int64_t totalEntries{};
    const bool readError    = error::occurred(fsDirRead(&m_handle, &totalEntries, m_entryCount, entryBuffer.get()));
    const bool entriesMatch = totalEntries == m_entryCount;
    if (readError || !entriesMatch) { return; }

    for (int64_t i = 0; i < m_entryCount; i++) { m_directoryList.emplace_back(entryBuffer[i]); }

    if (sortedListing) { std::sort(m_directoryList.begin(), m_directoryList.end(), compare_entries); }
    Directory::close();
    m_wasRead = true;
}

bool fslib::Directory::is_open() const noexcept { return m_wasRead; }

int64_t fslib::Directory::get_count() const noexcept { return m_entryCount; }

const fslib::DirectoryEntry &fslib::Directory::get_entry(int index) const { return m_directoryList[index]; }

const fslib::DirectoryEntry &fslib::Directory::operator[](int index) const { return m_directoryList[index]; }

fslib::Directory::iterator fslib::Directory::begin() const noexcept { return m_directoryList.begin(); }

fslib::Directory::iterator fslib::Directory::end() const noexcept { return m_directoryList.end(); }

void fslib::Directory::close() { fsDirClose(&m_handle); }

static bool compare_entries(const fslib::DirectoryEntry &entryA, const fslib::DirectoryEntry &entryB)
{
    const bool isDirA = entryA.is_directory();
    const bool isDirB = entryB.is_directory();
    if (isDirA != isDirB) { return isDirA; }

    const char *nameA          = entryA.get_filename();
    const char *nameB          = entryB.get_filename();
    const size_t entryALength  = std::char_traits<char>::length(nameA);
    const size_t entryBLength  = std::char_traits<char>::length(nameB);
    const size_t shortestEntry = entryALength < entryBLength ? entryALength : entryBLength;
    for (size_t i = 0; i < shortestEntry;)
    {
        const int charA = std::tolower(nameA[i]);
        const int charB = std::tolower(nameB[i]);
        if (charA != charB) { return charA < charB; }
    }
    return false;
}
