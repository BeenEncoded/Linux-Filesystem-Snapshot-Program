#include <iostream>
#include <fstream>
#include <string>

#include "snapshot_file_loader.hpp"
#include "global_defines.hpp"
#include "modify_snapshot_menu.hpp"
#include "filesystem.hpp"
#include "settings_loader.hpp"


namespace
{
    void main_menu();
    
    

    inline void main_menu()
    {
        using settings::settings_data;
        using snapshot::snapshot_folder;
        using common_menu::menu_return_data;
        
        settings_data program_settings(settings::load(snapshot_folder()));
        snapshot_menu::main_snapshot_menu(snapshot_folder(), program_settings);
    }
    
    inline settings::settings_data load_settings(const std::string& folder)
    {
        using fsys::is_folder;
        using fsys::is_file;
        using fsys::is_symlink;
        using fsys::pref_slash;
        
        std::string file(folder + pref_slash() + std::string("settings.dat"));
        settings::settings_data program_settings;
        
        if(is_folder(folder).value && !is_symlink(folder).value)
        {
            if(is_file(file).value)
            {
                std::ifstream in(file.c_str(), std::ios::in);
                if(in.good()) in>> program_settings;
                else ethrow("Error: can not load the settings file!"); //this shouldn't happen.  If it does, there's somthing wrong.
                in.close();
            }
        }
        return program_settings;
    }
    
    
}

int main(__attribute__((unused)) int c, __attribute__((unused)) char **v)
{
    main_menu();
    return 0;
}
