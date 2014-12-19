#include <iostream>
#include <string>
#include <boost/filesystem.hpp>

#include "main_menu.hpp"
#include "common_menu.hpp"
#include "common.hpp"
#include "filesystem.hpp"
#include "program_settings.hpp"
#include "settings_loader.hpp"
#include "settings_menu.hpp"
#include "snapshot_class.hpp"
#include "global_defines.hpp"
#include "modify_snapshot_menu.hpp"
#include "record_menu.hpp"

namespace
{
    bool folder_empty(const std::string&);
    bool has_contents(const std::string&);
    
    
    
    inline bool folder_empty(const std::string& folder)
    {
        using fsys::is_folder;
        using fsys::is_symlink;
        using fsys::is_file;
        using std::cout;
        using std::endl;
        
        boost::system::error_code err;
        
        if(is_folder(folder).value && !is_symlink(folder).value)
        {
            bool empty(boost::filesystem::is_empty(folder, err));
            if(err != boost::system::errc::success)
            {
                common::cls();
                cout<< std::string(v_center::value, '\n')<< "Error: \""<< 
                        err.message()<< "\"";
                cout.flush();
                common::wait();
            }
            return empty;
        }
        return false;
    }
    
    /** Returns true if a folder is not empty, and if it exists. */
    inline bool has_contents(const std::string& folder)
    {
        return (!folder_empty(folder) && fsys::is_folder(folder).value && 
                !fsys::is_symlink(folder).value);
    }
    
    
}

namespace menu
{
    common_menu::menu_return_data main_menu(settings::settings_data& psettings)
    {
        using fsys::is_folder;
        using fsys::is_symlink;
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
            common::center("Filesystem Snapshot Program");
            cout<< std::string(4, '\n');
            cout.flush();
            
            if(has_contents(*snapshot_folder))
            {
                cout<< "M -  Manage Snapshots";
            }
            cout<< endl;
            cout<< "T -  Take new Snapshot"<< endl;
            cout<< "S -  Settings"<< endl;
            if(has_contents(*record_folder))
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
                        case 't':
                        {
                            std::string temps;
                            
                            common::cls();
                            cout<< "Enter nothing to cancel."<< std::string(3, '\n');
                            cout<< "Enter a folder to take a snapshot of: ";
                            cout.flush();
                            std::getline(std::cin, temps);
                            if(!temps.empty())
                            {
                                if(fsys::is_folder(temps).value && !fsys::is_symlink(temps).value)
                                {
                                    if(snapshot::take_snapshot(temps) > 0)
                                    {
                                        if(!result.modified) result.modified = true;
                                    }
                                }
                            }
                            temps.erase();
                        }
                        break;
                        
                        case 's':
                        {
                            common_menu::menu_return_data tempres(menu::modify_program_settings(psettings));
                            if(tempres.modified && !tempres.canceled)
                            {
                                settings::save(psettings);
                                if(!result.modified) result.modified = true;
                            }
                        }
                        break;
                        
                        case 'm':
                        {
                            if(has_contents(*snapshot_folder))
                            {
                                common_menu::menu_return_data tempres(snapshot_menu::main_snapshot_menu(psettings));
                                if(tempres.modified && !tempres.canceled)
                                {
                                    if(!result.modified) result.modified = true;
                                }
                            }
                        }
                        break;
                        
                        case 'r':
                        {
                            if(has_contents(*record_folder))
                            {
                                common_menu::menu_return_data tempres(menu::manage_records(psettings));
                                if(tempres.modified && !tempres.canceled)
                                {
                                    if(!result.modified) result.modified = true;
                                    settings::save(psettings);
                                }
                            }
                        }
                        break;
                        
                        case 'e':
                        {
                            finished = true;
                        }
                        break;
                        
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