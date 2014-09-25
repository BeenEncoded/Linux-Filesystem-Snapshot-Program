#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <boost/filesystem.hpp>
#include <unordered_set>

#include "snapshot_file_loaders.hpp"
#include "filesystem.hpp"
#include "global_defines.hpp"
#include "common.hpp"

using boost::filesystem::current_path;
using fsys::is_file;
using fsys::is_folder;
using fsys::is_symlink;
using fsys::tree_riterator_class;
using std::string;
using fsys::pref_slash;
using std::unordered_set;
using std::vector;
using snapshot::snapshot_data;

namespace
{
    string file_extension(const std::string&);
    unordered_set<unsigned long long> extract_ids(const vector<snapshot_data>&);
    
    
    inline std::string file_extension(const std::string& p)
    {
        string temps;
        bool dotmet(false);
        
        if(is_file(p).value)
        {
            for(std::string::const_reverse_iterator it = p.rbegin(); ((it != p.rend()) && !dotmet); it++)
            {
                temps = ((*it) + temps);
                if((*it == '.') && !dotmet) dotmet = true;
            }
        }
        return temps;
    }
    
    inline unordered_set<unsigned long long> extract_ids(const vector<snapshot_data>&  snapshots)
    {
        unordered_set<unsigned long long> ids;
        for(vector<snapshot_data>::const_iterator it = snapshots.begin(); it != snapshots.end(); ++it)
        {
            ids.insert(it->id);
        }
        return ids;
    }
    
    
}

namespace snapshot
{
    std::vector<string> paths_of_extension(const string& p, const string& ext)
    {
        std::vector<string> snapshot_files;
        
        if(is_folder(p).value && !is_symlink(p).value)
        {
            for(tree_riterator_class it(p); !it.at_end(); ++it)
            {
                if(file_extension(it.value()) == ext)
                {
                    snapshot_files.push_back(it.value());
                }
            }
        }
        return snapshot_files;
    }
    
    std::string snapshot_folder()
    {
        return (current_path().make_preferred().string() + 
                boost::filesystem::path("/").make_preferred().string() + 
                fsyssnap_SNAPSHOT_FOLDER_NAME);
    }
    
    /* Returns the path of a snapshot given its id.*/
    std::string snapshot_path(const unsigned long long& id)
    {
        return std::string(snapshot_folder() + pref_slash() + std::string("snapshot") + 
                        std::to_string(id) + std::string(fsyssnap_SNAPSHOT_FILE_EXTENSION));
    }
    
    /* Loads an entire snapshot.  Returns false if fails. */
    bool load_snapshot(snapshot_data& snap, const string& path)
    {
        using fsys::is_folder;
        using fsys::is_file;
        using fsys::is_symlink;
        using std::ifstream;
        
        bool success(false);
        ifstream in;
        
        snap = snapshot_data();
        
        if(is_file(path).value && !is_symlink(path).value)
        {
            in.open(path.c_str(), std::ios::in);
            if(in.good())
            {
                in>> snap;
                if(!in.good())
                {
                    success = false;
                    snap = snapshot::snapshot_data();
                }
            }
            in.close();
        }
        return success;
    }
    
    /* returns a hash of all the paths in a snapshot by loading the snapshot
     * and then extracvting the paths from it.  Very in-efficient, but for the
     * purpose of comparing two snapshots, this is the least memory-intensive. */
    std::unordered_set<std::string> load_paths_from_snapshot(const snapshot::snapshot_data& snap)
    {
        using fsys::is_file;
        using fsys::is_folder;
        using fsys::is_symlink;
        
        std::unordered_set<std::string> paths;
        snapshot::snapshot_data tempsnap;
        std::string folder(snapshot::snapshot_folder()), path(snapshot::snapshot_path(snap.id));
        
        if(snap.id > 0)
        {
            if(is_folder(folder).value && !is_symlink(folder).value)
            {
                if(is_file(path).value && !is_symlink(path).value)
                {
                    if(load_snapshot(tempsnap, path))
                    {
                        for(std::vector<std::string>::const_iterator it = tempsnap.paths.begin(); 
                                        it != tempsnap.paths.end(); ++it)
                        {
                            if(!paths.insert(*it).second) std::cerr<< "\""<< *it<< "\" \
paths.insert(*it) > failed to insert"<< std::endl;
                        }
                    }
                }
            }
        }
        return paths;
    }
    
    unsigned long long new_snapshot_id()
    {
        unordered_set<unsigned long long> ids(extract_ids(
                list_snapshot_info(snapshot::snapshot_folder())));
        unsigned long long newid(1);
        
        if(!ids.empty())
        {
            while(ids.find(newid) != ids.end()) newid++;
        }
        return newid;
    }
    
    /** Loads all the snapshot information from snapshot files in a specified
     * folder.  It gets everything but the pathlist to save memory*/
    std::vector<snapshot::snapshot_data> list_snapshot_info(const std::string& root)
    {
        using fsys::is_file;
        
        std::vector<std::string> files(snapshot::paths_of_extension(root, fsyssnap_SNAPSHOT_FILE_EXTENSION));
        std::vector<snapshot::snapshot_data> snapshot_list;
        std::ifstream in;
        
        if(files.size() > 0)
        {
            for(vector<string>::const_iterator it = files.begin(); it != files.end(); ++it)
            {
                if(is_file(*it).value)
                {
                    in.open(it->c_str(), std::ios::in);
                    if(in.good())
                    {
                        snapshot_list.push_back(snapshot_data());
                        snapshot::retrieve_info(in, snapshot_list.back());
                    }
                    in.close();
                }
            }
        }
        files.clear();
        files.shrink_to_fit();
        return snapshot_list;
    }
    
    /* Retrieves the header info on a snapshot.  Can be used to avoid loading
     huge number of paths at once. */
    std::istream& retrieve_info(std::istream& in, snapshot_data& snap)
    {
        using common::safe_getline;
        
        snap.id = 0;
        snap.root.erase();
        snap.timestamp = date::date_val();
        
        if(in.good())
        {
            in>> snap.timestamp;
            if(safe_getline(in, snap.id, mem_delim::value))
                safe_getline(in, snap.root, mem_delim::value);
        }
        return in;
    }
    
    
}