#include <string>
#include <vector>
#include <iostream>
#include <unordered_map>

#include "common.hpp"
#include "global_defines.hpp"
#include "snapshot_class.hpp"
#include "snapshot_file_loaders.hpp"
#include "common_menu.hpp"
#include "time_class.hpp"
#include "filesystem.hpp"


namespace
{
    void display_help();
    bool remove_snapshot(const snapshot::snapshot_data&);
    std::string display_time(const tdata::time_class&);
    void diff_snapshots(const unsigned long long&, const unsigned long long&);
    bool create_record_folder();
    std::string record_folder();
    std::vector<snapshot::snapshot_data> load_all_headers(const std::string&);
    
    
    
    inline void display_help()
    {
        using std::cout;
        using std::endl;
        
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
    
    inline bool remove_snapshot(const snapshot::snapshot_data& snap)
    {
        using fsys::is_folder;
        using fsys::is_file;
        using fsys::is_symlink;
        using fsys::can_delete;
        using fsys::fdelete;
        using snapshot::snapshot_folder;
        
        std::string file(snapshot::snapshot_folder() + fsys::pref_slash() + 
                        std::to_string(snap.id) + fsyssnap_SNAPSHOT_FILE_EXTENSION);
        
        if(is_folder(snapshot_folder()).value && !is_symlink(snapshot_folder()).value)
        {
            if(is_file(file).value)
            {
                if(can_delete(file))
                {
                    return fdelete(file).value;
                }
            }
        }
        return false;
    }
    
    /** Returns a string representation of time */
    inline std::string display_time(const tdata::time_class& t)
    {
        std::string temps;
        
        temps = (t.wday_name() + " " + t.month_name() + " " + 
                        std::to_string((t.month() + 1)) + ", " + 
                        std::to_string(t.gyear()) + " at " + 
                        std::to_string(t.hour()) + ":" + std::to_string(t.minute()) + 
                        "." + std::to_string(t.second()) + std::string(" ") + 
                        (t.am() ? "AM" : "PM"));
        return temps;
    }

    inline std::string record_folder()
    {
        return std::string(snapshot::snapshot_folder() + fsys::pref_slash() + 
                        std::string("records"));
    }
    
    inline bool create_record_folder()
    {
        using fsys::is_folder;
        using fsys::is_file;
        using fsys::is_symlink;
        using snapshot::snapshot_folder;
        
        std::string folder(record_folder());
        
        if(!is_folder(folder).value && !is_file(folder).value && !is_symlink(folder).value)
        {
            if(!is_folder(snapshot_folder()).value && !is_file(snapshot_folder()).value && 
                            !is_symlink(snapshot_folder()).value)
            {
                if(!fsys::create_folder(snapshot_folder()).value) return false;
            }
            if(is_folder(snapshot_folder()).value && !is_symlink(snapshot_folder()).value)
            {
                fsys::create_folder(folder);
            }
        }
        return (is_folder(folder).value && !is_symlink(folder).value);
    }
    
    inline void diff_snapshots(const unsigned long long& id1 __attribute__((unused)), const unsigned long long& id2 __attribute__((unused)))
    {
        //todo finish this... I want to write a diff object to handle it, though
    }
    
    std::vector<snapshot::snapshot_data> load_all_headers(const std::string& folder)
    {
        using fsys::is_folder;
        using fsys::is_symlink;
        
        std::unordered_map<unsigned long long, std::string> ids;
        std::vector<snapshot::snapshot_data> snaps;
        
        if(is_folder(folder).value && !is_symlink(folder).value)
        {
            ids = snapshot::list_ids(folder);
            if(!ids.empty())
            {
                for(std::unordered_map<unsigned long long, std::string>::const_iterator it = ids.begin(); 
                                it != ids.end(); ++it)
                {
                    snaps.push_back(snapshot::snapshot_data());
                    if(!snapshot::load_header(snaps.back(), it->second)) snaps.pop_back();
                }
            }
        }
        return snaps;
    }
    
    
}

namespace snapshot_menu
{
    
    common_menu::menu_return_data main_snapshot_menu(const std::string& folder)
    {
        using namespace common_menu;
        using scrollDisplay::scroll_display_class;
        using common::display_scroll_window;
        using key_code::key_code_data;
        using std::cout;
        using std::endl;
        using namespace ::snapshot;
        
        menu_return_data result;
        std::vector<snapshot_data> snapshots(load_all_headers(folder));
        std::vector<std::string> display;
        scrollDisplay::scroll_display_class window(display);
        key_code::key_code_data ch;
        bool finished(false);
        
        auto update_display = [&snapshots, &display](void)->void
        {
            display.clear();
            for(unsigned int x = 0; x < snapshots.size(); x++)
            {
                display.push_back(display_time(snapshots.at(x).timestamp) + "   root: \"" + snapshots[x].root + "\"");
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
            
            if(key_code::is_listed_control(ch))
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
                                if(remove_snapshot(snapshots.at(window.gpos().whole))) 
                                {
                                    snapshots.erase(snapshots.begin() + window.gpos().whole);
                                }
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
                                        if(take_snapshot(temps) != 0)
                                        {
                                            snapshots.clear();
                                            snapshots.shrink_to_fit();
                                            snapshots = load_all_headers(folder);
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