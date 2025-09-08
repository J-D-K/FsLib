#pragma once
#include "FileSystem.hpp"

#include <memory>
#include <string>
#include <switch.h>
#include <unordered_map>

class FsLibCore final
{
    public:
        /// @brief Constructor. Mounts the sdmc.
        FsLibCore();

        // None of these shenanigans.
        FsLibCore(const FsLibCore &)            = delete;
        FsLibCore &operator=(const FsLibCore &) = delete;
        FsLibCore(FsLibCore &&)                 = delete;
        FsLibCore &operator=(FsLibCore &&)      = delete;

        /// @brief Basically returns whether or not the SD card was opened successfully.
        bool is_initialized() const noexcept;

        /// @brief Maps a filesystem to the device name passed.
        bool map_file_system(std::string_view deviceName, FsFileSystem &filesystem);

        /// @brief Searchs the map for a device matching deviceName. Returns true on success.
        bool get_file_system_by_device_name(std::string_view deviceName, FsFileSystem **filesystem);

        /// @brief Closes the filesystem matching device name (if it's found.)
        bool close_file_system(std::string_view deviceName);

    private:
        // clang-format off
        struct StringViewHash
        {
            using is_transparent = void;
            size_t operator()(std::string_view view) const noexcept { return std::hash<std::string_view>{}(view); }
        };

        struct StringViewEquals
        {
            using is_transparent = void;
            bool operator()(std::string_view viewA, std::string_view viewB) const noexcept { return viewA == viewB; }
        };
        // clang-format on

        /// @brief Stores whether or not fslib successfully initialized.
        bool m_initialized{};

        /// @brief Underlying device map.
        std::unordered_map<std::string, FileSystem, FsLibCore::StringViewHash, FsLibCore::StringViewEquals> m_deviceMap{};
};