#include "Path.hpp"

#include "error.hpp"

#include <cstring>
#include <string>
#include <switch.h>

namespace
{
    constexpr const char *FORBIDDEN_PATH_CHARACTERS = "<>:\"|?*";
} // namespace

fslib::Path::Path()
    : m_path(std::make_unique<char[]>(FS_MAX_PATH)) {};

fslib::Path::Path(const fslib::Path &path)
    : m_device(path.m_device)
    , m_path(std::make_unique<char[]>(FS_MAX_PATH))
    , m_offset(path.m_offset)
{
    const char *pathData = path.m_path.get();
    std::copy(pathData, pathData + FS_MAX_PATH, m_path.get());
}

fslib::Path::Path(Path &&path) noexcept
    : m_device(std::move(path.m_device))
    , m_path(std::move(path.m_path))
    , m_offset(path.m_offset)
{
    path.m_path   = std::make_unique<char[]>(FS_MAX_PATH);
    path.m_offset = 0;
}

fslib::Path::Path(const char *path)
    : Path()
{
    *this = path;
}

fslib::Path::Path(const std::string &path)
    : Path()
{
    *this = path;
}

fslib::Path::Path(std::string_view pathData)
    : Path()
{
    *this = pathData;
}

fslib::Path::Path(const std::filesystem::path &path)
    : Path()
{
    *this = path;
}

bool fslib::Path::is_valid() const noexcept
{
    const bool validDevice       = !m_device.empty();
    const bool validLength       = m_offset >= 1;
    const bool containsForbidden = std::strpbrk(m_path.get(), FORBIDDEN_PATH_CHARACTERS) != NULL;
    if (!validDevice || !validLength || containsForbidden)
    {
        error::occurred(error::codes::INVALID_PATH);
        return false;
    }

    return true;
}

fslib::Path fslib::Path::sub_path(size_t pathLength) const
{
    if (pathLength > FS_MAX_PATH) { pathLength = FS_MAX_PATH; }

    // To do: This needs to ensure a starting slash.
    fslib::Path newPath{};
    newPath.m_device = m_device;
    newPath.m_offset = pathLength;

    const char *path = m_path.get();
    std::copy(path, path + pathLength, m_path.get());

    return newPath;
}

size_t fslib::Path::find_first_of(char character) const noexcept
{
    for (size_t i = 0; i < m_offset; i++)
    {
        if (m_path[i] == character) { return i; }
    }

    return Path::NOT_FOUND;
}

size_t fslib::Path::find_first_of(char character, size_t begin) const noexcept
{
    if (begin >= m_offset) { return Path::NOT_FOUND; }

    for (size_t i = begin; i < m_offset; i++)
    {
        if (m_path[i] == character) { return i; }
    }

    return Path::NOT_FOUND;
}

size_t fslib::Path::find_first_not_of(char character) const noexcept
{
    for (size_t i = 0; i < m_offset; i++)
    {
        if (m_path[i] != character) { return i; }
    }

    return Path::NOT_FOUND;
}

size_t fslib::Path::find_first_not_of(char character, size_t begin) const noexcept
{
    if (begin >= m_offset) { return Path::NOT_FOUND; }

    for (size_t i = begin; i < m_offset; i++)
    {
        if (m_path[i] != character) { return i; }
    }

    return Path::NOT_FOUND;
}

size_t fslib::Path::find_last_of(char character) const noexcept
{
    for (size_t i = m_offset; i-- > 0;)
    {
        if (m_path[i] == character) { return i; }
    }

    return Path::NOT_FOUND;
}

size_t fslib::Path::find_last_of(char character, size_t begin) const noexcept
{
    if (begin > m_offset) { begin = m_offset; }

    for (size_t i = begin; i-- > 0;)
    {
        if (m_path[i] == character) { return i; }
    }

    return Path::NOT_FOUND;
}

size_t fslib::Path::find_last_not_of(char character) const noexcept
{
    for (size_t i = m_offset; i-- > 0;)
    {
        if (m_path[i] != character) { return i; }
    }

    return Path::NOT_FOUND;
}

size_t fslib::Path::find_last_not_of(char character, size_t begin) const noexcept
{
    if (begin >= m_offset) { begin = m_offset; }

    for (size_t i = m_offset; i-- > 0;)
    {
        if (m_path[i] != character) { return i; }
    }

    return Path::NOT_FOUND;
}

std::string fslib::Path::string() const { return m_device + ":" + m_path.get(); }

std::string_view fslib::Path::get_device_name() const noexcept { return m_device; }

const char *fslib::Path::get_path() const { return m_path.get(); }

const char *fslib::Path::get_filename() const noexcept
{
    size_t lastSlash = Path::find_last_of('/');
    if (lastSlash == Path::NOT_FOUND) { return nullptr; }
    // This could be dangerous. To do: Fix that.
    return &m_path[lastSlash + 1];
}

const char *fslib::Path::get_extension() const noexcept
{
    size_t extensionBegin = Path::find_last_of('.');
    if (extensionBegin == Path::NOT_FOUND) { return nullptr; }
    // To do: This is not safe.
    return &m_path[extensionBegin + 1];
}

