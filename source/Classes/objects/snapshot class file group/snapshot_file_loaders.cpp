#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <boost/filesystem.hpp>

#include "snapshot_file_loaders.hpp"
#include "filesystem.hpp"
#include "global_defines.hpp"

using boost::filesystem::current_path;
using fsys::is_file;
using fsys::is_folder;
using fsys::is_symlink;
using fsys::tree_riterator_class;
using std::string;
using fsys::pref_slash;

namespace
{
    string file_extension(const std::string&);
    
    
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
    
    std::string snapshot_path(const unsigned long long& id)
    {
        return std::string(snapshot_folder() + pref_slash() + std::string("snapshot") + 
                        std::to_string(id) + std::string(fsyssnap_SNAPSHOT_FILE_EXTENSION));
    }
    
    
}