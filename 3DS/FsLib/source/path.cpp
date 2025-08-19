#include "Path.hpp"

#include "EmptyPath.hpp"
#include "string.hpp"

#include <algorithm>
#include <array>
#include <cstring>

namespace
{
    // Array of characters that are forbidden in paths.
    constexpr std::array<char16_t, 8> FORBIDDEN_CHARS = {u'<', u'>', u':', u'\\', u'"', u'|', u'?', u'*'};

    constexpr char16_t CHAR16_COLON  = u':';
    constexpr char16_t CHAR16_SLASH  = u'/';
    constexpr char16_t CHAR16_PERIOD = u'.';
} // namespace

// Definitions at bottom.
static bool contains_forbidden_chars(std::u16string_view string);

fslib::Path::Path(const fslib::Path &path) { *this = path; }

fslib::Path::Path(fslib::Path &&path) { *this = std::move(path); }

fslib::Path::Path(const char16_t *path) { *this = path; }

fslib::Path::Path(const uint16_t *path) { *this = path; }

fslib::Path::Path(const std::u16string &path) { *this = path; }

fslib::Path::Path(const std::u16string_view path) { *this = path; }

bool fslib::Path::is_valid() const
{
    const char16_t *deviceEnd = std::char_traits<char16_t>::find(m_path.c_str(), m_path.length(), CHAR16_SLASH);
    const bool lengthCheck    = deviceEnd && std::char_traits<char16_t>::length(deviceEnd + 1) > 0;
    const bool hasForbidden   = lengthCheck && contains_forbidden_chars(m_path);

    return deviceEnd && lengthCheck && hasForbidden;
}

fslib::Path fslib::Path::sub_path(size_t length) const
{
    const char16_t *cPath = m_path.c_str();
    const std::u16string_view subPath{cPath, length};
    fslib::Path newPath{subPath};

    return newPath;
}

size_t fslib::Path::find_first_of(char16_t character) const { return m_path.find_first_of(character); }

size_t fslib::Path::find_first_of(char16_t character, size_t start) const { return m_path.find_first_of(character, start); }

size_t fslib::Path::find_first_not_of(char16_t character) const { return m_path.find_first_not_of(character); }

size_t fslib::Path::find_first_not_of(char16_t character, size_t start) const
{
    return m_path.find_first_not_of(character, start);
}

size_t fslib::Path::find_last_of(char16_t character) const { return m_path.find_last_of(character); }

size_t fslib::Path::find_last_of(char16_t character, size_t start) const { return m_path.find_last_of(character, start); }

size_t fslib::Path::find_last_not_of(char16_t character) const { return m_path.find_last_not_of(character); }

size_t fslib::Path::find_last_not_of(char16_t character, size_t start) const
{
    return m_path.find_last_not_of(character, start);
}

const char16_t *fslib::Path::full_path() const { return m_path.c_str(); }

std::u16string_view fslib::Path::get_device() const
{
    size_t deviceEnd = m_path.find_first_of(CHAR16_COLON);
    if (deviceEnd == m_path.npos) { return std::u16string_view(u""); }

    const char16_t *cPath = m_path.c_str();
    return std::u16string_view{cPath, deviceEnd};
}

const char16_t *fslib::Path::get_filename() const
{
    const size_t nameBegin = m_path.find_last_of(CHAR16_SLASH);
    if (nameBegin == m_path.npos) { return nullptr; }

    return &m_path.c_str()[nameBegin + 1];
}

const char16_t *fslib::Path::get_extension() const
{
    const size_t extBegin = m_path.find_last_of(CHAR16_PERIOD);
    if (extBegin == m_path.npos) { return nullptr; }
    return &m_path.c_str()[extBegin + 1];
}

