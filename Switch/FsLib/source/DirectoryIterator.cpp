#include "DirectoryIterator.hpp"

fslib::DirectoryIterator::DirectoryIterator(fslib::Directory *directory)
    : m_directory(directory) {};

fslib::DirectoryIterator::iterator fslib::DirectoryIterator::begin() const { return m_directory->m_directoryList.begin(); }

fslib::DirectoryIterator::iterator fslib::DirectoryIterator::end() const { return m_directory->m_directoryList.end(); }

fslib::DirectoryEntry &fslib::DirectoryIterator::operator&() const { return m_directory->m_directoryList[m_index]; }

fslib::DirectoryEntry *fslib::DirectoryIterator::operator*() const { return &m_directory->m_directoryList[m_index]; }

fslib::DirectoryIterator &fslib::DirectoryIterator::operator++()
{
    m_index++;
    return *this;
}

bool fslib::DirectoryIterator::operator!=(const fslib::DirectoryIterator &iter) const { return iter.m_index != m_index; }
