#include "bis_file_system.hpp"
#include "error.hpp"
#include "fslib.hpp"
#include <string>

bool fslib::open_bis_filesystem(std::string_view deviceName, FsBisPartitionId partitionID)
{
    FsFileSystem filesystem;

    if (error::occurred(fsOpenBisFileSystem(&filesystem, partitionID, "")))
    {
        return false;
    }

    if (!fslib::map_file_system(deviceName, &filesystem))
    {
        fsFsClose(&filesystem);
        return false;
    }

    return true;
}
