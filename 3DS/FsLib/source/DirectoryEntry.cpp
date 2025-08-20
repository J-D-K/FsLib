#include "DirectoryEntry.hpp"

fslib::DirectoryEntry::DirectoryEntry(const FS_DirectoryEntry &entry)
    : m_isDirectory(entry.attributes & FS_ATTRIBUTE_DIRECTORY)
    , m_filename(reinterpret_cast<const char16_t *>(entry.name)) {};

bool fslib::DirectoryEntry::is_directory() const { return m_isDirectory; }

const char16_t *fslib::DirectoryEntry::get_filename() const { return m_filename.c_str(); }

const char16_t *fslib::DirectoryEntry::get_extension() const
{
    size_t extensionBegin = m_filename.find_last_of(u'.');
    if (extensionBegin == m_filename.npos) { return nullptr; }

    // NOTE: This is dangerous and I shouldn't be doing it!
    return &m_filename.c_str()[extensionBegin + 1];
}