size_t fslib::Path::get_length() const noexcept { return m_offset; }

fslib::Path &fslib::Path::operator=(const fslib::Path &path)
{
    m_device = path.m_device;
    m_offset = path.m_offset;

    const char *pathData = path.m_path.get();
    std::copy(pathData, pathData + FS_MAX_PATH, m_path.get());

    return *this;
}

fslib::Path &fslib::Path::operator=(fslib::Path &&path) noexcept
{
    m_device = std::move(path.m_device);
    m_path   = std::move(path.m_path);
    m_offset = path.m_offset;

    // Just in case.
    path.m_device.clear();
    path.m_path   = std::make_unique<char[]>(FS_MAX_PATH);
    path.m_offset = 0;

    return *this;
}

fslib::Path &fslib::Path::operator=(const char *path) { return *this = std::string_view(path); }

fslib::Path &fslib::Path::operator=(const std::string &path) { return *this = std::string_view(path); }

fslib::Path &fslib::Path::operator=(std::string_view path)
{
    const size_t deviceEnd = path.find_first_of(':');
    if (deviceEnd == path.npos) { return *this; }

    m_offset                = 0;
    m_path[m_offset++]      = '/'; // Ensure this starts with a slash.
    m_device                = path.substr(0, deviceEnd);
    std::string_view fsPath = path.substr(deviceEnd + 1);

    const size_t pathBegin = fsPath.find_first_not_of('/');
    const size_t pathEnd   = fsPath.find_last_not_of('/');
    if (pathBegin == fsPath.npos || pathEnd == fsPath.npos) { return *this; }

    const size_t fsPathLength = (pathEnd - pathBegin) + 1;
    if (fsPathLength + 1 >= FS_MAX_PATH) { return *this; }

    fsPath               = fsPath.substr(pathBegin);
    const char *pathData = fsPath.data();
    std::copy(pathData, pathData + fsPathLength, &m_path[m_offset]);
    m_offset += fsPathLength;
    Path::null_terminate();

    return *this;
}

fslib::Path &fslib::Path::operator=(const std::filesystem::path &path) noexcept
{
    return *this = std::string_view(path.string());
}

fslib::Path &fslib::Path::operator/=(const char *path) noexcept { return *this /= std::string_view(path); }

fslib::Path &fslib::Path::operator/=(const std::string &path) noexcept { return *this /= std::string_view(path); }

fslib::Path &fslib::Path::operator/=(std::string_view path) noexcept
{
    const size_t pathBegin = path.find_first_not_of('/');
    const size_t pathEnd   = path.find_last_not_of('/');
    if (pathBegin == path.npos || pathEnd == path.npos) { return *this; }

    // This makes things easier.
    const size_t length = (pathEnd - pathBegin) + 1;
    if (m_offset + length + 1 >= FS_MAX_PATH) { return *this; }

    // Needed to avoid doubling up slashes directly appending to a device root.
    if (m_path[m_offset - 1] != '/') { m_path[m_offset++] = '/'; }

    // This looks really dangerous for some reason. I like it though.
    const std::string_view slice = path.substr(pathBegin);
    const char *pathData         = slice.data();
    std::copy(pathData, pathData + length, &m_path[m_offset]);
    m_offset += length;
    Path::null_terminate();

    return *this;
}

fslib::Path &fslib::Path::operator/=(const std::filesystem::path &path) noexcept
{
    return *this /= std::string_view(path.string());
}

fslib::Path &fslib::Path::operator/=(const fslib::DirectoryEntry &path) noexcept
{
    return *this /= std::string_view(path.get_filename());
}

fslib::Path &fslib::Path::operator+=(const char *path) noexcept { return *this += std::string_view(path); }

fslib::Path &fslib::Path::operator+=(const std::string &path) noexcept { return *this += std::string_view(path); }

fslib::Path &fslib::Path::operator+=(std::string_view path) noexcept
{
    const size_t length = path.length();
    if (m_offset + length >= FS_MAX_PATH) { return *this; }

    const char *pathData = path.data();
    std::copy(pathData, pathData + length, &m_path[m_offset]);
    m_offset += length;
    Path::null_terminate();

    return *this;
}

fslib::Path &fslib::Path::operator+=(const std::filesystem::path &path) noexcept
{
    return *this += std::string_view(path.string());
}

fslib::Path &fslib::Path::operator+=(const fslib::DirectoryEntry &path) noexcept
{
    return *this += std::string_view(path.get_filename());
}

void fslib::Path::null_terminate() noexcept { m_path[m_offset] = '\0'; }

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

bool fslib::operator==(const fslib::Path &pathA, const fslib::Path &pathB) noexcept
{
    if (pathA.get_device_name() != pathB.get_device_name()) { return false; }
    else if (pathA.get_length() != pathB.get_length()) { return false; }

    const char *fsPathA = pathA.get_path();
    const char *fsPathB = pathB.get_path();
    return std::strcmp(fsPathA, fsPathB) == 0;
}