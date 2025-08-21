#include "error.hpp"
#include "fslib.hpp"
#include "string.hpp"

#include <algorithm>
#include <cstring>
#include <string>

// Definition at bottom. Used to sort entries Dir->Alpha
static bool compare_entries(const fslib::DirectoryEntry &entryA, const fslib::DirectoryEntry &entryB);

fslib::Directory::Directory(const fslib::Path &directoryPath, bool sortEntries) { Directory::open(directoryPath, sortEntries); }

fslib::Directory::Directory(fslib::Directory &&directory) { *this = std::move(directory); }

fslib::Directory &fslib::Directory::operator=(fslib::Directory &&directory)
{
    m_handle    = directory.m_handle;
    m_wasOpened = directory.m_wasOpened;
    m_list      = std::move(directory.m_list);

    directory.m_handle    = 0;
    directory.m_wasOpened = false;
    directory.m_list.clear(); // Not sure if this is really needed, but w/e

    return *this;
}

void fslib::Directory::open(const fslib::Path &directoryPath, bool sortEntries)
{
    m_wasOpened = false;
    m_list.clear();

    FS_Archive archive{};
    const bool found = fslib::get_archive_by_device_name(directoryPath.get_device(), archive);
    if (!found) { return; }

    const bool openError = error::libctru(FSUSER_OpenDirectory(&m_handle, archive, directoryPath.get_fs_path()));
    if (openError) { return; }
    m_wasOpened = true;

    uint32_t entriesRead{};
    FS_DirectoryEntry entry{};
    while (R_SUCCEEDED(FSDIR_Read(m_handle, &entriesRead, 1, &entry)) && entriesRead == 1) { m_list.emplace_back(entry); }
    Directory::close();

    if (sortEntries) { std::sort(m_list.begin(), m_list.end(), compare_entries); }
}

bool fslib::Directory::is_open() const { return m_wasOpened; }

size_t fslib::Directory::get_count() const { return m_list.size(); }

const fslib::DirectoryEntry &fslib::Directory::get_entry(int index) const { return m_list[index]; }

const fslib::DirectoryEntry &fslib::Directory::operator[](int index) const { return m_list[index]; }

fslib::DirectoryIterator fslib::Directory::list() { return fslib::DirectoryIterator(this); }

bool fslib::Directory::close()
{
    if (!m_wasOpened) { return false; }

    const bool closeError = error::libctru(FSDIR_Close(m_handle));
    if (closeError) { return false; }

    return true;
}

static bool compare_entries(const fslib::DirectoryEntry &entryA, const fslib::DirectoryEntry &entryB)
{
    const bool aIsDir = entryA.is_directory();
    const bool bIsDir = entryB.is_directory();
    if (aIsDir != bIsDir) { return aIsDir; }

    const char16_t *nameA = entryA.get_filename();
    const char16_t *nameB = entryB.get_filename();
    const int lengthA     = std::char_traits<char16_t>::length(nameA);
    const int lengthB     = std::char_traits<char16_t>::length(nameB);
    const int shortest    = lengthA < lengthB ? lengthA : lengthB;
    for (int i = 0; i < shortest; i++)
    {
        const int charA = std::tolower(nameA[i]);
        const int charB = std::tolower(nameB[i]);
        if (charA != charB) { return charA < charB; }
    }
    return true;
}
