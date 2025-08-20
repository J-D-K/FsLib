#include "Path.hpp"

#include "error.hpp"

#include <cstring>
#include <string>
#include <switch.h>

namespace
{
    constexpr const char *FORBIDDEN_PATH_CHARACTERS = "<>:\"|?*";
} // namespace

fslib::Path::Path(const fslib::Path &path) { *this = path; }

fslib::Path::Path(Path &&path) { *this = std::move(path); }

fslib::Path::Path(const char *path) { *this = path; }

fslib::Path::Path(const std::string &path) { *this = path; }

fslib::Path::Path(std::string_view pathData) { *this = pathData; }

fslib::Path::Path(const std::filesystem::path &path) { *this = path; }

bool fslib::Path::is_valid() const
{
    const bool validPointers     = m_path && m_deviceEnd;
    const bool validLength       = std::char_traits<char>::length(m_deviceEnd + 1) > 0;
    const bool containsForbidden = std::strpbrk(m_deviceEnd + 1, FORBIDDEN_PATH_CHARACTERS) != NULL;
    if (!validPointers || !validLength || containsForbidden)
    {
        error::occurred(error::codes::INVALID_PATH);
        return false;
    }
    return true;
}

fslib::Path fslib::Path::sub_path(size_t pathLength) const
{
    if (pathLength > m_pathLength) { pathLength = m_pathLength; }

    fslib::Path newPath;
    newPath.m_path = std::make_unique<char[]>(m_pathSize);

    std::memcpy(newPath.m_path.get(), m_path.get(), pathLength);
    newPath.m_pathSize   = m_pathSize;
    newPath.m_pathLength = std::char_traits<char>::length(newPath.m_path.get());
    newPath.m_deviceEnd  = std::char_traits<char>::find(newPath.m_path.get(), newPath.m_pathLength, ':');
    // Safety because sometimes it seems like the memset doesn't always work?
    newPath.m_path[pathLength] = '\0';

    return newPath;
}

size_t fslib::Path::find_first_of(char character) const
{
    for (size_t i = 0; i < m_pathLength; i++)
    {
        if (m_path[i] == character) { return i; }
    }
    return Path::NOT_FOUND;
}

size_t fslib::Path::find_first_of(char character, size_t begin) const
{
    if (begin >= m_pathLength) { return Path::NOT_FOUND; }

    for (size_t i = begin; i < m_pathLength; i++)
    {
        if (m_path[i] == character) { return i; }
    }
    return Path::NOT_FOUND;
}

size_t fslib::Path::find_last_of(char character) const
{
    for (size_t i = m_pathLength; i > 0; i--)
    {
        if (m_path[i] == character) { return i; }
    }
    return Path::NOT_FOUND;
}

size_t fslib::Path::find_last_of(char character, size_t begin) const
{
    if (begin > m_pathLength) { begin = m_pathLength; }

    for (size_t i = begin; i > 0; i--)
    {
        if (m_path[i] == character) { return i; }
    }
    return Path::NOT_FOUND;
}

const char *fslib::Path::full_path() const { return m_path.get(); }

std::string_view fslib::Path::get_device_name() const
{
    const int deviceLength = m_deviceEnd - m_path.get();
    return std::string_view(m_path.get(), deviceLength);
}

const char *fslib::Path::get_filename() const
{
    size_t lastSlash = Path::find_last_of('/');
    if (lastSlash == Path::NOT_FOUND) { return nullptr; }
    // This could be dangerous. To do: Fix that.
    return &m_path[lastSlash + 1];
}

const char *fslib::Path::get_path() const
{
    size_t deviceEnd = Path::find_first_of(':');
    if (deviceEnd == Path::NOT_FOUND) { return nullptr; }
    return &m_path[deviceEnd + 1];
}

const char *fslib::Path::get_extension() const
{
    size_t extensionBegin = Path::find_last_of('.');
    if (extensionBegin == Path::NOT_FOUND) { return nullptr; }
    return &m_path[extensionBegin + 1];
}

size_t fslib::Path::get_length() const { return m_pathLength; }

fslib::Path &fslib::Path::operator=(const fslib::Path &path)
{
    m_path = std::make_unique<char[]>(path.m_pathSize);
    if (!m_path) { return *this; }

    std::memcpy(m_path.get(), path.m_path.get(), path.m_pathSize);
    m_pathSize           = path.m_pathSize;
    m_pathLength         = path.m_pathLength;
    m_deviceEnd          = std::strchr(m_path.get(), ':');
    m_path[m_pathLength] = '\0';

    return *this;
}

fslib::Path &fslib::Path::operator=(fslib::Path &&path)
{
    m_path       = std::move(path.m_path);
    m_deviceEnd  = path.m_deviceEnd;
    m_pathSize   = path.m_pathSize;
    m_pathLength = path.m_pathLength;

    path.m_path       = nullptr; // This might already be done by std::move.
    path.m_deviceEnd  = nullptr;
    path.m_pathSize   = 0;
    path.m_pathLength = 0;

    return *this;
}

fslib::Path &fslib::Path::operator=(const char *path) { return *this = std::string_view(path); }

fslib::Path &fslib::Path::operator=(const std::string &path) { return *this = std::string_view(path); }

