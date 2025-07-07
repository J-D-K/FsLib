#include "Path.hpp"
#include "error.hpp"
#include <cstring>
#include <string>
#include <switch.h>

namespace
{
    const char *FORBIDDEN_PATH_CHARACTERS = "<>:\"|?*";
} // namespace

fslib::Path::Path(const fslib::Path &path)
{
    *this = path;
}

fslib::Path::Path(Path &&path)
{
    *this = std::move(path);
}

fslib::Path::Path(const char *path)
{
    *this = path;
}

fslib::Path::Path(const std::string &path)
{
    *this = path;
}

fslib::Path::Path(std::string_view pathData)
{
    *this = pathData;
}

fslib::Path::Path(const std::filesystem::path &path)
{
    *this = path;
}

bool fslib::Path::is_valid() const noexcept
{
    bool valid = (m_path && m_deviceEnd && std::char_traits<char>::length(m_deviceEnd + 1) > 0 &&
                  std::strpbrk(m_deviceEnd + 1, FORBIDDEN_PATH_CHARACTERS) == NULL);
    if (!valid)
    {
        error::occurred(error::codes::INVALID_PATH);
    }
    return valid;
}

fslib::Path fslib::Path::sub_path(size_t pathLength) const
{
    if (pathLength > m_pathLength)
    {
        pathLength = m_pathLength;
    }

    fslib::Path newPath;
    newPath.m_path = std::make_unique<char[]>(m_pathSize);

    std::memcpy(newPath.m_path.get(), m_path.get(), pathLength);
    // Safety measure.
    newPath.m_path[pathLength] = '\0';
    newPath.m_pathSize = m_pathSize;
    newPath.m_deviceEnd = std::strchr(newPath.m_path.get(), ':');
    newPath.m_pathLength = std::char_traits<char>::length(newPath.m_path.get());

    // Not sure returning this empty on failure is the best idea, but it might be the only option.
    return newPath;
}

size_t fslib::Path::find_first_of(char character) const noexcept
{
    for (size_t i = 0; i < m_pathLength; i++)
    {
        if (m_path[i] == character)
        {
            return i;
        }
    }
    return Path::NOT_FOUND;
}

size_t fslib::Path::find_first_of(char character, size_t begin) const noexcept
{
    if (begin >= m_pathLength)
    {
        return Path::NOT_FOUND;
    }

    for (size_t i = begin; i < m_pathLength; i++)
    {
        if (m_path[i] == character)
        {
            return i;
        }
    }
    return Path::NOT_FOUND;
}

size_t fslib::Path::find_last_of(char character) const noexcept
{
    for (size_t i = m_pathLength; i > 0; i--)
    {
        if (m_path[i] == character)
        {
            return i;
        }
    }
    return Path::NOT_FOUND;
}

size_t fslib::Path::find_last_of(char character, size_t begin) const noexcept
{
    if (begin > m_pathLength)
    {
        begin = m_pathLength;
    }

    for (size_t i = begin; i > 0; i--)
    {
        if (m_path[i] == character)
        {
            return i;
        }
    }
    return Path::NOT_FOUND;
}

const char *fslib::Path::full_path() const noexcept
{
    return m_path.get();
}

std::string_view fslib::Path::get_device_name() const noexcept
{

    return std::string_view(m_path.get(), m_deviceEnd - m_path.get());
}

const char *fslib::Path::get_filename() const noexcept
{
    size_t lastSlash = Path::find_last_of('/');
    if (lastSlash == Path::NOT_FOUND)
    {
        // Just return empty.
        return {};
    }
    // This could be dangerous. To do: Fix that.
    return &m_path[lastSlash + 1];
}

const char *fslib::Path::get_path() const noexcept
{
    size_t deviceEnd = Path::find_first_of(':');
    if (deviceEnd == Path::NOT_FOUND)
    {
        return nullptr;
    }
    return &m_path[deviceEnd + 1];
}

const char *fslib::Path::get_extension() const noexcept
{
    size_t extensionBegin = Path::find_last_of('.');
    if (extensionBegin == Path::NOT_FOUND)
    {
        return nullptr;
    }
    return &m_path[extensionBegin + 1];
}

size_t fslib::Path::get_length() const noexcept
{
    return m_pathLength;
}

fslib::Path &fslib::Path::operator=(const fslib::Path &path)
{
    m_path = std::make_unique<char[]>(path.m_pathSize);
    if (!m_path)
    {
        return *this;
    }

    std::memcpy(m_path.get(), path.m_path.get(), path.m_pathSize);
    m_pathSize = path.m_pathSize;
    m_pathLength = path.m_pathLength;
    m_deviceEnd = std::strchr(m_path.get(), ':');

    // Safety first~
    m_path[m_pathLength] = '\0';

    return *this;
}

fslib::Path &fslib::Path::operator=(fslib::Path &&path) noexcept
{
    m_path = std::move(path.m_path);
    m_deviceEnd = path.m_deviceEnd;
    m_pathSize = path.m_pathSize;
    m_pathLength = path.m_pathLength;

    path.m_path = nullptr; // This might already be done by std::move.
    path.m_deviceEnd = nullptr;
    path.m_pathSize = 0;
    path.m_pathLength = 0;

    return *this;
}

