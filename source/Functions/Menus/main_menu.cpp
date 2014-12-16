#include <iostream>
#include <string>

#include "main_menu.hpp"
#include "common_menu.hpp"
#include "common.hpp"
#include "filesystem.hpp"
#include "program_settings.hpp"
#include "settings_loader.hpp"

namespace
{
    unsigned int folder_content_count(const std::string&);
    
    
    
    inline unsigned int folder_content_count(const std::string& f)
    {
        typedef fsys::tree_iterator_class iterator;
        
        using fsys::is_folder;
        using fsys::is_file;
        using fsys::is_symlink;
        
        unsigned int count(0);
        
        if(is_folder(f).value && !is_symlink(f).value)
        {
            for(iterator it(f); !it.at_end(); ++it, ++count);
        }
        return count;
    }
    
    
}

namespace menu
{
    common_menu::menu_return_data main_menu(settings::settings_data& psettings)
    {
        using common_menu::menu_return_data;
        using settings::load;
        using std::cout;
        using std::endl;
        using key_code::key_code_data;
        
        bool finished(false);
        menu_return_data result;
        std::string const *record_folder(&psettings.global.records_folder), 
                *snapshot_folder(&psettings.global.snapshot_folder);
        key_code_data ch;
        
        
        do
        {
            common::cls();
            cout<< endl;
            common::center("Main menu");
            cout<< std::string(4, '\n');
            cout.flush();
            
            if(folder_content_count(*snapshot_folder) > 0)
            {
                cout<< "M -  Manage Snapshots";
            }
            cout<< endl;
            cout<< "T -  Take new Snapshot"<< endl;
            cout<< "S -  Settings"<< endl;
            if(folder_content_count(*record_folder) > 0)
            {
                cout<< "R -  Manage comparison records";
            }
            cout<< endl;
            cout<< endl;
            cout<< "E -  Exit"<< endl;
            ch = common::gkey();
            if(!key_code::is_listed_control(ch))
            {
                if(!ch.is_control && !ch.control_d.empty())
                {
                    switch(std::tolower((char)ch.control_d[0]))
                    {
                        //cur_pos finish menu
                        default:
                        {
                        }
                        break;
                    }
                }
            }
        }while(!finished);
        return result;
    }
    
    
}