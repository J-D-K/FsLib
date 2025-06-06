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

    // This is a test thing so I can debug what's wrong with fslib::dev
    constexpr uint64_t APPLICATION_ID_FINAL_FANTASY = 0x01006C300E9F0000;

    /// @brief This is the mount point string for ^.
    constexpr std::string_view STRING_FF_MOUNT = "ff1";

    /// @brief This is the root directory of the FF1 save data on my Switch. Maybe yours too.
    constexpr std::string_view STRING_FF_SAVE_ROOT = "ff1:/";
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

static void copy_directory_to_zip(const fslib::Path &source, zipFile destination)
{
    // Start by opening the source directory.
    fslib::Directory sourceDir(source);
    if (!sourceDir)
    {
        print("Error opening directory for reading!\n");
        return;
    }

    print("%s count: %i\n", source.c_string(), sourceDir.get_count());

    // Loop and copy files whole. This doesn't need to be generic and complex like JKSV.
    for (int64_t i = 0; i < sourceDir.get_count(); i++)
    {
        // Just do this here since both conditions can use it.
        fslib::Path fullSource = source / sourceDir[i];

        // If it's a directory, just feed this function the new arguments.
        if (sourceDir.is_directory(i))
        {
            copy_directory_to_zip(fullSource, destination);
        }
        else
        {
            // Try to open it.
            fslib::File sourceFile(fullSource, FsOpenMode_Read);
            if (!sourceFile)
            {
                print("Error opening %s for reading!\n", fullSource.c_string());
                continue;
            }

            // Get the file's name/path.
            const char *zipFilePath = std::strchr(fullSource.c_string(), '/') + 1;

            //  We're not going to worry about the date/time data for this.
            int zipError = zipOpenNewFileInZip64(destination,
                                                 zipFilePath,
                                                 NULL,
                                                 NULL,
                                                 0,
                                                 NULL,
                                                 0,
                                                 NULL,
                                                 Z_DEFLATED,
                                                 Z_DEFAULT_COMPRESSION,
                                                 0);
            if (zipError != ZIP_OK)
            {
                print("Error creating %s in ZIP!\n", zipFilePath);
                continue;
            }

            // Don't need to call this function every loop.
            int64_t fileSize = sourceFile.get_size();

            // Using a small buffer on purpose to test this. There's something off with fslib::dev...
            std::unique_ptr<unsigned char[]> filebuffer = std::make_unique<unsigned char[]>(0x1000);

            for (int64_t i = 0; i < fileSize;)
            {
                ssize_t readSize = sourceFile.read(filebuffer.get(), 0x1000);
                if (readSize == -1)
                {
                    print("Error reading from file: %s", fslib::get_error_string());
                    break;
                }

                zipError = zipWriteInFileInZip(destination, filebuffer.get(), readSize);
                if (zipError != ZIP_OK)
                {
                    print("Error writing to zip: %i.\n", zipError);
                    break;
                }

                i += readSize;
            }

            // Finish. There's something funky going on here.
            zipCloseFileInZip(destination);
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
    if (!fslib::dev::initialize_sdmc())
    {
        return -3;
    }

    // This should be logged, but screw it
    AccountUid userID;
    accountInitialize(AccountServiceType_Application);
    accountGetPreselectedUser(&userID);

    // Default libnx console.
    consoleInit(NULL);

    // Game pad stuff.
    PadState gamePad;
    padConfigureInput(1, HidNpadStyleSet_NpadStandard);
    padInitializeDefault(&gamePad);

    if (!fslib::open_account_save_file_system(STRING_FF_MOUNT, APPLICATION_ID_FINAL_FANTASY, userID))
    {
        // Let's hope this stays on screen long enough to catch it.
        print("Error opening FINAL FANTASY save!");
        return -5;
    }

    // Try to open the zip on root.
    zipFile testZip = zipOpen64("sdmc:/ff1.zip", APPEND_STATUS_CREATE);
    if (!testZip)
    {
        print("Error creating test zip!");
        return -6;
    }

    copy_directory_to_zip(STRING_FF_SAVE_ROOT, testZip);

    zipClose(testZip, NULL);

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
