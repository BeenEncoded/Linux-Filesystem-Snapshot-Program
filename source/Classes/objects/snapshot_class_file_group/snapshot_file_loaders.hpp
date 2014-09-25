#ifndef SNAPSHOT_FILE_LOADERS_HPP_INCLUDED
#define SNAPSHOT_FILE_LOADERS_HPP_INCLUDED
#include <string>
#include <vector>
#include <unordered_set>
#include <iostream>

#include "snapshot_class.hpp"

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
    bool load_snapshot(snapshot::snapshot_data&, const std::string&);
    std::unordered_set<std::string> load_paths_from_snapshot(const snapshot::snapshot_data&);
    unsigned long long new_snapshot_id();
    std::vector<snapshot::snapshot_data> list_snapshot_info(const std::string&);
    std::istream& retrieve_info(std::istream&, snapshot::snapshot_data&);
    
}

#endif