#include "fslib.hpp"

#include <array>
#include <chrono>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <memory>

using byte = unsigned char;

namespace
{
    // Va buffer size for print function so we have output in real time.
    constexpr int VA_BUFFER_SIZE = 0x1000;

    // This is for debugging something.
    constexpr uint64_t TITLEID_NEW_SNAP = 0x0100F4300BF2C000;
} // namespace

// Feels stupid but needed to get actual output in real time on switch.
void print(const char *format, ...)
{
    std::array<char, VA_BUFFER_SIZE> vaBuffer{};

    std::va_list vaList{};
    va_start(vaList, format);
    std::vsnprintf(vaBuffer.data(), VA_BUFFER_SIZE, format, vaList);
    va_end(vaList);

    // I got bored and decided to make this type character by character for fun.
    size_t length = std::char_traits<char>::length(vaBuffer.data());
    for (size_t i = 0; i < length; i++)
    {
        std::fputc(vaBuffer.at(i), stdout);
        consoleUpdate(NULL);
    }
}

int main()
{
    static const char *SNAP_DIR = "sdmc:/snapTest";

    // Initialize fslib
    if (!fslib::initialize()) { return -1; }

    // This goes here if you're using romfs cause I don't feel like writing something for that.
    // if(R_FAILED(romfsInit()))
    // {
    //     return -2;
    // }

    // This shuts down fs_dev and puts it's own sdmc devop in its place. This only works for files on the SDMC. Everything else
    // should use fslib.
    if (!fslib::dev::initialize_sdmc()) { return -3; }

    consoleInit(nullptr);

    FILE *test = fopen("sdmc:/test.txt", "w");
    if (!test) { return -1; }

    fseek(test, 128, SEEK_SET);
    fputs("First message.", test);
    print("%s\n", fslib::error::get_string());
    fseek(test, -32, SEEK_SET);
    fputs("Second message.", test);
    print("%s\n", fslib::error::get_string());
    fclose(test);

    PadState padState{};
    padConfigureInput(1, HidNpadStyleSet_NpadStandard);
    padInitializeDefault(&padState);

    print("\nPress + to exit.\n");

    while (appletMainLoop())
    {
        padUpdate(&padState);
        if (padGetButtonsDown(&padState) & HidNpadButton_Plus) { break; }
        consoleUpdate(NULL);
    }

    // Just exit stuff.
    accountExit();
    fslib::device::exit();
    fslib::exit();
    consoleExit(NULL);
    return 0;
}