FS_Path fslib::Path::get_fs_path() const
{
    static constexpr size_t SIZE_CHAR16 = sizeof(char16_t);

    size_t deviceEnd = m_path.find_first_of(CHAR16_COLON);
    if (deviceEnd == m_path.npos) { return EMPTY_PATH; }
    ++deviceEnd;

    const char16_t *pathString = m_path.c_str();
    const std::u16string_view pathData{&pathString[deviceEnd]};

    const uint32_t pathLength = pathData.length() * SIZE_CHAR16 + SIZE_CHAR16;
    const FS_Path fsPath      = {.type = PATH_UTF16, .size = pathLength, .data = pathData.data()};

    return fsPath;
}

size_t fslib::Path::get_length() const { return m_path.length(); }

fslib::Path &fslib::Path::operator=(const fslib::Path &path)
{
    m_path = path.m_path;
    return *this;
}

fslib::Path &fslib::Path::operator=(fslib::Path &&path)
{
    m_path = std::move(path.m_path);
    return *this;
}

fslib::Path &fslib::Path::operator=(const char16_t *path) { return *this = std::u16string_view(path); }

fslib::Path &fslib::Path::operator=(const uint16_t *path)
{
    const char16_t *castPath = reinterpret_cast<const char16_t *>(path);
    return *this             = std::u16string_view(castPath);
}

fslib::Path &fslib::Path::operator=(const std::u16string &path) { return *this = std::u16string_view(path); }

fslib::Path &fslib::Path::operator=(std::u16string_view path)
{
    const size_t deviceEnd = path.find_first_of(CHAR16_SLASH);
    if (deviceEnd == path.npos) { return *this; }

    const std::u16string_view device  = path.substr(0, deviceEnd + 1);
    const std::u16string_view absPath = path.substr(deviceEnd + 1);
    size_t pathBegin                  = absPath.find_first_not_of(CHAR16_SLASH);
    size_t pathEnd                    = absPath.find_last_not_of(CHAR16_SLASH);
    if (pathBegin == absPath.npos || pathEnd == absPath.npos) { return *this; }

    m_path = device;
    m_path += absPath.substr(pathBegin, (pathEnd - pathBegin) + 1);
    return *this;
}

fslib::Path &fslib::Path::operator/=(const char16_t *path) { return *this /= std::u16string_view(path); }

fslib::Path &fslib::Path::operator/=(const uint16_t *path)
{
    const char16_t *castPath = reinterpret_cast<const char16_t *>(path);
    return *this /= std::u16string_view(castPath);
}

fslib::Path &fslib::Path::operator/=(const std::u16string &path) { return *this /= std::u16string_view(path); }

fslib::Path &fslib::Path::operator/=(std::u16string_view path)
{
    const size_t pathBegin = path.find_first_not_of(CHAR16_SLASH);
    const size_t pathEnd   = path.find_last_not_of(CHAR16_SLASH);
    if (pathBegin == path.npos || pathEnd == path.npos) { return *this; }

    if (m_path.back() != CHAR16_SLASH) { m_path.append(u"/"); }

    m_path += path.substr(pathBegin, (pathEnd - pathBegin) + 1);
    return *this;
}

fslib::Path &fslib::Path::operator+=(const char16_t *path) { return *this += std::u16string_view(path); }

fslib::Path &fslib::Path::operator+=(const uint16_t *path)
{
    const char16_t *castPath = reinterpret_cast<const char16_t *>(path);
    return *this += std::u16string_view(castPath);
}

fslib::Path &fslib::Path::operator+=(const std::u16string &path) { return *this += std::u16string_view(path); }

fslib::Path &fslib::Path::operator+=(std::u16string_view path)
{
    m_path.append(path);
    return *this;
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

static bool contains_forbidden_chars(std::u16string_view string)
{
    for (const char16_t point : string)
    {
        const auto findForbidden = std::find(FORBIDDEN_CHARS.begin(), FORBIDDEN_CHARS.end(), point);
        if (findForbidden != FORBIDDEN_CHARS.end()) { return true; }
    }
    return false;
}
