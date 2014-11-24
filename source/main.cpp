#include <iostream>

#include "snapshot_file_loaders.hpp"
#include "global_defines.hpp"
#include "modify_snapshot_menu.hpp"
#include "program_settings.hpp"
#include "filesystem.hpp"


namespace
{
    void main_menu();
    

    inline void main_menu()
    {
        using settings::settings_data;
        using settings::load_settings;
        using snapshot::snapshot_folder;
        
        settings_data program_settings(load_settings(snapshot_folder() + 
                fsys::pref_slash() + std::string("program settings.dat")));
        
        snapshot_menu::main_snapshot_menu(snapshot_folder());
    }
    
    
}

int main(__attribute__((unused)) int c, __attribute__((unused)) char **v)
{
    main_menu();
    return 0;
}