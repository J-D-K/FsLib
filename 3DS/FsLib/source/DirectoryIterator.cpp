#include "DirectoryIterator.hpp"

fslib::DirectoryIterator::DirectoryIterator(const fslib::Path &path)
    : m_directory(path) {};

const FS_DirectoryEntry *fslib::DirectoryIterator::begin() const { return &m_directory.m_list[0]; }

const FS_DirectoryEntry *fslib::DirectoryIterator::end() const
{
    const size_t listSize = m_directory.m_list.size();
    return &m_directory.m_list[listSize];
}

FS_DirectoryEntry &fslib::DirectoryIterator::operator*() { return m_directory.m_list[m_index]; }

FS_DirectoryEntry *fslib::DirectoryIterator::operator->() { return &m_directory.m_list[m_index]; }

fslib::DirectoryIterator &fslib::DirectoryIterator::operator++()
{
    m_index++;
    return *this;
}

bool fslib::DirectoryIterator::operator!=(const fslib::DirectoryIterator &iter) const { return iter.m_index != m_index; }
