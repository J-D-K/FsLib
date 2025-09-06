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

class timer
{
    public:
        timer()
            : m_start(std::chrono::high_resolution_clock::now()) {};

        ~timer()
        {
            auto end = std::chrono::high_resolution_clock::now();
            print("Timer: %ums\n", std::chrono::duration_cast<std::chrono::microseconds>(end - m_start));
        }

    private:
        std::chrono::system_clock::time_point m_start{};
};

int main()
{
    static constexpr int BUFFER_SIZE = 0x80;
    static const char *SNAP_DIR      = "sdmc:/snapTest";

    if (!fslib::is_initialized()) { return -1; }

    // This goes here if you're using romfs cause I don't feel like writing something for that.
    // if(R_FAILED(romfsInit()))
    // {
    //     return -2;
    // }

    // This shuts down fs_dev and puts it's own sdmc devop in its place. This only works for files on the SDMC. Everything else
    // should use fslib.
    if (!fslib::dev::initialize_sdmc()) { return -3; }

    consoleInit(nullptr);

    PadState padState{};
    padConfigureInput(1, HidNpadStyleSet_NpadStandard);
    padInitializeDefault(&padState);

    FsTimeStampRaw fslibStamp{};
    FsTimeStampRaw rawStamp;

    FsFileSystem *sdmc{};
    fslib::get_file_system_by_device_name("sdmc", &sdmc);

    const fslib::Path hbmenu{"sdmc:/hbmenu.nro"};
    const bool fslibGet = fslib::get_file_timestamp(hbmenu, fslibStamp);
    const bool rawGet   = R_SUCCEEDED(fsFsGetFileTimeStampRaw(sdmc, hbmenu.get_path(), &rawStamp));
    if (!fslibGet || !rawGet) { return -2; }

    const std::time_t fslibCreated = static_cast<std::time_t>(fslibStamp.created);
    const std::time_t rawCreated   = static_cast<std::time_t>(rawStamp.created);

    if (fslibCreated != rawCreated) { print("Stamp mismatch."); }

    print("%lli\n", fslibStamp.created);

    char fslibBuffer[BUFFER_SIZE] = {0};
    char rawBuffer[BUFFER_SIZE]   = {0};

    const std::tm *fslibTm = std::localtime(&fslibCreated);
    const std::tm *rawTm   = std::localtime(&rawCreated);

    std::strftime(fslibBuffer, BUFFER_SIZE, "%c", fslibTm);
    std::strftime(rawBuffer, BUFFER_SIZE, "%c", rawTm);

    print("fslib: %s\nRaw: %s\n", fslibBuffer, rawBuffer);

    print("\nPress + to Exit");
    while (appletMainLoop())
    {
        padUpdate(&padState);
        if (padGetButtonsDown(&padState) & HidNpadButton_Plus) { break; }
        consoleUpdate(NULL);
    }

    // Just exit stuff.
    accountExit();
    consoleExit(NULL);
    return 0;
}
