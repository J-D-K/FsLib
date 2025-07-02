#include "fslib.hpp"
#include "string.hpp"
#include <algorithm>
#include <cstring>
#include <string>

extern std::string g_fslibErrorString;

// This sorts the entries Directories->File, then pseudo-alphabetically.
static bool compareEntries(const FS_DirectoryEntry &entryA, const FS_DirectoryEntry &entryB)
{
    if (entryA.attributes != entryB.attributes)
    {
        return entryA.attributes & FS_ATTRIBUTE_DIRECTORY;
    }

    // Get the shortest length to avoid reading out-of-bounds.
    size_t entryALength = std::char_traits<uint16_t>::length(entryA.name);
    size_t entryBLength = std::char_traits<uint16_t>::length(entryB.name);
    size_t shortestEntry = entryALength < entryBLength ? entryALength : entryBLength;
    for (size_t i = 0; i < shortestEntry; i++)
    {
        // Lower case the character so that doesn't impact sorting.
        int charA = std::tolower(entryA.name[i]);
        int charB = std::tolower(entryB.name[i]);
        if (charA != charB)
        {
            return charA < charB;
        }
    }
    return false;
}

fslib::Directory::Directory(const fslib::Path &directoryPath, bool sortEntries)
{
    Directory::open(directoryPath, sortEntries);
}

void fslib::Directory::open(const fslib::Path &directoryPath, bool sortEntries)
{
    // Just in case directory is reused.
    m_wasOpened = false;
    m_directoryList.clear();

    FS_Archive archive;
    if (!fslib::processDeviceAndPath(directoryPath, &archive))
    {
        // Function will set error string.
        return;
    }

    Result fsError = FSUSER_OpenDirectory(&m_directoryHandle, archive, directoryPath.getPath());
    if (R_FAILED(fsError))
    {
        g_fslibErrorString = string::getFormattedString("Error opening directory: 0x%08X.", fsError);
        return;
    }

    // Switch has a function to fetch entry count. 3DS doesn't, so we have to loop and load one at a time.
    uint32_t entriesRead = 0;
    FS_DirectoryEntry currentEntry;
    while (R_SUCCEEDED(FSDIR_Read(m_directoryHandle, &entriesRead, 1, &currentEntry)) && entriesRead == 1)
    {
        m_directoryList.push_back(currentEntry);
    }
    // The directory was read and there's no reason to keep it open.
    Directory::close();

    // Sort entries if requested.
    if (sortEntries)
    {
        std::sort(m_directoryList.begin(), m_directoryList.end(), compareEntries);
    }

    // Should be good to go?
    m_wasOpened = true;
}

bool fslib::Directory::isOpen() const
{
    return m_wasOpened;
}

uint32_t fslib::Directory::getCount() const
{
    return m_directoryList.size();
}

bool fslib::Directory::isDirectory(int index) const
{
    if (index < 0 || index >= static_cast<int>(m_directoryList.size()))
    {
        return false;
    }
    return m_directoryList.at(index).attributes & FS_ATTRIBUTE_DIRECTORY;
}

std::u16string_view fslib::Directory::getEntry(int index) const
{
    if (index < 0 || index >= static_cast<int>(m_directoryList.size()))
    {
        return std::u16string_view(u"nullptr");
    }
    return std::u16string_view(reinterpret_cast<const char16_t *>(m_directoryList.at(index).name));
}

const char16_t *fslib::Directory::operator[](int index) const
{
    if (index < 0 || index >= static_cast<int>(m_directoryList.size()))
    {
        return nullptr;
    }
    return reinterpret_cast<const char16_t *>(m_directoryList.at(index).name);
}

bool fslib::Directory::close()
{
    if (!m_wasOpened)
    {
        return false;
    }

    Result fsError = FSDIR_Close(m_directoryHandle);
    if (R_FAILED(fsError))
    {
        g_fslibErrorString = string::getFormattedString("Error closing directory: 0x%08X.", fsError);
        return false;
    }
    return true;
}
