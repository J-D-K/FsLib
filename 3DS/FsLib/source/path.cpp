#include "Path.hpp"
#include "string.hpp"
#include <algorithm>
#include <array>
#include <cstring>

namespace
{
    // Array of characters that are forbidden in paths.
    constexpr std::array<char16_t, 8> FORBIDDEN_CHARS = {u'<', u'>', u':', u'\\', u'"', u'|', u'?', u'*'};
} // namespace

// Apparently C++ has no way of doing this in char_traits.
// C standard library has some pretty hard to decipher names.
const char16_t *strpbrk16(const char16_t *string)
{
    while (*string)
    {
        if (std::find(FORBIDDEN_CHARS.begin(), FORBIDDEN_CHARS.end(), *string) != FORBIDDEN_CHARS.end())
        {
            return string;
        }
        ++string;
    }
    return nullptr;
}

// This is an all in one version of what Switch does. Passing nullptr as PathBegin skips trimming the beginning.
void getTrimmedPath(const char16_t *path, const char16_t **pathBegin, size_t &pathLengthOut)
{
    // This will trim extra beginning slashes.
    if (pathBegin)
    {
        while (*path == '/')
        {
            ++path;
        }
        *pathBegin = path;
    }

    for (pathLengthOut = std::char_traits<char16_t>::length(path); pathLengthOut > 0; pathLengthOut--)
    {
        if (path[pathLengthOut - 1] != u'/')
        {
            break;
        }
    }
}

fslib::Path::Path(const fslib::Path &path)
{
    *this = path;
}

fslib::Path::Path(const char16_t *pathData)
{
    *this = pathData;
}

fslib::Path::Path(const uint16_t *pathData)
{
    *this = pathData;
}

fslib::Path::Path(const std::u16string &pathData)
{
    *this = pathData;
}

fslib::Path::Path(const std::u16string_view pathData)
{
    *this = pathData;
}

fslib::Path::~Path()
{
    Path::freePath();
}

bool fslib::Path::isValid(void) const
{
    return m_path && m_deviceEnd && std::char_traits<char16_t>::length(m_deviceEnd + 1) > 0 && strpbrk16(m_deviceEnd + 1) == nullptr;
}

fslib::Path fslib::Path::subPath(size_t pathLength) const
{
    if (pathLength > m_pathLength)
    {
        pathLength = m_pathLength;
    }

    fslib::Path newPath;
    if (newPath.allocatePath(m_pathSize))
    {
        std::memcpy(newPath.m_path, m_path, pathLength * sizeof(char16_t));
        newPath.m_deviceEnd = std::char_traits<char16_t>::find(newPath.m_path, m_pathLength, u':');
        newPath.m_pathLength = m_pathLength;
    }
    return newPath;
}

size_t fslib::Path::findFirstOf(char16_t character) const
{
    for (size_t i = 0; i < m_pathLength; i++)
    {
        if (m_path[i] == character)
        {
            return i;
        }
    }
    return Path::notFound;
}

size_t fslib::Path::findFirstOf(char16_t character, size_t begin) const
{
    for (size_t i = begin; i < m_pathLength; i++)
    {
        if (m_path[i] == character)
        {
            return i;
        }
    }
    return Path::notFound;
}

size_t fslib::Path::findLastOf(char16_t character) const
{
    for (size_t i = m_pathLength; i > 0; i--)
    {
        if (m_path[i] == character)
        {
            return i;
        }
    }
    return Path::notFound;
}

size_t fslib::Path::findLastOf(char16_t character, size_t begin) const
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
    return Path::notFound;
}

const char16_t *fslib::Path::cString(void) const
{
    return m_path;
}

std::u16string_view fslib::Path::getDevice(void) const
{
    return std::u16string_view(m_path, m_deviceEnd - m_path);
}

std::u16string_view fslib::Path::getFileName(void) const
{
    size_t fileNameBegin = Path::findLastOf(u'/');
    size_t fileNameEnd = Path::findFirstOf(u'.');
    if (fileNameBegin == Path::notFound || fileNameEnd == Path::notFound)
    {
        // I'm hoping this is just empty and doesn't cause more errors.
        return std::u16string_view(u"nullptr");
    }
    ++fileNameBegin;
    --fileNameEnd;
    return std::u16string_view(&m_path[fileNameBegin], fileNameEnd - fileNameBegin);
}

std::u16string_view fslib::Path::getExtension(void) const
{
    size_t extensionBegin = Path::findLastOf(u'.');
    if (extensionBegin == Path::notFound)
    {
        return std::u16string_view(u"nullptr");
    }
    // I'm assuming the last dot will start the extension.
    return std::u16string_view(&m_path[extensionBegin] + 1, m_pathLength);
}

FS_Path fslib::Path::getPath(void) const
{
    return {PATH_UTF16, (std::char_traits<char16_t>::length(m_deviceEnd + 1) * sizeof(char16_t)) + sizeof(char16_t), m_deviceEnd + 1};
}

size_t fslib::Path::getLength(void) const
{
    return m_pathLength;
}

fslib::Path &fslib::Path::operator=(const fslib::Path &path)
{
    if (!Path::allocatePath(path.m_pathSize))
    {
        // To do: Better error handling than this.
        return *this;
    }

    // Copy path data from incoming path.
    std::memcpy(m_path, path.m_path, path.m_pathSize * sizeof(char16_t));
    m_pathSize = path.m_pathSize;
    m_pathLength = path.m_pathLength;
    m_deviceEnd = std::char_traits<char16_t>::find(m_path, m_pathLength, u':');

    return *this;
}

