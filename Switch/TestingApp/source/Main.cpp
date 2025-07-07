#include "fslib.hpp"
#include <array>
#include <chrono>
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <minizip/unzip.h>
#include <minizip/zip.h>
#include <switch.h>
#include <thread>

namespace
{
    // Va buffer size for print function so we have output in real time.
    constexpr int VA_BUFFER_SIZE = 0x1000;
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

    // I got bored and decided to make this type character by character for fun.
    size_t length = std::char_traits<char>::length(vaBuffer);
    for (size_t i = 0; i < length; i++)
    {
        std::fputc(vaBuffer[i], stdout);
        consoleUpdate(NULL);
    }
}

int main()
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
    if (!fslib::dev::initialize_sdmc())
    {
        return -3;
    }

    consoleInit(NULL);

    PadState gamePad = {0};
    padConfigureInput(1, HidNpadStyleSet_NpadStandard);
    padInitializeDefault(&gamePad);

    print("fslib::TestingApp\n\n");

    while (appletMainLoop())
    {
        padUpdate(&gamePad);
        if (padGetButtonsDown(&gamePad) & HidNpadButton_Plus)
        {
            break;
        }
        consoleUpdate(NULL);
    }

    // Just exit stuff.
    accountExit();
    fslib::device::exit();
    fslib::exit();
    consoleExit(NULL);
    return 0;
}
