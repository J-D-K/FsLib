#pragma once
#include <3ds.h>
#include <cstdint>
#include <string>

namespace fslib
{
    /// @brief The maximum path length FsLib on 3DS supports.
    inline constexpr size_t MAX_PATH = 0x301;

    /// @brief Class to make working with UTF-16 paths easier to manage.
    class Path
    {
        public:
            /// @brief Default path constructor.
            Path() = default;

            /// @brief Creates a new path for use with FsLib
            /// @param path Path to assign from.
            Path(const Path &path);

            /// @brief Move constructor.
            /// @param path Path to eviscerate.
            Path(Path &&path);

            /// @brief Creates a new path for use with FsLib
            /// @param path UTF-16 string to assign from.
            Path(const char16_t *path);

            /// @brief Creates a new path for use with FsLib
            /// @param path UTF-16 string to assign from.
            Path(const uint16_t *path);

            /// @brief Creates a new path for use with FsLib
            /// @param path UTF-16 string to assign from.
            Path(const std::u16string &path);

            /// @brief Creates a new path for use with FsLib
            /// @param path UTF-16 string to assign from.
            Path(std::u16string_view path);

            /// @brief Performs checks and returns if path is valid for use with FsLib.
            /// @return True if path is valid. False if it is not.
            bool is_valid() const;

            /// @brief Returns a sub-path ending at PathLength
            /// @param pathLength Length of subpath to return.
            /// @return Sub-path.
            Path sub_path(size_t length) const;

            /// @brief Searches for first occurrence of Character in Path. Overload starts at Begin.
            /// @param character Character to search.
            /// @return Position of Character or Path::NotFound on failure.
            size_t find_first_of(char16_t character) const;
            size_t find_first_of(char16_t character, size_t begin) const;

            /// @brief Searches backwards to find last occurrence of Character in string. Overload starts at begin.
            /// @param character Character to search for.
            /// @return Position of Character or Path::NotFound on failure.
            size_t find_last_of(char16_t character) const;
            size_t find_last_of(char16_t character, size_t begin) const;

            /// @brief Returns the entire path as a C const char16_t* String
            /// @return Pointer to path string buffer.
            const char16_t *full_path() const;

            /// @brief Returns the device as a UTF-16 u16string_view for use with FsLib internally.
            /// @return Device string.
            std::u16string_view get_device() const;

            /// @brief Returns file name as u16string_view.
            /// @return File name
            const char16_t *get_filename() const;

            /// @brief Returns extension of path as u16string_view.
            /// @return Path's extension.
            const char16_t *get_extension() const;

            /// @brief Returns an FS_Path for use with 3DS FS functions.
            /// @return FS_Path
            FS_Path get_fs_path() const;

            /// @brief Returns length of the entire path string.
            /// @return Length of path string.
            size_t get_length() const;

            /// @brief Assigns Path from various standard UTF-16 string types.
            /// @param path Path to assign from
            /// @return Reference to current path.
            Path &operator=(const Path &path);

            /// @brief Move operator.
            /// @param path Path to eviscerate.
            Path &operator=(Path &&path);

            /// @brief Assigns Path from various standard UTF-16 string types.
            /// @param path UTF-16 string to assign from
            /// @return Reference to current path.
            Path &operator=(const char16_t *path);

            /// @brief Assigns Path from various standard UTF-16 string types.
            /// @param path UTF-16 string to assign from
            /// @return Reference to current path.
            Path &operator=(const uint16_t *path);

            /// @brief Assigns Path from various standard UTF-16 string types.
            /// @param path UTF-16 string to assign from
            /// @return Reference to current path.
            Path &operator=(const std::u16string &path);

            /// @brief Assigns Path from various standard UTF-16 string types.
            /// @param path UTF-16 string to assign from
            /// @return Reference to current path.
            Path &operator=(std::u16string_view path);

            /// @brief Preferred appending operator. Adds / if needed between paths. Also trims slashes from input.
            /// @param path UTF-16 string to append.
            /// @return Reference to current Path
            Path &operator/=(const char16_t *path);

