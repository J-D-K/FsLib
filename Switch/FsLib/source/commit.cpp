#include "commit.hpp"

#include "error.hpp"
#include "fslib.hpp"

bool fslib::commit_data_to_file_system(std::string_view device)
{
    FsFileSystem *filesystem{};
    const bool found = fslib::get_file_system_by_device_name(device, &filesystem);
    if (!found) { return false; }

    const bool commitError = error::occurred(fsFsCommit(filesystem));
    if (commitError) { return false; }

    return true;
}