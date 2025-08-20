#include "DirectoryIterator.hpp"

fslib::DirectoryIterator::DirectoryIterator(const fslib::Path &path)
    : m_directory(path) {};

FsDirectoryEntry *fslib::DirectoryIterator::begin() const { return &m_directory.m_directoryList[0]; }

FsDirectoryEntry *fslib::DirectoryIterator::end() const { return &m_directory.m_directoryList[m_directory.m_entryCount]; }

FsDirectoryEntry &fslib::DirectoryIterator::operator*() { return m_directory.m_directoryList[m_index]; }

FsDirectoryEntry *fslib::DirectoryIterator::operator->() { return &m_directory.m_directoryList[m_index]; }

fslib::DirectoryIterator &fslib::DirectoryIterator::operator++()
{
    m_index++;
    return *this;
}

bool fslib::DirectoryIterator::operator!=(const fslib::DirectoryIterator &iter) { return m_index != iter.m_index; }
