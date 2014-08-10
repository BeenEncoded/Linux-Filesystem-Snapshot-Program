#ifndef SNAPSHOT_FILE_LOADERS_HPP_INCLUDED
#define SNAPSHOT_FILE_LOADERS_HPP_INCLUDED
#include <string>
#include <vector>

#include "snapshot_class.hpp"

/** 
 * Loads snapshot information.  This is specific to the filesystem, 
 * and is put here as a means to centralize all filesystem operations
 * regarding snapshot files to one, obvious location.
 */
namespace snapshot
{
    std::string snapshot_folder();
    std::vector<std::string> get_files(const std::string&);
}

#endif