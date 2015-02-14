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
    void display_about();
    
    
    inline void display_about()
    {
        using std::cout;
        using std::endl;
        
        common::cls();
        cout<< endl;
        cout<< "This program was compiled on "<< __DATE__<< " at "<< __TIME__<< endl;
        cout<< std::string(2, '\n');
        cout<< "Written by Jonathan Whitlock"<< endl;
        cout<< std::string(1, '\n');
        cout<< "This program is meant to be used for administrative purposes.  \
It was made in order to keep track of paths that are created and deleted on \
a filesystem.  This is good for things like recording what files are created \
during an installation process, or uninstallation process.  For those purposes, \
regular expression filtering can be used.  Note that this filtering will only \
filter results of a comparison, not the actual snapshot process, so if you want \
to, you may change the filter settings and re-perform a comparison on the same \
\"snapshots\" and receive accorded results.  For each comparison, a record is saved.  \
These records are stored in plain text files and can be managed from the main \
menu once there is more than one record to view.  You may choose the editor you \
view these records in through the settings menu.";
        cout<< endl;
        common::wait();
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
        using common_menu::records_exist;
        using common_menu::snapshots_exist;
        
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
            
            if(snapshots_exist(*snapshot_folder))
            {
                cout<< "M -  Manage Snapshots";
            }
            cout<< endl;
            cout<< "T -  Take new Snapshot"<< endl;
            cout<< "S -  Settings"<< endl;
            if(records_exist(*record_folder))
            {
                cout<< "R -  Manage comparison records";
            }
            cout<< endl;
            cout<< "A -  About"<< endl;
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
                            if(snapshots_exist(*snapshot_folder))
                            {
                                common_menu::menu_return_data tempres(menu::manage_snapshots(psettings));
                                if(tempres.modified && !tempres.canceled)
                                {
                                    if(!result.modified) result.modified = true;
                                }
                            }
                        }
                        break;
                        
                        case 'a':
                        {
                            display_about();
                        }
                        break;
                        
                        case 'r':
                        {
                            if(records_exist(*record_folder))
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