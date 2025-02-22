#include "bis_file_system.hpp"
#include "fslib.hpp"
#include "string.hpp"
#include <string>

extern std::string g_fslibErrorString;

bool fslib::open_bis_filesystem(std::string_view deviceName, FsBisPartitionId partitionID)
{
    FsFileSystem fileSystem;
    Result fsError = fsOpenBisFileSystem(&fileSystem, partitionID, "");
    if (R_FAILED(fsError))
    {
        g_fslibErrorString = string::get_formatted_string("Error opening BIS filesystem: 0x%08.", fsError);
        return false;
    }

    if (!fslib::map_file_system(deviceName, &fileSystem))
    {
        fsFsClose(&fileSystem);
        return false;
    }
    return true;
}
