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
                                                     u"they/go/on/forever/end/Yes/or/maybe/not///////////////";
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

static inline void print_fslib_error() { printf_typed("%s\n", fslib::error::get_string()); }

int main()
{
    gfxInitDefault();
    consoleInit(GFX_TOP, nullptr);
    printf_typed("FsLib Test App\n");

    const bool fslibInit = fslib::initialize();
    if (!fslibInit) { print_fslib_error(); }

    const bool fslibDevInit = fslib::dev::initialize_sdmc();
    if (!fslibDevInit) { print_fslib_error(); }

    fslib::Path testPath = fslib::Path{u"sdmc://///////JKSM////////////"} / u"////////lolol////";
    {
        const std::string device    = utf16ToUtf8(testPath.get_device());
        const std::string printPath = utf16ToUtf8(testPath.full_path());
        printf_typed("%s : %s\n", device.c_str(), printPath.c_str());
    }

    testPath /= u"///////file";
    testPath += u".zip";
    {
        const std::string printPath = utf16ToUtf8(testPath.full_path());
        printf_typed("%s\n", printPath.c_str());
    }

    const bool jksmDir = fslib::directory_exists(u"sdmc:/JKSM");
    if (jksmDir) { printf_typed("JKSM Directory located!"); }
    else { printf_typed("No JKSM dir!\n"); }

    const bool createDir = fslib::create_directory(u"sdmc:/test_dir");
    if (!createDir) { print_fslib_error(); }
    else { printf_typed("Created test_dir!\n"); }

    const fslib::Path reallyLong{PATH_REALLY_LONG};
    {
        const std::string longPrint = utf16ToUtf8(reallyLong.full_path());
        printf_typed("%s\n", longPrint.c_str());
    }

    // const bool createDirs = fslib::create_directory_recursively(PATH_REALLY_LONG);
    // if (!createDirs) { print_fslib_error(); }
    // else { printf_typed("Created long ass chain of folders!"); }

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
