#include "DirectoryEntry.hpp"

#include <cstring>
#include <string>

fslib::DirectoryEntry::DirectoryEntry(const FsDirectoryEntry &entry)
    : m_directory(entry.type == FsDirEntryType_Dir)
    , m_filename(entry.name)
    , m_size(entry.file_size) {};

fslib::DirectoryEntry::DirectoryEntry(DirectoryEntry &&entry) noexcept
    : m_directory(entry.m_directory)
    , m_filename(std::move(entry.m_filename))
    , m_size(entry.m_size)

{
    entry.m_directory = false;
    entry.m_size      = 0;
}

fslib::DirectoryEntry &fslib::DirectoryEntry::operator=(DirectoryEntry &&entry) noexcept
{
    m_directory = entry.m_directory;
    m_filename  = std::move(entry.m_filename);
    m_size      = entry.m_size;

    entry.m_directory = false;
    entry.m_size      = 0;

    return *this;
}

bool fslib::DirectoryEntry::is_directory() const noexcept { return m_directory; }

const char *fslib::DirectoryEntry::get_filename() const noexcept { return m_filename.c_str(); }

const char *fslib::DirectoryEntry::get_extension() const noexcept
{
    const size_t lastDot = m_filename.find_last_of('.');
    if (lastDot == m_filename.npos) { return nullptr; }

    return &m_filename.data()[lastDot];
}

int64_t fslib::DirectoryEntry::get_size() const noexcept { return m_size; }
