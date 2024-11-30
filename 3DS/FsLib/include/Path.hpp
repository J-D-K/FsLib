#pragma once
#include <3ds.h>
#include <cstdint>
#include <memory>
#include <string>

namespace FsLib
{
    /*
        Stealing this from Switch. I'm not aware of a max path value for 3DS.
        I don't feel like constantly allocating memory when I can just memcpy.
    */
    static constexpr size_t MAX_PATH = 0x301;
    class Path
    {
        public:
            Path(void) = default;
            Path(const Path &P);
            Path(const char16_t *P);
            Path(const uint16_t *P);
            Path(const std::u16string &P);
            Path(std::u16string_view P);
            Path(Path &&P);
            ~Path();
            // Returns if a path is valid for use with FsLib and 3DS FS.
            bool IsValid(void) const;
            // Returns a subpath of length. If length >= m_PathLength... Good job.
            Path SubPath(size_t PathLength) const;
            // Searches for Character in path. Overload uses Begin at the start point. Returns npos when not found.
            size_t FindFirstOf(char16_t Character) const;
            size_t FindFirstOf(char16_t Character, size_t Begin) const;
            // Searches backwards for character in path. Same as above.
            size_t FindLastOf(char16_t Character) const;
            size_t FindLastOf(char16_t Character, size_t Begin) const;
            // Returns entire path as char16_t string array
            const char16_t *CString(void) const;
            // Returns the device name as u16string_view fo r use with FsLib's device map.
            std::u16string_view GetDevice(void) const;
            // Returns the path for use with 3DS FS functions.
            FS_Path GetPath(void) const;
            // Returns the current length of the string.
            size_t GetLength(void) const;

            // These erase and assign.
            Path &operator=(const Path &P);
            Path &operator=(const char16_t *P);
            Path &operator=(const uint16_t *P);
            Path &operator=(const std::u16string &P);
            Path &operator=(std::u16string_view P);
            // These append, check and erase beginning and trailing slashes, and append.
            Path &operator/=(const char16_t *P);
            Path &operator/=(const uint16_t *P);
            Path &operator/=(const std::u16string &P);
            Path &operator/=(std::u16string_view P);
            // These are unchecked appending operators.
            Path &operator+=(const char16_t *P);
            Path &operator+=(const uint16_t *P);
            Path &operator+=(const std::u16string &P);
            Path &operator+=(std::u16string_view P);

            // This is for returning failure for find functions.
            static constexpr uint16_t NotFound = -1;

        private:
            // First is the pointer to path data, second is pointer to the ':'.
            char16_t *m_Path = nullptr;
            const char16_t *m_DeviceEnd = nullptr;
            // First is the actual size of the path buffer, second is the current, actual length of the path.
            uint16_t m_PathSize = 0;
            uint16_t m_PathLength = 0;
            // This allocates memory to hold the string.
            bool AllocatePath(uint16_t PathSize);
            // This frees it.
            void FreePath(void);
    };
    FsLib::Path operator/(const FsLib::Path &Path1, const char16_t *Path2);
    FsLib::Path operator/(const FsLib::Path &Path1, const uint16_t *Path2);
    FsLib::Path operator/(const FsLib::Path &Path1, const std::u16string &Path2);
    FsLib::Path operator/(const FsLib::Path &Path1, std::u16string_view Path2);
    FsLib::Path operator+(const FsLib::Path &Path1, const char16_t *Path2);
    FsLib::Path operator+(const FsLib::Path &Path1, const uint16_t *Path2);
    FsLib::Path operator+(const FsLib::Path &Path1, const std::u16string &Path2);
    FsLib::Path operator+(const FsLib::Path &Path1, std::u16string_view Path2);
} // namespace FsLib
