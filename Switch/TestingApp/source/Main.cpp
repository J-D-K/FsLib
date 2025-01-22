#include "fslib.hpp"
#include <array>
#include <chrono>
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <switch.h>
#include <thread>

namespace
{
    // Va buffer size for print function so we have output in real time.
    constexpr int VA_BUFFER_SIZE = 0x1000;
    // These are games on my switch that have the data needed to test stuff.
    // Mario Kart 8 Deluxe has account and device.
    constexpr uint64_t MARIO_KART_8_DELUXE_APPLICATION_ID = 0x0100152000022000;
    // Monster Hunter RISE has BCAT
    constexpr uint64_t MONSTER_HUNTER_RISE_APPLICATION_ID = 0x0100B04011742000;
    // Super Mario Maker 2 has cache
    constexpr uint64_t SUPER_MARIO_MAKER_TWO_APPLICATION_ID = 0x01009B90006DC000;
    // Alternate: Minecraft has it too.
    constexpr uint64_t MINECRAFT_APPLICATION_ID = 0x0100D71004694000;
    // Save ID of test system save mount.
    constexpr uint64_t TARGET_SYSTEM_SAVE = 0x8000000000000011;
    // Array of save data space ids. For some reason All doesn't seem to catch SD card saves?
    constexpr std::array<FsSaveDataSpaceId, 7> SAVE_DATA_SPACE_IDS = {FsSaveDataSpaceId_System,
                                                                      FsSaveDataSpaceId_User,
                                                                      FsSaveDataSpaceId_SdSystem,
                                                                      FsSaveDataSpaceId_Temporary,
                                                                      FsSaveDataSpaceId_SdUser,
                                                                      FsSaveDataSpaceId_ProperSystem,
                                                                      FsSaveDataSpaceId_SafeMode};
} // namespace

// Feels stupid but needed to get actual output in real time on switch.
void print(const char *format, ...)
{
    // va arg the string
    char vaBuffer[VA_BUFFER_SIZE] = {0};
    std::va_list vaList;
    va_start(vaList, format);
    vsnprintf(vaBuffer, VA_BUFFER_SIZE, format, vaList);
    va_end(vaList);

    // Write to console and update so it prints in realtime instead of all at once after everything is done.
    std::fputs(vaBuffer, stdout);
    consoleUpdate(NULL);
}

// This is a recursive directory printing function.
void printDirectory(const fslib::Path &directoryPath)
{
    fslib::Directory dir(directoryPath);
    if (!dir.isOpen())
    {
        print("%s\n", fslib::getErrorString());
        return;
    }

    for (int64_t i = 0; i < dir.getCount(); i++)
    {
        if (dir.isDirectory(i))
        {
            // New path
            fslib::Path newPath = directoryPath / dir[i];

            // Print
            print("\tDIR %s\n", dir[i]);
            // Feed this function the path.
            printDirectory(newPath);
        }
        else
        {
            print("\tFIL %s\n", dir[i]);
        }
    }
}

int main(void)
{
    // Initialize fslib
    if (!fslib::initialize())
    {
        return -1;
    }

    // This goes here if you're using romfs cause I don't feel like writing something for that.
    // if(R_FAILED(romfsInit()))
    // {
    //     return -2;
    // }


    // This shuts down fs_dev and puts it's own sdmc devop in its place. This only works for files on the SDMC. Everything else should use fslib.
    if (!fslib::dev::initializeSDMC())
    {
        return -3;
    }

    if (!fslib::device::initialize())
    {
        return -4;
    }

    fslib::File logFile("sdmc:/fslib.log", FsOpenMode_Create | FsOpenMode_Write);

    // Account is needed for now.
    accountInitialize(AccountServiceType_Application);

    // Default libnx console.
    consoleInit(NULL);

    // Game pad stuff.
    PadState gamePad;
    padConfigureInput(1, HidNpadStyleSet_NpadStandard);
    padInitializeDefault(&gamePad);

    if (!fslib::device::sdIsInserted())
    {
        print("Strong independent game don't need no sd card detected.\n");
    }
    else
    {
        print("I bet your SD card is fake :^).\n");
    }

    if (!fslib::device::gameCardIsInserted())
    {
        print("Why would I expect to find a game card in a hacked switch?\n");
    }
    else
    {
        print("Sweet. Free game card cert. Gimme a chance while I upload that... ");
        std::this_thread::sleep_for(std::chrono::seconds(3));
        print("Done! Wew. Thanks.\n");
    }

    // Get my current user to debug this crap.
    AccountUid myID;
    accountGetPreselectedUser(&myID);

    // I'm testin stuff. I actually don't know what order the AccountUID is supposed to be in. I don't get why libnx went from u128 to the struct...
    logFile.writef("Account ID from account service: %016llX%016llX.\n", myID.uid[0], myID.uid[1]);

    // Current save ID number.
    int currentSaveID = 0;
    // Number of successfully mounted saves.
    int totalSavesOpened = 0;
    // Save data info reader and fslib device testing thing, I guess.
    for (int i = 0; i < 7; i++)
    {
        fslib::SaveInfoReader saveInfoReader(SAVE_DATA_SPACE_IDS[i]);
        if (!saveInfoReader.isOpen())
        {
            logFile.writef("%s\n", fslib::getErrorString());
            continue;
        }

        while (saveInfoReader.read())
        {
            // fslib requires a device name on mount. This is actually bad since fslib uses string_view and once this is out of scope, it's invalid. To do: Switch to regular string instead?
            constexpr std::string_view SAVE_MOUNT = "save";
            constexpr std::string_view SAVE_ROOT = "save:/";

            FsSaveDataInfo &saveInfo = saveInfoReader.getSaveDataInfo();

            logFile.writef("Account ID: %016llX%016llX\n", saveInfo.uid.uid[0], saveInfo.uid.uid[1]);

            if (!fslib::openSaveFileSystemWithSaveDataInfo(SAVE_MOUNT, saveInfo))
            {
                // Print error and continue.
                logFile.writef("%s: %016llX\n", fslib::getErrorString(), saveInfoReader.getSaveDataInfo().application_id);
                continue;
            }
            // // Print to see
            // print("%s:\n", SAVE_ROOT.data());
            // printDirectory(SAVE_ROOT);

            // Inc total
            totalSavesOpened++;
        }
    }

    // Print total to see what happened.
    logFile.writef("Total save filesystems opened: %i\n", totalSavesOpened);

    print("Press + to exit.");

    while (appletMainLoop())
    {
        padUpdate(&gamePad);
        if (padGetButtonsDown(&gamePad) & HidNpadButton_Plus)
        {
            break;
        }
        consoleUpdate(NULL);
    }
    fslib::device::exit();
    fslib::exit();
    consoleExit(NULL);
    return 0;
}
