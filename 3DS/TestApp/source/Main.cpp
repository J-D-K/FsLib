#include "fslib.hpp"

#include <3ds.h>
#include <array>
#include <chrono>
#include <cstdarg>
#include <string>
#include <thread>

namespace
{
    constexpr size_t SIZE_VA_BUFFER = 0x400;
}

static std::string utf16ToUtf8(std::u16string_view str)
{
    // This isn't the best idea.
    uint8_t utf8Buffer[str.length() + 1] = {0};
    utf16_to_utf8(utf8Buffer, reinterpret_cast<const uint16_t *>(str.data()), str.length() + 1);
    return std::string(reinterpret_cast<const char *>(utf8Buffer));
}

void printf_typed(const char *format, ...)
{
    std::array<char, SIZE_VA_BUFFER> vaBuffer = {0};

    std::va_list vaList{};
    va_start(vaList, format);
    std::vsnprintf(vaBuffer.data(), SIZE_VA_BUFFER, format, vaList);
    va_end(vaList);

    const int length = std::char_traits<char>::length(vaBuffer.data());
    for (int i = 0; i < length; i++)
    {
        std::fputc(vaBuffer[i], stdout);
        gfxFlushBuffers();
        gfxSwapBuffers();
        gspWaitForVBlank();
    }
}

// This will completely cut out archive_dev.
extern "C"
{
    u32 __stacksize__ = 0x20000;
    void __appInit()
    {
        srvInit();
        aptInit();
        fsInit();
        hidInit();
    }

    void __appExit()
    {
        hidExit();
        fsExit();
        aptExit();
        srvExit();
    }
}

int main()
{
    gfxInitDefault();
    consoleInit(GFX_TOP, nullptr);
    printf_typed("FsLib Test App\n");

    const bool fslibInit = fslib::initialize();
    if (!fslibInit) { printf_typed("%s\n", fslib::error::get_string()); }

    const bool fslibDevInit = fslib::dev::initialize_sdmc();
    if (!fslibDevInit) { printf_typed("%s\n", fslib::error::get_string()); }

    fslib::Path testPath = fslib::Path{u"sdmc://///////JKSM////////////"} / u"////////lolol////";
    {
        const std::string printPath = utf16ToUtf8(testPath.full_path());
        printf_typed("%s\n", printPath.c_str());
    }

    testPath /= u"///////file";
    testPath += u".zip";
    {
        const std::string printPath = utf16ToUtf8(testPath.full_path());
        printf_typed("%s\n", printPath.c_str());
    }

    printf_typed("Press Start to exit.");
    while (aptMainLoop())
    {
        hidScanInput();
        if (hidKeysDown() & KEY_START) { break; }
        gfxFlushBuffers();
        gfxSwapBuffers();
        gspWaitForVBlank();
    }

    fslib::exit();
    gfxExit();
    hidExit();
    return 0;
}
