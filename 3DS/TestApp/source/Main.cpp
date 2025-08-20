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

    constexpr std::u16string_view PATH_REALLY_LONG = u"sdmc:////////////really/really/long/path/of/directories/that/never/seem/"
                                                     u"to/end/maybe/it/ends/here/got/ya/I/was/just/kidding/"
                                                     u"they/go/on/forever/end/Yes/or/maybe/not";
}

static std::string utf16_to_utf8(std::u16string_view str)
{
    const size_t stringLength   = str.length();
    const size_t bufferLength   = stringLength + 1;
    const uint16_t *castPointer = reinterpret_cast<const uint16_t *>(str.data());

    uint8_t utf8Buffer[bufferLength] = {0};
    utf16_to_utf8(utf8Buffer, castPointer, stringLength);
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

static inline void print_fslib_error() { printf_typed("%s\n", fslib::error::get_string()); }

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
    if (!fslibInit) { print_fslib_error(); }

    const bool fslibDevInit = fslib::dev::initialize_sdmc();
    if (!fslibDevInit) { print_fslib_error(); }

    for (const FS_DirectoryEntry &entry : fslib::DirectoryIterator(u"sdmc:/3ds"))
    {
        const char16_t *castPointer = reinterpret_cast<const char16_t *>(entry.name);
        const std::string entryUTF8 = utf16_to_utf8(castPointer);

        printf_typed("%s\n", entryUTF8.c_str());
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