fslib::Path &fslib::Path::operator=(std::string_view path)
{
    const size_t deviceEnd = path.find_first_of(':');
    if (deviceEnd == path.npos) { return *this; }

    // This *should* be good enough.
    m_pathSize = FS_MAX_PATH + deviceEnd + 1;
    m_path     = std::make_unique<char[]>(m_pathSize); // Error checking this might actually be pointless on Switch.

    // Locate where the first slash is after the device.
    const size_t pathBegin = path.find_first_not_of('/', deviceEnd + 1);
    const size_t pathEnd   = path.find_last_not_of('/');

    // Copy the device over.
    std::memcpy(m_path.get(), path.data(), deviceEnd + 2);

    // To do: Revise from here down.
    if (pathBegin != path.npos)
    {
        // Readability.
        const size_t length          = std::char_traits<char>::length(m_path.get());
        const std::string_view slice = path.substr(pathBegin);
        const size_t copyLength      = (pathEnd - pathBegin) + 1;

        std::memcpy(&m_path[length], slice.data(), copyLength);
    }

    m_pathLength         = std::char_traits<char>::length(m_path.get());
    m_deviceEnd          = std::char_traits<char>::find(m_path.get(), m_pathLength, ':');
    m_path[m_pathLength] = '\0'; // Safety

    return *this;
}

fslib::Path &fslib::Path::operator=(const std::filesystem::path &path) { return *this = std::string_view(path.string()); }

fslib::Path &fslib::Path::operator/=(const char *path) { return *this /= std::string_view(path); }

fslib::Path &fslib::Path::operator/=(const std::string &path) { return *this /= std::string_view(path); }

fslib::Path &fslib::Path::operator/=(std::string_view path)
{
    const size_t pathBegin = path.find_first_not_of('/');
    const size_t pathEnd   = path.find_last_not_of('/');
    if (pathBegin == path.npos || pathEnd == path.npos) { return *this; }

    // This makes things easier.
    const size_t length = (pathEnd - pathBegin) + 1;
    if (length + 1 >= m_pathSize) { return *this; }

    // Needed to avoid doubling up slashes directly appending to a device root.
    if (m_path[m_pathLength - 1] != '/') { m_path[m_pathLength++] = '/'; }

    // This looks really dangerous for some reason. I like it though.
    const std::string_view slice = path.substr(pathBegin);
    std::memcpy(&m_path[m_pathLength], slice.data(), length);

    m_pathLength += length;

    m_path[m_pathLength] = '\0';

    return *this;
}

fslib::Path &fslib::Path::operator/=(const std::filesystem::path &path) { return *this /= std::string_view(path.string()); }

fslib::Path &fslib::Path::operator/=(const fslib::DirectoryEntry &path)
{
    return *this /= std::string_view(path.get_filename());
}

fslib::Path &fslib::Path::operator+=(const char *path) { return *this += std::string_view(path); }

fslib::Path &fslib::Path::operator+=(const std::string &path) { return *this += std::string_view(path); }

fslib::Path &fslib::Path::operator+=(std::string_view path)
{
    const size_t length = path.length();
    if (m_pathLength + length >= m_pathSize) { return *this; }

    std::memcpy(&m_path[m_pathLength], path.data(), length);

    m_pathLength += length;
    m_path[m_pathLength] = '\0';

    return *this;
}

fslib::Path &fslib::Path::operator+=(const std::filesystem::path &path) { return *this += std::string_view(path.string()); }

fslib::Path &fslib::Path::operator+=(const fslib::DirectoryEntry &path)
{
    return *this += std::string_view(path.get_filename());
}

fslib::Path fslib::operator/(const fslib::Path &pathA, const char *pathB)
{
    fslib::Path newPath{pathA};
    newPath /= pathB;
    return newPath;
}

fslib::Path fslib::operator/(const fslib::Path &pathA, const std::string &pathB)
{
    fslib::Path newPath{pathA};
    newPath /= pathB;
    return newPath;
}

fslib::Path fslib::operator/(const fslib::Path &pathA, std::string_view pathB)
{
    fslib::Path newPath{pathA};
    newPath /= pathB;
    return newPath;
}

fslib::Path fslib::operator/(const fslib::Path &pathA, const std::filesystem::path &pathB)
{
    fslib::Path newPath{pathA};
    newPath /= pathB;
    return newPath;
}

fslib::Path fslib::operator/(const fslib::Path &pathA, const fslib::DirectoryEntry &pathB)
{
    fslib::Path newPath{pathA};
    newPath /= pathB;
    return newPath;
}

fslib::Path fslib::operator+(const fslib::Path &pathA, const char *pathB)
{
    fslib::Path newPath{pathA};
    newPath += pathB;
    return newPath;
}

fslib::Path fslib::operator+(const fslib::Path &pathA, const std::string &pathB)
{
    fslib::Path newPath{pathA};
    newPath += pathB;
    return newPath;
}

fslib::Path fslib::operator+(const fslib::Path &pathA, std::string_view pathB)
{
    fslib::Path newPath{pathA};
    newPath += pathB;
    return newPath;
}

fslib::Path fslib::operator+(const fslib::Path &pathA, const std::filesystem::path &pathB)
{
    fslib::Path newPath{pathA};
    newPath += pathB;
    return newPath;
}

fslib::Path fslib::operator+(const fslib::Path &pathA, const fslib::DirectoryEntry &pathB)
{
    fslib::Path newPath{pathA};
    newPath += pathB;
    return newPath;
}
