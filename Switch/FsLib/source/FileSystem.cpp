#include "FileSystem.hpp"

FileSystem::FileSystem(FsFileSystem &filesystem) noexcept
    : m_filesystem(filesystem) {};

FileSystem::~FileSystem() noexcept { fsFsClose(&m_filesystem); }

FsFileSystem *FileSystem::get() noexcept { return &m_filesystem; }