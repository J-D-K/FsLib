#include "DirectoryEntry.hpp"

#include <string>

fslib::DirectoryEntry::DirectoryEntry(const FsDirectoryEntry &entry)
    : m_isDirectory(entry.type & FsDirEntryType_Dir)
    , m_filename(entry.name)
    , m_size(entry.file_size) {};

bool fslib::DirectoryEntry::is_directory() const { return m_isDirectory; }

const char *fslib::DirectoryEntry::get_filename() const { return m_filename.c_str(); }

const char *fslib::DirectoryEntry::get_extension() const
{
    size_t extensionBegin = m_filename.find_last_of('.');
    if (extensionBegin == m_filename.npos) { return nullptr; }

    // Note: This can still be dangerous. Need to figure this out better.
    return &m_filename.c_str()[extensionBegin + 1];
}

int64_t fslib::DirectoryEntry::get_size() const { return m_size; }
