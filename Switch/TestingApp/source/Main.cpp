#include "fslib.hpp"
#include <array>
#include <chrono>
#include <cstdarg>
#include <cstdio>
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

void copyDirectoryTo(const fslib::Path &source, const fslib::Path &dest)
{
    fslib::Directory sourceDir{source};
    if (!sourceDir)
    {
        printf("%s\n", fslib::error::get_string());
        return;
    }

    const int64_t count = sourceDir.get_count();
    for (int64_t i = 0; i < count; i++)
    {
        const fslib::Path fullSource{source / sourceDir[i]};
        const fslib::Path fullDest{dest / sourceDir[i]};

        if (sourceDir.is_directory(i))
        {
            const bool dirCreated = fslib::create_directory(fullDest);
            if (!dirCreated) { continue; }
            copyDirectoryTo(fullSource, fullDest);
        }
        else
        {
            fslib::File sourceFile{fullSource, FsOpenMode_Read};
            fslib::File destFile{fullDest, FsOpenMode_Create | FsOpenMode_Write, sourceFile.get_size()};
            if (!sourceFile || !destFile) { continue; }

            print("Copying %s to %s... ", fullSource.full_path(), fullDest.full_path());

            auto buffer            = std::make_unique<byte[]>(0x100000);
            const int64_t fileSize = sourceFile.get_size();

            for (int64_t i = 0; i < fileSize;)
            {
                ssize_t read = sourceFile.read(buffer.get(), 0x100000);
                if (read == -1)
                {
                    print("Read Error: %s\n", fslib::error::get_string());
                    break;
                }

                ssize_t write = destFile.write(buffer.get(), read);
                if (write == -1)
                {
                    print("Write Error: %s\n", fslib::error::get_string());
                    break;
                }
                i += read;
            }
            print("Done!\n");
        }
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

    // This shuts down fs_dev and puts it's own sdmc devop in its place. This only works for files on the SDMC. Everything else should use fslib.
    if (!fslib::dev::initialize_sdmc()) { return -3; }

    // I should error check this but screw it.
    AccountUid user{};
    accountInitialize(AccountServiceType_Application);
    accountGetPreselectedUser(&user);

    consoleInit(NULL);

    PadState gamePad = {0};
    padConfigureInput(1, HidNpadStyleSet_NpadStandard);
    padInitializeDefault(&gamePad);

    print("fslib::TestingApp\n\n");

    const bool openSnap   = fslib::open_account_save_file_system("snap", TITLEID_NEW_SNAP, user);
    const bool snapExists = fslib::directory_exists(SNAP_DIR);
    const bool snapCreate = !snapExists && fslib::create_directory(SNAP_DIR);
    if (!openSnap || (!snapExists && !snapCreate)) { print("Error: %s", fslib::error::get_string()); }
    else { copyDirectoryTo("snap:/", SNAP_DIR); }

    print("\nPress + to exit.\n");

    while (appletMainLoop())
    {
        padUpdate(&gamePad);
        if (padGetButtonsDown(&gamePad) & HidNpadButton_Plus) { break; }
        consoleUpdate(NULL);
    }

    // Just exit stuff.
    accountExit();
    fslib::device::exit();
    fslib::exit();
    consoleExit(NULL);
    return 0;
}