fslib::Path &fslib::Path::operator=(const char *path)
{
    return *this = std::string_view(path);
}

fslib::Path &fslib::Path::operator=(const std::string &path)
{
    return *this = std::string_view(path);
}

fslib::Path &fslib::Path::operator=(std::string_view path)
{
    size_t deviceEnd = path.find_first_of(':');
    if (deviceEnd == path.npos)
    {
        return *this;
    }

    // This *should* be good enough.
    m_pathSize = FS_MAX_PATH + deviceEnd + 1;
    m_path = std::make_unique<char[]>(m_pathSize); // Error checking this might actually be pointless on Switch.
    // Locate where the first slash is after the device.
    size_t pathBegin = path.find_first_not_of('/', deviceEnd + 1);
    size_t pathEnd = path.find_last_not_of('/');

    // Copy the device over.
    std::memcpy(m_path.get(), path.data(), deviceEnd + 2);
    if (pathBegin != path.npos)
    {
        // Copy the rest starting where the actual path begins.
        std::memcpy(&m_path[std::char_traits<char>::length(m_path.get())],
                    path.substr(pathBegin, pathEnd - pathBegin).data(),
                    (pathEnd - pathBegin) + 1);
    }

    // This makes some other things easier and simpler to do later.
    m_deviceEnd = std::strchr(m_path.get(), ':');

    m_pathLength = std::char_traits<char>::length(m_path.get());

    m_path[m_pathLength] = '\0';

    return *this;
}

fslib::Path &fslib::Path::operator=(const std::filesystem::path &path)
{
    return *this = std::string_view(path.string());
}

fslib::Path &fslib::Path::operator/=(const char *path) noexcept
{
    return *this /= std::string_view(path);
}

fslib::Path &fslib::Path::operator/=(const std::string &path) noexcept
{
    return *this /= std::string_view(path);
}

fslib::Path &fslib::Path::operator/=(std::string_view path) noexcept
{
    size_t pathBegin = path.find_first_not_of('/');
    size_t pathEnd = path.find_last_not_of('/');
    if (pathBegin == path.npos || pathEnd == path.npos)
    {
        return *this;
    }

    // This makes things easier.
    size_t length = (pathEnd - pathBegin) + 1;
    if (length + 1 >= m_pathSize)
    {
        return *this;
    }

    // Needed to avoid doubling up slashes directly appending to a device root.
    if (m_path[m_pathLength - 1] != '/')
    {
        m_path[m_pathLength++] = '/';
    }

    // This looks really dangerous for some reason. I like it though.
    std::memcpy(&m_path[m_pathLength], &path.data()[pathBegin], length);

    m_pathLength += length;

    m_path[m_pathLength] = '\0';

    return *this;
}

fslib::Path &fslib::Path::operator/=(const std::filesystem::path &path) noexcept
{
    return *this /= std::string_view(path.string());
}

fslib::Path &fslib::Path::operator+=(const char *path) noexcept
{
    return *this += std::string_view(path);
}

fslib::Path &fslib::Path::operator+=(const std::string &path) noexcept
{
    return *this += std::string_view(path);
}

fslib::Path &fslib::Path::operator+=(std::string_view path) noexcept
{
    size_t length = std::char_traits<char>::length(path.data());
    if (m_pathLength + length >= m_pathSize)
    {
        return *this;
    }

    std::memcpy(&m_path[m_pathLength], path.data(), length);

    m_pathLength += length;
    m_path[m_pathLength] = '\0';

    return *this;
}

fslib::Path &fslib::Path::operator+=(const std::filesystem::path &path) noexcept
{
    return *this += std::string_view(path.string());
}

fslib::Path fslib::operator/(const fslib::Path &pathA, const char *pathB)
{
    fslib::Path newPath = pathA;
    newPath /= pathB;
    return newPath;
}

fslib::Path fslib::operator/(const fslib::Path &pathA, const std::string &pathB)
{
    fslib::Path newPath = pathA;
    newPath /= pathB;
    return newPath;
}

fslib::Path fslib::operator/(const fslib::Path &pathA, std::string_view pathB)
{
    fslib::Path newPath = pathA;
    newPath /= pathB;
    return newPath;
}

fslib::Path fslib::operator/(const fslib::Path &pathA, const std::filesystem::path &pathB)
{
    fslib::Path newPath = pathA;
    newPath /= pathB;
    return newPath;
}

fslib::Path fslib::operator+(const fslib::Path &pathA, const char *pathB)
{
    fslib::Path newPath = pathA;
    newPath += pathB;
    return newPath;
}

fslib::Path fslib::operator+(const fslib::Path &pathA, const std::string &pathB)
{
    fslib::Path newPath = pathA;
    newPath += pathB;
    return newPath;
}

fslib::Path fslib::operator+(const fslib::Path &pathA, std::string_view pathB)
{
    fslib::Path newPath = pathA;
    newPath += pathB;
    return newPath;
}

fslib::Path fslib::operator+(const fslib::Path &pathA, const std::filesystem::path &pathB)
{
    fslib::Path newPath = pathA;
    newPath += pathB;
    return newPath;
}
