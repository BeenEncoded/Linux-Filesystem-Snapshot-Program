

#include "snapshot_file_loader.hpp"
#include "main_menu.hpp"
#include "program_settings.hpp"
#include "settings_loader.hpp"


namespace
{
    void main_menu();
    

    inline void main_menu()
    {
        using settings::settings_data;
        
        settings_data program_settings;
        program_settings = settings::load(program_settings.global.settings_folder);
        menu::main_menu(program_settings);
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
