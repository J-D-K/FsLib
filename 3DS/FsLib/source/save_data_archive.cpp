#include "EmptyPath.hpp"
#include "fslib.hpp"
#include "string.hpp"

#include <array>

bool fslib::open_save_data(std::u16string_view deviceName)
{
    FS_Archive archive{};
    const bool openError = error::libctru(FSUSER_OpenArchive(&archive, ARCHIVE_SAVEDATA, EMPTY_PATH));
    if (openError) { return false; }

    return fslib::map_archive(deviceName, archive);
}

bool fslib::open_extra_data(std::u16string_view deviceName, uint32_t extraDataID)
{
    FS_Archive archive{};
    const std::array<uint32_t, 3> binaryData = {MEDIATYPE_SD, extraDataID, 0x00000000};
    const FS_Path path                       = {.type = PATH_BINARY, .size = 0x0C, .data = binaryData.data()};

    const bool openError = error::libctru(FSUSER_OpenArchive(&archive, ARCHIVE_EXTDATA, path));
    if (openError) { return false; }

    return fslib::map_archive(deviceName, archive);
}

bool fslib::open_shared_extra_data(std::u16string_view deviceName, uint32_t extraDataID)
{
    FS_Archive archive{};
    const std::array<uint32_t, 3> binaryData = {MEDIATYPE_NAND, extraDataID, 0x0048000};
    const FS_Path path                       = {.type = PATH_BINARY, .size = 0x0C, .data = binaryData.data()};

    const bool openError = error::libctru(FSUSER_OpenArchive(&archive, ARCHIVE_SHARED_EXTDATA, path));
    if (openError) { return false; }

    return fslib::map_archive(deviceName, archive);
}

bool fslib::open_boss_extra_data(std::u16string_view deviceName, uint32_t extraDataID)
{
    FS_Archive archive{};
    const std::array<uint32_t, 3> binaryData = {MEDIATYPE_SD, extraDataID, 0x00000000};
    const FS_Path path                       = {.type = PATH_BINARY, .size = 0x0C, .data = binaryData.data()};

    const bool openError = error::libctru(FSUSER_OpenArchive(&archive, ARCHIVE_BOSS_EXTDATA, path));
    if (openError) { return false; }

    return fslib::map_archive(deviceName, archive);
}

bool fslib::open_system_save_data(std::u16string_view deviceName, uint32_t uniqueID)
{
    FS_Archive archive{};
    const std::array<uint32_t, 2> binaryData = {MEDIATYPE_NAND, 0x00020000 | uniqueID};
    const FS_Path path                       = {.type = PATH_BINARY, .size = 0x08, .data = binaryData.data()};

    const bool openError = error::libctru(FSUSER_OpenArchive(&archive, ARCHIVE_SYSTEM_SAVEDATA, path));
    if (openError) { return false; }

    return fslib::map_archive(deviceName, archive);
}

bool fslib::open_system_module_save_data(std::u16string_view deviceName, uint32_t uniqueID)
{
    FS_Archive archive{};
    const std::array<uint32_t, 2> binaryData = {MEDIATYPE_NAND, 0x00010000 | uniqueID};
    const FS_Path path                       = {.type = PATH_BINARY, .size = 0x08, .data = binaryData.data()};

    const bool openError = error::libctru(FSUSER_OpenArchive(&archive, ARCHIVE_SYSTEM_SAVEDATA, path));
    if (openError) { return false; }

    return fslib::map_archive(deviceName, archive);
}

bool fslib::open_gamecard_save_data(std::u16string_view deviceName)
{
    FS_Archive archive{};
    const bool openError = error::libctru(FSUSER_OpenArchive(&archive, ARCHIVE_GAMECARD_SAVEDATA, EMPTY_PATH));
    if (openError) { return false; }

    return fslib::map_archive(deviceName, archive);
}

bool fslib::open_user_save_data(std::u16string_view deviceName, FS_MediaType mediaType, uint64_t titleID)
{
    const uint32_t upperID = titleID >> 32 & 0xFFFFFFFF;
    const uint32_t lowerID = titleID & 0xFFFFFFFF;

    FS_Archive archive{};
    const std::array<uint32_t, 3> binaryData = {mediaType, lowerID, upperID};
    const FS_Path path                       = {.type = PATH_BINARY, .size = 0x0C, .data = binaryData.data()};
    const bool openError                     = error::libctru(FSUSER_OpenArchive(&archive, ARCHIVE_USER_SAVEDATA, path));
    if (openError) { return false; }

    return fslib::map_archive(deviceName, archive);
}
