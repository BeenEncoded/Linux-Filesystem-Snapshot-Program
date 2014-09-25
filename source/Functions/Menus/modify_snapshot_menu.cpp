#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <boost/filesystem.hpp>
#include <unordered_set>

#include "modify_snapshot_menu.hpp"
#include "snapshot_file_loaders.hpp"
#include "snapshot_class.hpp"
#include "common.hpp"
#include "global_defines.hpp"
#include "scroll_display.hpp"
#include "filesystem.hpp"
#include "date_class.hpp"
#include "common_menu.hpp"

using common_menu::menu_return_data;
using snapshot::snapshot_data;
using std::cout;
using std::endl;
using std::cerr;
using std::vector;
using std::string;
using std::unordered_set;


namespace
{
    void display_help();
    
    
    
    inline void display_help()
    {
        common::cls();
        for(unsigned int x = 0; x < 3; x++) cout<< endl;
        cout<< " Controls:"<< endl;
        cout<< endl;
        cout<< " [DELETE]      -  Deletes selected snapshot"<< endl;
        cout<< " [ENTER]       -  Set selected snapshot"<< endl;
        cout<< " [up arrow]    -  scroll up in list"<< endl;
        cout<< " [down arrow]  -  scroll down in list"<< endl;
        cout<< " [page up]     -  scroll one page up"<< endl;
        cout<< " [page down]   -  scroll one page down"<< endl;
        cout<< " [HOME]        -  jump to begin"<< endl;
        cout<< " [END]         -  jump to end"<< endl;
        cout<< " \'n\'         -  New Snapshot"<< endl;
        cout<< " \'e\'         -  Exit"<< endl;
        common::wait();
    }
    
    
}

namespace snapshot_menu
{
    
    menu_return_data main_snapshot_menu(vector<snapshot_data>& snapshots)
    {
        using scrollDisplay::scroll_display_class;
        using common::display_scroll_window;
        using key_code::key_code_data;
        
        menu_return_data result;
        vector<string> display;
        scroll_display_class window(display);
        key_code_data ch;
        bool finished(false);
        
        auto update_display = [&snapshots, &display](void)->void
        {
            display.clear();
            for(std::vector<snapshot_data>::const_iterator it = snapshots.begin(); 
                    it != snapshots.end(); ++it)
            {
                display.push_back(date::display(it->timestamp));
            }
        };
        
        
        update_display();
        window.window_size() = common_menu::wsize::value;
        do
        {
            common::cls();
            cout<< endl;
            common::center("Snapshot Menu:");
            cout<< endl;
            
            for(unsigned int x = 0; x < 2; x++) cout<< endl;
            common_menu::display_scroll_window(window, display.size());
            
            for(unsigned int x = 0; x < 3; x++) cout<< endl;
            cout<< " [SPC] -  Select"<< endl;
            cout<< " n -  NEW snapshot"<< endl;
            cout<< " e -  Exit"<< endl;
            
            ch = common::gkey_funct();
            
            if(key_code::is_control(ch))
            {
                if(!display.empty())
                {
                    //block for containment of key_code::code namespace
                    {
                        using key_code::keys;
                        using namespace key_code::code;

                        if(ch == keys[up::value]) window.mv_up();
                        else if(ch == keys[down::value]) window.mv_down();
                        else if(ch == keys[pgup::value]) window.pg_up();
                        else if(ch == keys[pgdown::value]) window.pg_down();
                        else if(ch == keys[home::value]) while(window.pg_up());
                        else if(ch == keys[end::value]) while(window.pg_down());
                        else if(ch == keys[del::value])
                        {
                            if(!snapshots.empty())
                            {
                                snapshots.erase(snapshots.begin() + window.gpos().whole);
                            }
                            update_display();
                        }
                        else if(ch == keys[f5::value])
                        {
                            display_help();
                        }
                    }
                }
            }
            else
            {
                if(!ch.is_control)
                {
                    switch(std::tolower(ch.ch()))
                    {
                        case 'n':
                        {
                            if(common::inp::is_sure("Do you really want to take a snapshot?"))
                            {
                                using std::cout;
                                using std::endl;
                                using std::cin;
                                
                                std::string temps;
                                
                                common::cls();
                                cout<< "Enter nothing to cancel"<< endl;
                                for(unsigned int x = 0; x < 3; x++) cout<< endl;
                                cout<< "Enter a root folder to scan: ";
                                std::getline(cin, temps, '\n');
                                if(!temps.empty())
                                {
                                    if(fsys::is_folder(temps).value && !fsys::is_symlink(temps).value)
                                    {
                                        if(snapshot::take_snapshot(temps) != 0)
                                        {
                                            snapshots.clear();
                                            snapshots.shrink_to_fit();
                                            snapshots = snapshot::list_snapshot_info(snapshot::snapshot_folder());
                                        }
                                    }
                                }
                                update_display();
                            }
                        }
                        break;
                        
                        case ' ':
                        {
                            //todo select snapshot to compare
                        }
                        break;
                        
                        case 'e':
                        {
                            if(!finished) finished = true;
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