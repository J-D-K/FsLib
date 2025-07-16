#include "bis_file_system.hpp"
#include "error.hpp"
#include "fslib.hpp"
#include <string>

bool fslib::open_bis_filesystem(std::string_view deviceName, FsBisPartitionId partitionID)
{
    FsFileSystem filesystem;
    const bool openError = error::occurred(fsOpenBisFileSystem(&filesystem, partitionID, ""));
    if (openError) { return false; }

    const bool mapped = fslib::map_file_system(deviceName, &filesystem);
    if (!mapped)
    {
        fsFsClose(&filesystem);
        return false;
    }

    return true;
}
