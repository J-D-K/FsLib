#include "fslib.hpp"
#include "string.hpp"

extern std::string g_fslibErrorString;

bool fslib::openSaveData(std::u16string_view deviceName)
{
    FS_Archive archive;
    Result fsError = FSUSER_OpenArchive(&archive, ARCHIVE_SAVEDATA, {PATH_EMPTY, 0x00, NULL});
    if (R_FAILED(fsError))
    {
        g_fslibErrorString = string::getFormattedString("Error open save data archive: 0x%08X.", fsError);
        return false;
    }

    if (!fslib::mapArchiveToDevice(deviceName, archive))
    {
        FSUSER_CloseArchive(archive);
        return false;
    }

    return true;
}

bool fslib::openExtData(std::u16string_view deviceName, uint32_t extDataID)
{
    FS_Archive archive;
    uint32_t binaryData[] = {MEDIATYPE_SD, extDataID, 0x00000000};
    FS_Path path = {.type = PATH_BINARY, .size = 0x0C, .data = binaryData};

    Result fsError = FSUSER_OpenArchive(&archive, ARCHIVE_EXTDATA, path);
    if (R_FAILED(fsError))
    {
        g_fslibErrorString = string::getFormattedString("Error opening extdata archive: 0x%08X.", fsError);
        return false;
    }

    if (!fslib::mapArchiveToDevice(deviceName, archive))
    {
        FSUSER_CloseArchive(archive);
        return false;
    }

    return true;
}

bool fslib::openSharedExtData(std::u16string_view deviceName, uint32_t sharedExtDataID)
{
    FS_Archive archive;
    uint32_t binaryData[] = {MEDIATYPE_NAND, sharedExtDataID, 0x00480000};
    FS_Path path = {.type = PATH_BINARY, .size = 0x0C, .data = binaryData};

    Result fsError = FSUSER_OpenArchive(&archive, ARCHIVE_SHARED_EXTDATA, path);
    if (R_FAILED(fsError))
    {
        g_fslibErrorString = string::getFormattedString("Error opening shared extdata: 0x%08X.", fsError);
        return false;
    }

    if (!fslib::mapArchiveToDevice(deviceName, archive))
    {
        FSUSER_CloseArchive(archive);
        return false;
    }

    return true;
}

// This is basically identical to OpenExtData, but the archive ID is different.
bool fslib::openBossExtData(std::u16string_view deviceName, uint32_t extDataID)
{
    FS_Archive archive;
    uint32_t binaryData[] = {MEDIATYPE_SD, extDataID, 0x00000000};
    FS_Path path = {.type = PATH_BINARY, .size = 0x0C, .data = binaryData};

    Result fsError = FSUSER_OpenArchive(&archive, ARCHIVE_BOSS_EXTDATA, path);
    if (R_FAILED(fsError))
    {
        g_fslibErrorString = string::getFormattedString("Error opening BOSS extdata: 0x%08X.", fsError);
        return false;
    }

    if (!fslib::mapArchiveToDevice(deviceName, archive))
    {
        FSUSER_CloseArchive(archive);
        return false;
    }

    return true;
}

bool fslib::openSystemSaveData(std::u16string_view deviceName, uint32_t uniqueID)
{
    FS_Archive archive;
    uint32_t binaryData[] = {MEDIATYPE_NAND, 0x00020000 | uniqueID};
    FS_Path path = {.type = PATH_BINARY, .size = 0x08, .data = binaryData};

    Result fsError = FSUSER_OpenArchive(&archive, ARCHIVE_SYSTEM_SAVEDATA, path);
    if (R_FAILED(fsError))
    {
        g_fslibErrorString = string::getFormattedString("Error opening system save data: 0x%08X.", fsError);
        return false;
    }

    if (!fslib::mapArchiveToDevice(deviceName, archive))
    {
        FSUSER_CloseArchive(archive);
        return false;
    }

    return true;
}

bool fslib::openSystemModuleSaveData(std::u16string_view deviceName, uint32_t uniqueID)
{
    FS_Archive archive;
    uint32_t binaryData[] = {MEDIATYPE_NAND, 0x00010000 | uniqueID};
    FS_Path path = {.type = PATH_BINARY, .size = 0x08, .data = binaryData};

    Result fsError = FSUSER_OpenArchive(&archive, ARCHIVE_SYSTEM_SAVEDATA, path);
    if (R_FAILED(fsError))
    {
        g_fslibErrorString = string::getFormattedString("Error opening system module save data: 0x%08X.", uniqueID);
        return false;
    }

    if (!fslib::mapArchiveToDevice(deviceName, archive))
    {
        FSUSER_CloseArchive(archive);
        return false;
    }

    return true;
}


bool fslib::openGameCardSaveData(std::u16string_view deviceName)
{
    FS_Archive archive;
    Result fsError = FSUSER_OpenArchive(&archive, ARCHIVE_GAMECARD_SAVEDATA, {PATH_EMPTY, 0x00, NULL});
    if (R_FAILED(fsError))
    {
        g_fslibErrorString = string::getFormattedString("Error open game card save data: 0x%08X.", fsError);
        return false;
    }

    if (!fslib::mapArchiveToDevice(deviceName, archive))
    {
        FSUSER_CloseArchive(archive);
        return false;
    }

    return true;
}

bool fslib::openUserSaveData(std::u16string_view deviceName, FS_MediaType mediaType, uint32_t lowerID, uint32_t upperID)
{
    FS_Archive archive;
    uint32_t binaryData[] = {mediaType, lowerID, upperID};
    FS_Path path = {.type = PATH_BINARY, .size = 0x0C, .data = binaryData};
    Result fsError = FSUSER_OpenArchive(&archive, ARCHIVE_USER_SAVEDATA, path);
    if (R_FAILED(fsError))
    {
        g_fslibErrorString = string::getFormattedString("Error opening user save data: 0x%08X.", fsError);
        return false;
    }

    if (!fslib::mapArchiveToDevice(deviceName, archive))
    {
        FSUSER_CloseArchive(archive);
        return false;
    }

    return true;
}
