#pragma once
#include <array>
#include <filesystem>
#include <string>

namespace fslib
{
    /// @brief Class to make working with the Switch's FS and it's odd rules much easier.
    class Path
    {
        public:
            /// @brief Default constructor for Path
            Path() = default;

            /// @brief Constructor for Path. Takes most standard C/C++ string types.
            /// @param path Path to assign.
            Path(const Path &path);

            /// @brief Move constructor for path.
            /// @param path Path to eviscerate.
            Path(Path &&path);

            /// @brief Constructor for path.
            /// @param path String to assign.
            Path(const char *path);

            /// @brief Constructor for path.
            /// @param path String to assign.
            Path(const std::string &path);

            /// @brief Constructor for path.
            /// @param path String to assign.
            Path(std::string_view path);

            /// @brief Constructor for path.
            /// @param path Path to assign.
            Path(const std::filesystem::path &path);

            /**
             * @brief Return whether or not path is valid for use with FsLib and Switch's FS.
             *
             * @return True if it is. False if it's not.
             * @note Based on four conditions:
                    1. The path was properly allocated and m_Path isn't nullptr.
                    2. There was a device found in the path.
                    3. The path length following the device is not empty.
                    4. The path has no illegal characters in it.
             */
            bool is_valid() const;

            /// @brief Returns a sub-path ending at PathLength.
            /// @param pathLength Length of sub-path to return.
            /// @return Sub-Path.
            Path sub_path(size_t pathLength) const;

            /// @brief Searches for the first occurrence of Character in path.
            /// @param character Character to search for.
            /// @return Position of character in path. Path::NOT_FOUND if the character isn't found.
            size_t find_first_of(char character) const;

            /// @brief Searches for the first occurrence of Character in path starting at Begin.
            /// @param character Character to search for.
            /// @param begin Postion to begin searching from.
            /// @return Position of character in path. Path::NOT_FOUND if the character wasn't found.
            size_t find_first_of(char character, size_t begin) const;

            /// @brief Searches backwards through path to find last occurrence of character in path.
            /// @param character Character to search for.
            /// @return Position of character in path. Path::NOT_FOUND if the character wasn't found in path.
            size_t find_last_of(char character) const;

            /// @brief Searches backwards through path beginning at Begin to find last occurrence of character in path.
            /// @param character Character to search for.
            /// @param begin Position to "begin" at.
            /// @return Position of character in path. Path::NOT_FOUND if the character isn't found.
            size_t find_last_of(char character, size_t begin) const;

            /// @brief Returns the entire path. Ex: sdmc:/Path/To/File.txt
            const char *full_path() const;

            /// @brief Returns the device at the beginning of the path for use with FsLib's internal functions.
            /// @note Trying to use .data() with this will just result in the entire path being returned.
            std::string_view get_device_name() const;

            /// @brief Returns the file name in the path starting at the final '/' found.
            const char *get_filename() const;

            /// @brief Returns the path after the device for use with Switch's FS functions. Ex: /Path/To/File.txt
            const char *get_path() const;

            /// @brief Returns the extension. After the '.'
            const char *get_extension() const;

            /// @brief Returns full path length of the path buffer.
            size_t get_length() const;

            /// @brief Assigns P to Path. Accepts most standard C/C++ string types.
            /// @param pathData Path to assign.
            Path &operator=(const Path &path);

            /// @brief Move = operator.
            /// @param path Path to eviscerate.
            Path &operator=(Path &&path);

            /// @brief Assigns path
            /// @param path String to assign from.
            Path &operator=(const char *path);

            /// @brief Assigns path
            /// @param path String to assign from.
            Path &operator=(const std::string &path);

            /// @brief Assigns path
            /// @param pathData String to assign from.
            Path &operator=(std::string_view path);

            /// @brief Assigns path from path passed.
            /// @param path Path to assign from.
            Path &operator=(const std::filesystem::path &path);

            /// @brief Appends path to Path.
            /// @param path String to append.
            Path &operator/=(const char *path);

            /// @brief Appends path to Path.
            /// @param path String to append.
            Path &operator/=(const std::string &path);

            /// @brief Appends path to Path.
            /// @param path String to append.
            Path &operator/=(std::string_view path);

            /// @brief Appends path to Path.
            /// @param path String to append.
            Path &operator/=(const std::filesystem::path &path);

            /// @brief Appends path to Path. Unchecked.
            /// @param path String to append.
            Path &operator+=(const char *path);

            /// @brief Appends path to Path. Unchecked.
            /// @param path String to append.
            Path &operator+=(const std::string &path);

            /// @brief Appends path to Path. Unchecked.
            /// @param path String to append.
            Path &operator+=(std::string_view path);

            /// @brief Appends path to Path. Unchecked.
            /// @param path String to append.
            Path &operator+=(const std::filesystem::path &path);

            /**
             * @brief Value returned by Find[X]Of functions if the search fails.
             * @note This can be used two ways:
             *      1. FsLib::Path::NOT_FOUND
             *      2. [Path Instance].NOT_FOUND.
             */
            static constexpr uint16_t NOT_FOUND = -1;

        private:
            /// @brief Path buffer.
            /** @note The Switch seems to only really like buffers 0x301 in length? Using STL containers with short
             * paths can seemingly cause random errors for no reason?
            */
            std::unique_ptr<char[]> m_path = nullptr;

            /// @brief This is the position where the device ends.
            const char *m_deviceEnd = nullptr;

            /// @brief This is the actual length of the path buffer
            uint16_t m_pathSize = 0;

            /// @brief The current length of the path.
            uint16_t m_pathLength = 0;
    };


    /// @brief Concatenates two paths. Adds a / if needed.
    /// @param pathA Base path.
    /// @param pathB Path to concatenate to Path1.
    fslib::Path operator/(const fslib::Path &pathA, const char *pathB);

    /// @brief Concatenates two paths. Adds a / if needed.
    /// @param pathA Base path.
    /// @param pathB Path to concatenate to Path1.
    fslib::Path operator/(const fslib::Path &pathA, const std::string &pathB);

    /// @brief Concatenates two paths. Adds a / if needed.
    /// @param pathA Base path.
    /// @param pathB Path to concatenate to Path1.
    fslib::Path operator/(const fslib::Path &pathA, std::string_view pathB);

    /// @brief Concatenates two paths. Adds a / if needed.
    /// @param pathA Base path.
    /// @param pathB Path to concatenate to Path1.
    fslib::Path operator/(const fslib::Path &pathA, const std::filesystem::path &pathB);

    /// @brief Unchecked concatenation operator. Doesn't perform checks or add / if needed.
    /// @param pathA Base path.
    /// @param pathB Path to concatenate to Path1
    fslib::Path operator+(const fslib::Path &pathA, const char *pathB);

    /// @brief Unchecked concatenation operator. Doesn't perform checks or add / if needed.
    /// @param pathA Base path.
    /// @param pathB Path to concatenate to Path1
    fslib::Path operator+(const fslib::Path &pathA, const std::string &pathB);

    /// @brief Unchecked concatenation operator. Doesn't perform checks or add / if needed.
    /// @param pathA Base path.
    /// @param pathB Path to concatenate to Path1
    fslib::Path operator+(const fslib::Path &pathA, std::string_view pathB);

    /// @brief Concatenates two paths. Adds a / if needed.
    /// @param pathA Base path.
    /// @param pathB Path to concatenate to Path1.
    fslib::Path operator+(const fslib::Path &pathA, const std::filesystem::path &pathB);
} // namespace fslib