fslib::Path &fslib::Path::operator=(const char16_t *pathData)
{
    // Calculate how much memory to allocate for path.
    size_t pathLength = std::char_traits<char16_t>::length(pathData);
    m_deviceEnd = std::char_traits<char16_t>::find(pathData, pathLength, u':');
    if (!m_deviceEnd)
    {
        // To do: things
        return *this;
    }

    m_pathSize = fslib::MAX_PATH + ((m_deviceEnd - pathData) + 1);
    if (!Path::allocatePath(m_pathSize))
    {
        return *this;
    }

    // Need to get trimmed, proper path. Training wheels, I guess.
    const char16_t *pathBegin = nullptr;
    getTrimmedPath(m_deviceEnd + 1, &pathBegin, pathLength);

    // Copy device over.
    std::memcpy(m_path, pathData, ((m_deviceEnd - pathData) + 2) * sizeof(char16_t));
    // Copy trimmed path over starting after the device string.
    std::memcpy(&m_path[std::char_traits<char16_t>::length(m_path)], pathBegin, pathLength * sizeof(char16_t));
    // Make sure these are correct and point to this instance's members.
    m_pathLength = std::char_traits<char16_t>::length(m_path);
    m_deviceEnd = std::char_traits<char16_t>::find(m_path, m_pathLength, ':');

    return *this;
}

fslib::Path &fslib::Path::operator=(const uint16_t *pathData)
{
    return *this = reinterpret_cast<const char16_t *>(pathData);
}

fslib::Path &fslib::Path::operator=(const std::u16string &pathData)
{
    return *this = pathData.c_str();
}

fslib::Path &fslib::Path::operator=(std::u16string_view pathData)
{
    // To do: Properly support string_view.
    return *this = pathData.data();
}

fslib::Path &fslib::Path::operator/=(const char16_t *pathData)
{
    size_t pathLength = 0;
    const char16_t *pathBegin = nullptr;
    getTrimmedPath(pathData, &pathBegin, pathLength);

    // Check to make sure buffer has enough space.
    if ((m_pathLength + pathLength) + 1 >= m_pathSize)
    {
        // To do: The thing.
        return *this;
    }

    // This is to avoid doubling up slashes after the device.
    if (m_path[m_pathLength - 1] != u'/')
    {
        m_path[m_pathLength++] = u'/';
    }

    // Copy trimmed path to the end of our current path.
    std::memcpy(&m_path[m_pathLength], pathBegin, pathLength * sizeof(char16_t));

    m_pathLength += pathLength;

    return *this;
}

fslib::Path &fslib::Path::operator/=(const uint16_t *pathData)
{
    return *this /= reinterpret_cast<const char16_t *>(pathData);
}

fslib::Path &fslib::Path::operator/=(const std::u16string &pathData)
{
    return *this /= pathData.c_str();
}

fslib::Path &fslib::Path::operator/=(std::u16string_view pathData)
{
    return *this /= pathData.data();
}

fslib::Path &fslib::Path::operator+=(const char16_t *pathData)
{
    size_t pathLength = std::char_traits<char16_t>::length(pathData);
    if (m_pathLength + pathLength >= m_pathSize)
    {
        return *this;
    }

    // Don't check anything. Just copy.
    std::memcpy(&m_path[m_pathLength], pathData, pathLength * sizeof(char16_t));

    m_pathLength += pathLength;

    return *this;
}

fslib::Path &fslib::Path::operator+=(const uint16_t *pathData)
{
    return *this += reinterpret_cast<const char16_t *>(pathData);
}

fslib::Path &fslib::Path::operator+=(const std::u16string &pathData)
{
    return *this += pathData.c_str();
}

fslib::Path &fslib::Path::operator+=(std::u16string_view pathData)
{
    return *this += pathData.data();
}

bool fslib::Path::allocatePath(uint16_t pathSize)
{
    Path::freePath();
    m_path = new (std::nothrow) char16_t[pathSize];
    if (!m_path)
    {
        return false;
    }
    std::memset(m_path, 0x00, pathSize * sizeof(char16_t));
    return true;
}

void fslib::Path::freePath(void)
{
    if (m_path)
    {
        delete[] m_path;
    }
}

fslib::Path fslib::operator/(const fslib::Path &pathA, const char16_t *pathB)
{
    fslib::Path newPath = pathA;
    newPath /= pathB;
    return newPath;
}

fslib::Path fslib::operator/(const fslib::Path &pathA, const uint16_t *pathB)
{
    fslib::Path newPath = pathA;
    newPath /= pathB;
    return newPath;
}

fslib::Path fslib::operator/(const fslib::Path &pathA, const std::u16string &pathB)
{
    fslib::Path newPath = pathA;
    newPath /= pathB;
    return newPath;
}

fslib::Path fslib::operator/(const fslib::Path &pathA, const std::u16string_view pathB)
{
    fslib::Path newPath = pathA;
    newPath /= pathB;
    return newPath;
}

fslib::Path fslib::operator+(const fslib::Path &pathA, const char16_t *pathB)
{
    fslib::Path newPath = pathA;
    newPath += pathB;
    return newPath;
}

fslib::Path fslib::operator+(const fslib::Path &pathA, const uint16_t *pathB)
{
    fslib::Path newPath = pathA;
    newPath += pathB;
    return newPath;
}

fslib::Path fslib::operator+(const fslib::Path &pathA, const std::u16string &pathB)
{
    fslib::Path newPath = pathA;
    newPath += pathB;
    return newPath;
}

fslib::Path fslib::operator+(const fslib::Path &pathA, std::u16string_view pathB)
{
    fslib::Path newPath = pathA;
    newPath += pathB;
    return newPath;
}