            /// @brief Preferred appending operator. Adds / if needed between paths. Also trims slashes from input.
            /// @param path UTF-16 string to append.
            /// @return Reference to current Path
            Path &operator/=(const uint16_t *path);

            /// @brief Preferred appending operator. Adds / if needed between paths. Also trims slashes from input.
            /// @param path UTF-16 string to append.
            /// @return Reference to current Path
            Path &operator/=(const std::u16string &path);

            /// @brief Preferred appending operator. Adds / if needed between paths. Also trims slashes from input.
            /// @param path UTF-16 string to append.
            /// @return Reference to current Path
            Path &operator/=(std::u16string_view path);

            /// @brief Unchecked appending operator. Input is not checked for validity of string appended.
            /// @param path UTF-16 string to append.
            /// @return Reference to current Path.
            Path &operator+=(const char16_t *path);

            /// @brief Unchecked appending operator. Input is not checked for validity of string appended.
            /// @param path UTF-16 string to append.
            /// @return Reference to current Path.
            Path &operator+=(const uint16_t *path);

            /// @brief Unchecked appending operator. Input is not checked for validity of string appended.
            /// @param path UTF-16 string to append.
            /// @return Reference to current Path.
            Path &operator+=(const std::u16string &path);

            /// @brief Unchecked appending operator. Input is not checked for validity of string appended.
            /// @param path UTF-16 string to append.
            /// @return Reference to current Path.
            Path &operator+=(std::u16string_view path);

            /// @brief This is the value that is returned when find[X]Of can't find the character.
            static constexpr size_t NOT_FOUND = std::u16string::npos;

        private:
            /// @brief 3DS doesn't have weird problems with path buffer sizes like switch so...
            std::u16string m_path{};
    };

    /// @brief Concatenates a path to a string and returns a new one. Checks are performed and / is added if needed.
    /// @param pathA BasePath
    /// @param pathB Path to concatenate to pathB
    /// @return New path containing concatenated paths.
    fslib::Path operator/(const fslib::Path &pathA, const char16_t *pathB);

    /// @brief Concatenates a path to a string and returns a new one. Checks are performed and / is added if needed.
    /// @param pathA BasePath
    /// @param pathB Path to concatenate to pathB
    /// @return New path containing concatenated paths.
    fslib::Path operator/(const fslib::Path &pathA, const uint16_t *pathB);

    /// @brief Concatenates a path to a string and returns a new one. Checks are performed and / is added if needed.
    /// @param pathA BasePath
    /// @param pathB Path to concatenate to pathB
    /// @return New path containing concatenated paths.
    fslib::Path operator/(const fslib::Path &pathA, const std::u16string &pathB);

    /// @brief Concatenates a path to a string and returns a new one. Checks are performed and / is added if needed.
    /// @param pathA BasePath
    /// @param pathB Path to concatenate to pathB
    /// @return New path containing concatenated paths.
    fslib::Path operator/(const fslib::Path &pathA, std::u16string_view pathB);

    /// @brief Concatenates a path to a string and returns a new one. No checks are performed and pathB is appended as-is.
    /// @param pathA BasePath
    /// @param pathB Path to concatenate to pathA
    /// @return New path containing concatenated paths.
    fslib::Path operator+(const fslib::Path &pathA, const char16_t *pathB);

    /// @brief Concatenates a path to a string and returns a new one. No checks are performed and pathB is appended as-is.
    /// @param pathA BasePath
    /// @param pathB Path to concatenate to pathA
    /// @return New path containing concatenated paths.
    fslib::Path operator+(const fslib::Path &pathA, const uint16_t *pathB);

    /// @brief Concatenates a path to a string and returns a new one. No checks are performed and pathB is appended as-is.
    /// @param pathA BasePath
    /// @param pathB Path to concatenate to pathA
    /// @return New path containing concatenated paths.
    fslib::Path operator+(const fslib::Path &pathA, const std::u16string &pathB);

    /// @brief Concatenates a path to a string and returns a new one. No checks are performed and pathB is appended as-is.
    /// @param pathA BasePath
    /// @param pathB Path to concatenate to pathA
    /// @return New path containing concatenated paths.
    fslib::Path operator+(const fslib::Path &pathA, std::u16string_view pathB);
} // namespace fslib
