#include "DirectoryIterator.hpp"

fslib::DirectoryIterator::DirectoryIterator(fslib::Directory *directory) noexcept
    : m_directory(directory) {};

fslib::DirectoryIterator::iterator fslib::DirectoryIterator::begin() const noexcept
{
    return m_directory->m_directoryList.begin();
}

fslib::DirectoryIterator::iterator fslib::DirectoryIterator::end() const noexcept { return m_directory->m_directoryList.end(); }
