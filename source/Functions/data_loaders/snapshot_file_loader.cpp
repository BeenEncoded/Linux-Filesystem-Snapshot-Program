#include <vector>
#include <string>
#include <fstream>

#include "snapshot_file_loader.hpp"
#include "filesystem.hpp"
#include "global_defines.hpp"
#include "common.hpp"
#include "snapshot_class.hpp"

using boost::filesystem::current_path;
using fsys::is_file;
using fsys::is_folder;
using fsys::is_symlink;
using fsys::tree_riterator_class;
using std::string;
using fsys::pref_slash;
using std::vector;
using snapshot::snapshot_data;

namespace
{
    string file_extension(const std::string&);
    bool load_id(const std::string&, unsigned long long&);
    std::vector<std::string> paths_of_extension(const std::string&, const std::string&);
    
    
    inline std::string file_extension(const std::string& p)
    {
        string temps(p);
        std::string::size_type pos(std::string::npos);
        
        if(!temps.empty())
        {
            pos = temps.rfind('.');
            if(pos != std::string::npos)
            {
                temps.erase(temps.begin(), (temps.begin() + pos));
            }
        }
        return temps;
    }
    
    /** Loads a snapshot's id from a designated file.  If fails, returns false. */
    inline bool load_id(const std::string& s, unsigned long long& id)
    {
        id = 0;
        bool success(false);
        std::ifstream in;
        snapshot::snapshot_data tempsnap;
        
        if(is_file(s).value && !is_symlink(s).value)
        {
            in.open(s, std::ios::binary);
            snapshot::in_header(in, tempsnap);
            in.close();
            id = tempsnap.id;
            success = (id > 0);
        }
        return success;
    }
    
    /** Finds a all the paths in a folder that end in a specified extension. */
    inline std::vector<string> paths_of_extension(const string& p, const string& ext)
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
    
    
}

namespace snapshot
{
    /** Returns the name of the folder that should be used to put snapshots in. */
    std::string snapshot_folder()
    {
        return (current_path().make_preferred().string() + 
                boost::filesystem::path("/").make_preferred().string() + 
                fsyssnap_SNAPSHOT_FOLDER_NAME);
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
    
    /** Generates a unique id for a snapshot.  The id should not be in use
     * by another snapshot, and should not be equal to zero. */
    unsigned long long new_snapshot_id()
    {
        std::unordered_map<unsigned long long, std::string> ids(list_ids(snapshot_folder()));
        unsigned long long newid(1);
        
        if(!ids.empty())
        {
            while(ids.find(newid) != ids.end()) newid++;
        }
        return newid;
    }
    
    /* Lists the ids of all snapshots found within a folder.  The ids are tied 
     * to the files they were extracted from, so loading the appropriate
     * file for an id should be trivial. */
    std::unordered_map<unsigned long long, std::string> list_ids(const std::string& folder)
    {
        std::unordered_map<unsigned long long, std::string> ids;
        unsigned long long tempid(0);
        std::vector<std::string> files;
        
        if(is_folder(folder).value && !is_symlink(folder).value)
        {
            files = paths_of_extension(folder, fsyssnap_SNAPSHOT_FILE_EXTENSION);
            if(!files.empty())
            {
                for(std::vector<std::string>::const_iterator it = files.begin(); 
                                it != files.end(); ++it)
                {
                    if(load_id(*it, tempid)) ids[tempid] = *it;
                }
            }
        }
        return ids;
    }
    
    bool load_header(snapshot::snapshot_data& snap, const std::string& file)
    {
        bool success(false);
        std::ifstream in;
        
        if(is_file(file).value && !is_symlink(file).value)
        {
            in.open(file.c_str(), std::ios::binary);
            snapshot::in_header(in, snap);
            in.close();
            success = (snap.id > 0);
        }
        return success;
    }
    
    
}