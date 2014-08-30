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
using std::vector;
using std::string;
using std::unordered_set;


namespace
{
    bool load_snapshot(snapshot_data&, const string&);
    vector<snapshot_data> list_snapshot_info(const string&);
    unsigned long long new_snapshot_id();
    unordered_set<unsigned long long> extract_ids(const vector<snapshot_data>&);
    
    
    
    inline bool load_snapshot(snapshot_data& snap, const string& path)
    {
        using fsys::is_folder;
        using fsys::is_file;
        using fsys::is_symlink;
        using std::ifstream;
        
        bool success(false);
        ifstream in;
        
        snap = snapshot_data();
        
        if(is_file(path).value)
        {
            in.open(path.c_str(), std::ios::in);
            if(in.good())
            {
                success = true;
                in>> snap;
            }
            in.close();
        }
        return success;
    }
    
    inline vector<snapshot_data> list_snapshot_info(const string& root)
    {
        using fsys::is_file;
        
        vector<string> files(snapshot::paths_of_extension(root, fsyssnap_SNAPSHOT_FILE_EXTENSION));
        vector<snapshot_data> snapshot_list;
        std::ifstream in;
        
        if(files.size() > 0)
        {
            for(vector<string>::const_iterator it = files.begin(); it != files.end(); ++it)
            {
                if(is_file(*it).value)
                {
                    in.open(it->c_str(), std::ios::in);
                    if(in.good())
                    {
                        snapshot_list.push_back(snapshot_data());
                        snapshot::retrieve_info(in, snapshot_list.back());
                    }
                    in.close();
                }
            }
        }
        files.clear();
        files.shrink_to_fit();
        return snapshot_list;
    }
    
    inline unordered_set<unsigned long long> extract_ids(const vector<snapshot_data>&  snapshots)
    {
        unordered_set<unsigned long long> ids;
        for(vector<snapshot_data>::const_iterator it = snapshots.begin(); it != snapshots.end(); ++it)
        {
            ids.insert(it->id);
        }
        return ids;
    }
    
    inline unsigned long long new_snapshot_id()
    {
        unordered_set<unsigned long long> ids(extract_ids(
                list_snapshot_info(snapshot::snapshot_folder())));
        unsigned long long newid(1);
        
        if(!ids.empty())
        {
            while(ids.find(newid) != ids.end()) newid++;
        }
        return newid;
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
        
        do
        {
            common::cls();
            cout<< endl;
            common::center("Snapshot Menu:");
            cout<< endl;
            
            for(unsigned int x = 0; x < 2; x++) cout<< endl;
            common_menu::display_scroll_window(window, display.size());
            
            for(unsigned int x = 0; x < 3; x++) cout<< endl;
            cout<< " n -  NEW snapshot"<< endl;
            cout<< " e -  Exit"<< endl;
            
            ch = common::gkey_funct();
            
            if(key_code::is_control(ch))
            {
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
                    }
                    else if(ch == keys[f5::value])
                    {//@todo help display
                    }
                }
            }
            else
            {
                if(!ch.is_control)
                {
                    switch(std::tolower(ch.ch()))
                    {
                        case '\n':
                        {
                            if(!snapshots.empty())
                            {
                                //@todo modify selected snapshot
                            }
                        }
                        break;
                        
                        case 'n':
                        {
                            //@todo add new snapshot && save
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