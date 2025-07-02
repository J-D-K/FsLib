#include "fslib.hpp"
#include <3ds.h>
#include <fstream>
#include <minizip/unzip.h>
#include <minizip/zip.h>
#include <string>

static std::string utf16ToUtf8(std::u16string_view str)
{
    // This isn't the best idea.
    uint8_t utf8Buffer[str.length() + 1] = {0};
    utf16_to_utf8(utf8Buffer, reinterpret_cast<const uint16_t *>(str.data()), str.length() + 1);
    return std::string(reinterpret_cast<const char *>(utf8Buffer));
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
    hidInit();
    gfxInitDefault();
    consoleInit(GFX_TOP, NULL);
    printf("FsLib Test App\n");

    // This is just a barebones run down of functions. This not printing much is a good thing.
    if (!fslib::initialize())
    {
        printf("%s\n", fslib::getErrorString());
        return -1;
    }

    if (!fslib::dev::initializeSDMC())
    {
        printf("%s\n", fslib::getErrorString());
        return -2;
    }

    fslib::Path testPath = u"sdmc://///////test/path/here///";
    fslib::Path concatTest = testPath / u"directory";
    concatTest += u".zip";

    printf("%s\n", utf16ToUtf8(concatTest.cString()).c_str());

    // Just load and print the 3ds dir
    fslib::Directory threedeeessDir(u"sdmc:/3ds");
    if (!threedeeessDir.isOpen())
    {
        printf("FIRST: %s\n", fslib::getErrorString());
    }
    else
    {
        printf("3ds: \n");
        for (uint32_t i = 0; i < threedeeessDir.getCount(); i++)
        {
            printf("\t%s\n", utf16ToUtf8(threedeeessDir.getEntry(i)).c_str());
        }
    }

    // Just check to make sure this stuff works right.
    if (!fslib::directoryExists(u"sdmc:/3ds"))
    {
        printf("sdmc:/3ds doesn't exist?\n");
    }

    if (!fslib::createDirectory(u"sdmc:/fslibTestDir"))
    {
        printf("%s\n", fslib::getErrorString());
    }

    if (!fslib::createDirectoriesRecursively(u"sdmc:/more/and/more/and/more/and/more/and/more/dirs/one/more"))
    {
        printf("%s\n", fslib::getErrorString());
    }

    if (!fslib::renameDirectory(u"sdmc:/fslibTestDir", u"sdmc:/notFsLibTestDir"))
    {
        printf("%s\n", fslib::getErrorString());
    }

    if (!fslib::deleteDirectory(u"sdmc:/notFsLibTestDir"))
    {
        printf("%s\n", fslib::getErrorString());
    }

    if (!fslib::deleteDirectoryRecursively(u"sdmc:/JKSM"))
    {
        printf("%s\n", fslib::getErrorString());
    }

    // Open the homebrew menu and read off its header in bytes and print it. This isn't a thorough test. JKSM will expose any bugs later.
    fslib::File hbmenu(u"sdmc:/boot.3dsx", FS_OPEN_READ);
    if (!hbmenu.isOpen())
    {
        printf("%s\n", fslib::getErrorString());
    }
    else
    {
        char header[5] = {0x00};
        if (hbmenu.read(header, sizeof(uint32_t)) != sizeof(uint32_t))
        {
            printf("%s\n", fslib::getErrorString());
        }
        else
        {
            printf("%s\n", header);
        }
    }

    if (!fslib::createFile(u"sdmc:/randomFile", 0x100000))
    {
        printf("create: %s\n", fslib::getErrorString());
    }

    if (!fslib::fileExists(u"sdmc:/randomFile"))
    {
        printf("fileExists: %s\n", fslib::getErrorString());
    }

    if (!fslib::renameFile(u"sdmc:/randomFile", u"sdmc:/notRandomLovedFile"))
    {
        printf("renameFile: %s\n", fslib::getErrorString());
    }

    if (!fslib::deleteFile(u"sdmc:/notRandomLovedFile"))
    {
        printf("deleteFile: %s\n", fslib::getErrorString());
    }

    printf("Press Start to exit.");
    while (aptMainLoop())
    {
        hidScanInput();
        if (hidKeysDown() & KEY_START)
        {
            break;
        }
        gfxFlushBuffers();
        gfxSwapBuffers();
        gspWaitForVBlank();
    }

    fslib::exit();
    gfxExit();
    hidExit();
    return 0;
}
