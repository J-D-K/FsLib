#pragma once
#include <3ds.h>
#include <cstdint>
#include <memory>
#include <string>

namespace fslib
{
    /// @brief The maximum path length FsLib on 3DS supports.
    static constexpr size_t MAX_PATH = 0x301;

    /// @brief Class to make working with UTF-16 paths easier to manage.
    class Path
    {
        public:
            /// @brief Default path constructor.
            Path(void) = default;

            /// @brief Creates a new path for use with FsLib
            /// @param path Path to assign from.
            Path(const Path &path);

            /// @brief Creates a new path for use with FsLib
            /// @param pathData UTF-16 string to assign from.
            Path(const char16_t *P);

            /// @brief Creates a new path for use with FsLib
            /// @param pathData UTF-16 string to assign from.
            Path(const uint16_t *P);

            /// @brief Creates a new path for use with FsLib
            /// @param pathData UTF-16 string to assign from.
            Path(const std::u16string &P);

            /// @brief Creates a new path for use with FsLib
            /// @param pathData UTF-16 string to assign from.
            Path(std::u16string_view P);

            /// @brief Frees path buffer.
            ~Path();

            /// @brief Performs checks and returns if path is valid for use with FsLib.
            /// @return True if path is valid. False if it is not.
            bool isValid(void) const;

            /// @brief Returns a sub-path ending at PathLength
            /// @param pathLength Length of subpath to return.
            /// @return Sub-path.
            Path subPath(size_t pathLength) const;

            /// @brief Searches for first occurrence of Character in Path. Overload starts at Begin.
            /// @param character Character to search.
            /// @return Position of Character or Path::NotFound on failure.
            size_t findFirstOf(char16_t character) const;
            size_t findFirstOf(char16_t character, size_t begin) const;

            /// @brief Searches backwards to find last occurrence of Character in string. Overload starts at begin.
            /// @param character Character to search for.
            /// @return Position of Character or Path::NotFound on failure.
            size_t findLastOf(char16_t character) const;
            size_t findLastOf(char16_t character, size_t begin) const;

            /// @brief Returns the entire path as a C const char16_t* String
            /// @return Pointer to path string buffer.
            const char16_t *cString(void) const;

            /// @brief Returns the device as a UTF-16 u16string_view for use with FsLib internally.
            /// @return Device string.
            std::u16string_view getDevice(void) const;

            /// @brief Returns file name as u16string_view.
            /// @return File name
            std::u16string_view getFileName(void) const;

            /// @brief Returns extension of path as u16string_view.
            /// @return Path's extension.
            std::u16string_view getExtension(void) const;

            /// @brief Returns an FS_Path for use with 3DS FS functions.
            /// @return FS_Path
            FS_Path getPath(void) const;

            /// @brief Returns length of the entire path string.
            /// @return Length of path string.
            size_t getLength(void) const;

            /// @brief Assigns Path from various standard UTF-16 string types.
            /// @param path Path to assign from
            /// @return Reference to current path.
            Path &operator=(const Path &path);

            /// @brief Assigns Path from various standard UTF-16 string types.
            /// @param pathData UTF-16 string to assign from
            /// @return Reference to current path.
            Path &operator=(const char16_t *pathData);

            /// @brief Assigns Path from various standard UTF-16 string types.
            /// @param pathData UTF-16 string to assign from
            /// @return Reference to current path.
            Path &operator=(const uint16_t *pathData);

            /// @brief Assigns Path from various standard UTF-16 string types.
            /// @param pathData UTF-16 string to assign from
            /// @return Reference to current path.
            Path &operator=(const std::u16string &);

            /// @brief Assigns Path from various standard UTF-16 string types.
            /// @param pathData UTF-16 string to assign from
            /// @return Reference to current path.
            Path &operator=(std::u16string_view pathData);

            /// @brief Preferred appending operator. Adds / if needed between paths. Also trims slashes from input.
            /// @param pathData UTF-16 string to append.
            /// @return Reference to current Path
            Path &operator/=(const char16_t *pathData);

            /// @brief Preferred appending operator. Adds / if needed between paths. Also trims slashes from input.
            /// @param pathData UTF-16 string to append.
            /// @return Reference to current Path
            Path &operator/=(const uint16_t *pathData);

            /// @brief Preferred appending operator. Adds / if needed between paths. Also trims slashes from input.
            /// @param pathData UTF-16 string to append.
            /// @return Reference to current Path
            Path &operator/=(const std::u16string &pathData);

            /// @brief Preferred appending operator. Adds / if needed between paths. Also trims slashes from input.
            /// @param pathData UTF-16 string to append.
            /// @return Reference to current Path
            Path &operator/=(std::u16string_view pathData);

            /// @brief Unchecked appending operator. Input is not checked for validity of string appended.
            /// @param pathData UTF-16 string to append.
            /// @return Reference to current Path.
            Path &operator+=(const char16_t *pathData);

            /// @brief Unchecked appending operator. Input is not checked for validity of string appended.
            /// @param pathData UTF-16 string to append.
            /// @return Reference to current Path.
            Path &operator+=(const uint16_t *pathData);

            /// @brief Unchecked appending operator. Input is not checked for validity of string appended.
            /// @param pathData UTF-16 string to append.
            /// @return Reference to current Path.
            Path &operator+=(const std::u16string &pathData);

            /// @brief Unchecked appending operator. Input is not checked for validity of string appended.
            /// @param pathData UTF-16 string to append.
            /// @return Reference to current Path.
            Path &operator+=(std::u16string_view pathData);

            /// @brief This is the value that is returned when find[X]Of can't find the character.
            static constexpr uint16_t notFound = -1;

        private:
            /// @brief Buffer 0x301 + Device length * sizeof(char16_t) bytes long containing path data.
            char16_t *m_path = nullptr;

            /// @brief Pointer to where the end of the device in the path is located.
            const char16_t *m_deviceEnd = nullptr;

            /// @brief Size of path buffer.
            uint16_t m_pathSize = 0;

            /// @brief Current length of the path.
            uint16_t m_pathLength = 0;

            /// @brief Allocates memory to hold path.
            /// @param pathSize Size of buffer for path.
            /// @return True on success. False on failure.
            /// @note The path is a raw pointer to allocated memory so copies can be made.
            bool allocatePath(uint16_t pathSize);

            /// @brief Frees memory used for path buffer if it isn't nullptr.
            void freePath(void);
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
