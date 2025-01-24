#pragma once
#include <filesystem>
#include <string>

namespace fslib
{
    /// @brief Class to make working with the Switch's FS and it's odd rules much easier.
    class Path
    {
        public:
            /// @brief Default constructor for Path
            Path(void) = default;

            /// @brief Constructor for Path. Takes most standard C/C++ string types.
            /// @param path Path to assign.
            Path(const Path &path);

            /// @brief Constructor for Path. Takes most standard C/C++ string types.
            /// @param pathData String to assign.
            Path(const char *pathData);

            /// @brief Constructor for Path. Takes most standard C/C++ string types.
            /// @param pathData String to assign.
            Path(const std::string &pathData);

            /// @brief Constructor for Path. Takes most standard C/C++ string types.
            /// @param pathData String to assign.
            Path(std::string_view pathData);

            /// @brief Constructor for Path. Takes most standard C/C++ string types.
            /// @param pathData String to assign.
            Path(const std::filesystem::path &pathData);

            /// @brief Frees memory used for path.
            ~Path();

            /**
             * @brief Returns whether or not path is valid for use with FsLib and Switch's FS.
             *
             * @return True if it is. False if it's not.
             * @note Based on four conditions:
                    1. The path was properly allocated and m_Path isn't nullptr.
                    2. There was a device found in the path.
                    3. The path length following the device is not empty.
                    4. The path has no illegal characters in it.
             */
            bool isValid(void) const;

            /// @brief Returns a sub-path ending at PathLength.
            /// @param pathLength Length of sub-path to return.
            /// @return Sub-Path.
            Path subPath(size_t pathLength) const;

            /// @brief Searches for the first occurrence of Character in path.
            /// @param character Character to search for.
            /// @return Position of character in path. Path::NotFound if the character isn't found.
            size_t findFirstOf(char character) const;

            /// @brief Searches for the first occurrence of Character in path starting at Begin.
            /// @param character Character to search for.
            /// @param begin Postion to begin searching from.
            /// @return Position of character in path. Path::NotFound if the character wasn't found.
            size_t findFirstOf(char character, size_t begin) const;

            /// @brief Searches backwards through path to find last occurrence of character in path.
            /// @param character Character to search for.
            /// @return Position of character in path. Path::NotFound if the character wasn't found in path.
            size_t findLastOf(char character) const;

            /// @brief Searches backwards through path beginning at Begin to find last occurrence of character in path.
            /// @param character Character to search for.
            /// @param begin Position to "begin" at.
            /// @return Position of character in path. Path::NotFound if the character isn't found.
            size_t findLastOf(char character, size_t begin) const;

            /// @brief Returns the entire path. Ex: sdmc:/Path/To/File.txt
            /// @return Entire path.
            const char *cString(void) const;

            /// @brief Returns the device at the beginning of the path for use with FsLib's internal functions. Ex: sdmc
            /// @return Device string.
            std::string_view getDeviceName(void) const;

            /// @brief Returns the path after the device for use with Switch's FS functions. Ex: /Path/To/File.txt
            /// @return Filesystem path.
            const char *getPath(void) const;

            /// @brief Returns full path length of the path buffer.
            /// @return Path length.
            size_t getLength(void) const;

            /// @brief Assigns P to Path. Accepts most standard C/C++ string types.
            /// @param pathData Path to assign.
            /// @return Reference to path
            Path &operator=(const Path &path);

            /// @brief Assigns P to Path. Accepts most standard C/C++ string types.
            /// @param pathData String to assign.
            /// @return Reference to path
            Path &operator=(const char *pathData);

            /// @brief Assigns P to Path. Accepts most standard C/C++ string types.
            /// @param pathData String to assign.
            /// @return Reference to path
            Path &operator=(const std::string &pathData);

            /// @brief Assigns P to Path. Accepts most standard C/C++ string types.
            /// @param pathData String to assign.
            /// @return Reference to path
            Path &operator=(std::string_view pathData);

            /// @brief Assigns P to Path. Accepts most standard C/C++ string types.
            /// @param pathData String to assign.
            /// @return Reference to path
            Path &operator=(const std::filesystem::path &pathData);

            /// @brief Appends P to Path. Adds / if needed. Performs minor checks on P before appending.
            /// @param pathData String to append.
            /// @return Reference to path.
            Path &operator/=(const char *pathData);

