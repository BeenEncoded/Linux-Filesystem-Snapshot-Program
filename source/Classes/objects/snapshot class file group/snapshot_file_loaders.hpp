#ifndef SNAPSHOT_FILE_LOADERS_HPP_INCLUDED
#define SNAPSHOT_FILE_LOADERS_HPP_INCLUDED
#include <string>
#include <vector>

/** 
 * Loads snapshot information.  This is specific to the filesystem, 
 * and is put here as a means to centralize all filesystem operations
 * regarding snapshot files to one, obvious location.
 */
namespace snapshot
{
    std::string snapshot_folder();
    std::vector<std::string> paths_of_extension(const std::string&, const std::string&);
    std::string snapshot_path(const unsigned long long&);
    
}

#endif