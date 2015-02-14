#include <string>
#include <fstream>
#include <iostream>

#include "settings_loader.hpp"
#include "program_settings.hpp"
#include "common.hpp"
#include "filesystem.hpp"
#include "global_defines.hpp"

namespace
{
    std::string settings_file(const std::string&) noexcept;
    void center_message(const std::string&) noexcept;
    
    
    
    inline std::string settings_file(const std::string& folder) noexcept
    {
        return (folder + fsys::pref_slash() + std::string("program settings.dat"));
    }
    
    inline void center_message(const std::string& message) noexcept
    {
        using std::cout;
        using std::endl;
        
        common::cls();
        cout<< std::string(v_center::value, '\n');
        cout.flush();
        common::center(message);
        common::wait();
        common::cls();
        cout<< "please wait...";
        cout.flush();
    }
    
    
}

namespace settings
{
    std::string settings_folder() noexcept
    {
        return (fsys::current_path() + fsys::pref_slash() + std::string(fsyssnap_SNAPSHOT_FOLDER_NAME));
    }
    
    /** Loads the settings from a settings file from a specified folder. */
    settings::settings_data load(const std::string& folder)
    {
        using fsys::is_folder;
        using fsys::is_symlink;
        using fsys::is_file;
        using settings::settings_data;
        
        std::ifstream in;
        std::string file(settings_file(folder));
        settings_data settings;
        
        if(is_folder(folder).value && !is_symlink(folder).value  && 
                is_file(file).value && !is_symlink(file).value)
        {
            in.open(file.c_str(), std::ios::in);
            if(in.good()) in>> settings;
            else ethrow("Error: could not load settings!");
            in.close();
        }
        return settings;
    }
    
    /** Saves settings to the settings file within a specified folder. */
    bool save(const settings::settings_data& s)
    {
        using fsys::is_folder;
        using fsys::is_file;
        using fsys::is_symlink;
        using settings::settings_data;
        
        std::ofstream out;
        std::string file(settings_file(s.global.settings_folder));
        
        if(!is_folder(s.global.settings_folder).value)
        {
            fsys::result_data_boolean tempres;
            if(is_symlink(s.global.settings_folder).value)
            {
                if(!common::inp::is_sure("This folder exists as a symlink!  Would you like \
to delete it?"))
                {
                    center_message("Unable to save your settings: folder exists as a symlink!");
                    return false;
                }
                tempres = fsys::fdelete(s.global.settings_folder);
                if(!tempres.value)
                {
                    center_message("Error deleting the symlink: \"" + tempres.error + 
                            "\"");
                    return false;
                }
                if(is_folder(s.global.settings_folder).value || is_symlink(s.global.settings_folder).value)
                {
                    center_message("Error: Delete reports false positive!  Can not create the folder.");
                    return false;
                }
            }
            tempres = fsys::create_folder(s.global.settings_folder);
            if(!tempres.value)
            {
                center_message("Could not create the folder: \"" + tempres.error + "\"");
                return false;
            }
        }
        out.open(file.c_str(), std::ios::out);
        if(out.good()) out<< s;
        else ethrow("Error, could not open ostream!");
        out.close();
        return true;
    }
    
    
}