            /// @brief Appends P to Path. Adds / if needed. Performs minor checks on P before appending.
            /// @param pathData String to append.
            /// @return Reference to path.
            Path &operator/=(const std::string &pathData);

            /// @brief Appends P to Path. Adds / if needed. Performs minor checks on P before appending.
            /// @param pathData String to append.
            /// @return Reference to path.
            Path &operator/=(std::string_view pathData);

            /// @brief Appends P to Path. Adds / if needed. Performs minor checks on P before appending.
            /// @param pathData String to append.
            /// @return Reference to path.
            Path &operator/=(const std::filesystem::path &pathData);

            /// @brief Appends P to Path without any checks or / added.
            /// @param pathData String to append.
            /// @return Reference to path.
            Path &operator+=(const char *pathData);

            /// @brief Appends P to Path without any checks or / added.
            /// @param pathData String to append.
            /// @return Reference to path.
            Path &operator+=(const std::string &pathData);

            /// @brief Appends P to Path without any checks or / added.
            /// @param pathData String to append.
            /// @return Reference to path.
            Path &operator+=(std::string_view pathData);

            /// @brief Appends P to Path without any checks or / added.
            /// @param pathData String to append.
            /// @return Reference to path.
            Path &operator+=(const std::filesystem::path &pathData);

            /**
             * @brief Value returned by Find[X]Of functions if the search fails.
             * @note This can be used two ways:
             *      1. FsLib::Path::NotFound
             *      2. [Path Instance].NotFound.
             */
            static constexpr uint16_t notFound = -1;

        private:
            // Path buffer. Switch expects a buffer 0x301 in length. unique_ptr deletes copy operators and I didn't want to use vector.
            char *m_path = nullptr;
            // Where the ':' is in the path.
            char *m_deviceEnd = nullptr;
            // Neither of these are going to exceed 0xFFFF.
            // Actual length of path buffer.
            uint16_t m_pathSize = 0;
            // Current length of path.
            uint16_t m_pathLength = 0;

            // This allocates the memory to hold the path according to pathSize.
            bool allocatePath(uint16_t pathSize);
            // Frees the buffer holding the path data.
            void freePath(void);
    };

    /// @brief Concatenates two paths. Adds a / if needed.
    /// @param pathA Base path.
    /// @param pathB Path to concatenate to Path1.
    /// @return New path consisting of both paths.
    fslib::Path operator/(const fslib::Path &pathA, const char *pathB);

    /// @brief Concatenates two paths. Adds a / if needed.
    /// @param pathA Base path.
    /// @param pathB Path to concatenate to Path1.
    /// @return New path consisting of both paths.
    fslib::Path operator/(const fslib::Path &pathA, const std::string &pathB);

    /// @brief Concatenates two paths. Adds a / if needed.
    /// @param pathA Base path.
    /// @param pathB Path to concatenate to Path1.
    /// @return New path consisting of both paths.
    fslib::Path operator/(const fslib::Path &pathA, std::string_view pathB);

    /// @brief Concatenates two paths. Adds a / if needed.
    /// @param pathA Base path.
    /// @param pathB Path to concatenate to Path1.
    /// @return New path consisting of both paths.
    fslib::Path operator/(const fslib::Path &pathA, const std::filesystem::path &pathB);

    /// @brief Unchecked concatenation operator. Doesn't perform checks or add / if needed.
    /// @param pathA Base path.
    /// @param pathB Path to concatenate to Path1
    /// @return New pat consisting of both paths.
    fslib::Path operator+(const fslib::Path &pathA, const char *pathB);

    /// @brief Unchecked concatenation operator. Doesn't perform checks or add / if needed.
    /// @param pathA Base path.
    /// @param pathB Path to concatenate to Path1
    /// @return New pat consisting of both paths.
    fslib::Path operator+(const fslib::Path &pathA, const std::string &pathB);

    /// @brief Unchecked concatenation operator. Doesn't perform checks or add / if needed.
    /// @param pathA Base path.
    /// @param pathB Path to concatenate to Path1
    /// @return New pat consisting of both paths.
    fslib::Path operator+(const fslib::Path &pathA, std::string_view pathB);

    /// @brief Unchecked concatenation operator. Doesn't perform checks or add / if needed.
    /// @param pathA Base path.
    /// @param pathB Path to concatenate to Path1
    /// @return New pat consisting of both paths.
    fslib::Path operator+(const fslib::Path &pathA, const std::filesystem::path &pathB);
} // namespace fslib
