#include "error.hpp"
#include "fslib.hpp"
#include "string.hpp"

#include <algorithm>
#include <cstring>
#include <string>

// Definition at bottom.
static bool compare_entries(const FS_DirectoryEntry &entryA, const FS_DirectoryEntry &entryB);

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

    uint32_t entriesRead{};
    FS_DirectoryEntry entry{};
    while (R_SUCCEEDED(FSDIR_Read(m_handle, &entriesRead, 1, &entry)) && entriesRead == 1) { m_list.push_back(entry); }
    Directory::close();

    if (sortEntries) { std::sort(m_list.begin(), m_list.end(), compare_entries); }

    m_wasOpened = true;
}

bool fslib::Directory::is_open() const { return m_wasOpened; }

size_t fslib::Directory::get_count() const { return m_list.size(); }

bool fslib::Directory::is_directory(int index) const
{
    const bool indexValid = Directory::index_check(index);
    if (!indexValid) { return false; }
    return m_list[index].attributes & FS_ATTRIBUTE_DIRECTORY;
}

const char16_t *fslib::Directory::get_entry(int index) const
{
    const bool indexValid = Directory::index_check(index);
    if (!indexValid) { return nullptr; }
    return reinterpret_cast<const char16_t *>(m_list[index].name);
}

const char16_t *fslib::Directory::operator[](int index) const { return reinterpret_cast<const char16_t *>(m_list[index].name); }

bool fslib::Directory::close()
{
    if (!m_wasOpened) { return false; }

    const bool closeError = error::libctru(FSDIR_Close(m_handle));
    if (closeError) { return false; }
    return true;
}

static bool compare_entries(const FS_DirectoryEntry &entryA, const FS_DirectoryEntry &entryB)
{
    {
        const uint32_t attributesA = entryA.attributes;
        const uint32_t attributesB = entryB.attributes;
        if (attributesA != attributesB) { return attributesA & FS_ATTRIBUTE_DIRECTORY; }
    }

    const int lengthA  = std::char_traits<uint16_t>::length(entryA.name);
    const int lengthB  = std::char_traits<uint16_t>::length(entryB.name);
    const int shortest = lengthA < lengthB ? lengthA : lengthB;
    for (int i = 0; i < shortest; i++)
    {
        const int charA = std::tolower(entryA.name[i]);
        const int charB = std::tolower(entryB.name[i]);
        if (charA != charB) { return charA < charB; }
    }
    return true;
}
