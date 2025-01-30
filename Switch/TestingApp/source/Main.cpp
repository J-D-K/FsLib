#include "fslib.hpp"
#include <array>
#include <chrono>
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <minizip/unzip.h>
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
    // Using this path to try to debug something.
    const char *TEST_ZIP_PATH = "sdmc:/switch/JKSV/MONSTER HUNTER RISE/JK - 2025-01-28_15-16-03.zip";
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

    // Default libnx console.
    consoleInit(NULL);

    // Game pad stuff.
    PadState gamePad;
    padConfigureInput(1, HidNpadStyleSet_NpadStandard);
    padInitializeDefault(&gamePad);

    fslib::File testBoolOp("sdmc:/hbmenu.nro", FsOpenMode_Read);
    if (testBoolOp)
    {
        print("File opened!\n");
    }
    else
    {
        print("File not opened.\n");
    }

    fslib::File testBoolOpB("sdmc:/bullshit_file_name.txt", FsOpenMode_Read);
    if (testBoolOpB)
    {
        print("File opened?\n");
    }
    else
    {
        print("File not opened.\n");
    }

    // unzFile testOpen = unzOpen64(TEST_ZIP_PATH);
    // if (!testOpen)
    // {
    //     print("What the fuck?\n");
    // }

    // unzClose(testOpen);

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
