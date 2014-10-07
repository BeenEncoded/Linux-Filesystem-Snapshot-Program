#ifndef SNAPSHOT_FILE_LOADERS_HPP_INCLUDED
#define SNAPSHOT_FILE_LOADERS_HPP_INCLUDED
#include <string>
#include <unordered_map>

#include "snapshot_class.hpp"

/** 
 * Loads snapshot information.  This is specific to the filesystem, 
 * and is put here as a means to centralize all filesystem operations
 * regarding snapshot files to one, obvious location.
 */
namespace snapshot
{
    std::string snapshot_folder();
    bool load_snapshot(snapshot::snapshot_data&, const std::string&);
    bool load_header(snapshot::snapshot_data&, const std::string&);
    unsigned long long new_snapshot_id();
    std::unordered_map<unsigned long long, std::string> list_ids(const std::string&);
}

#endif