#include "FileSystem.hpp"

FileSystem::FileSystem(FsFileSystem &filesystem) noexcept
    : m_filesystem(filesystem) {};

FileSystem::FileSystem(FileSystem &&filesystem) noexcept
    : m_filesystem(filesystem.m_filesystem)
{
    filesystem.m_filesystem = {0};
}

FileSystem &FileSystem::operator=(FileSystem &&filesystem) noexcept
{
    m_filesystem = filesystem.m_filesystem;

    filesystem.m_filesystem = {0};

    return *this;
}

FileSystem::~FileSystem() noexcept { fsFsClose(&m_filesystem); }

FsFileSystem *FileSystem::get() noexcept { return &m_filesystem; }