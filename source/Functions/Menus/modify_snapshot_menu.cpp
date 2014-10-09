#include <string>
#include <vector>
#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <stdexcept>

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
    bool remove_snapshot(const snapshot::snapshot_data&, const std::string&);
    std::string display_time(const tdata::time_class&);
    void diff_snapshots(const std::string&, const std::string&);
    bool create_record_folder(const std::string&);
    std::string record_folder(const std::string&);
    std::vector<snapshot::snapshot_data> load_all_headers(const std::string&);
    std::vector<std::string> difference_between(const std::unordered_set<std::string>&, 
                    const std::unordered_set<std::string>&);
    std::string file_of_snapshot(const snapshot::snapshot_data&, const std::string&);
    std::unordered_set<std::string> load_paths(const std::string&);
    
    
    
    inline void display_help()
    {
        using std::cout;
        using std::endl;
        
        common::cls();
        for(unsigned int x = 0; x < 3; x++) cout<< endl;
        cout<< " Controls:"<< endl;
        cout<< endl;
        cout<< " [DELETE]      -  Deletes selected snapshot"<< endl;
        cout<< " [up arrow]    -  scroll up in list"<< endl;
        cout<< " [down arrow]  -  scroll down in list"<< endl;
        cout<< " [page up]     -  scroll one page up"<< endl;
        cout<< " [page down]   -  scroll one page down"<< endl;
        cout<< " [HOME]        -  jump to begin"<< endl;
        cout<< " [END]         -  jump to end"<< endl;
        cout<< " \' \'         -  Select a snapshot"<< endl;
        cout<< " \'c\'         -  Compare selected snapshots"<< endl;
        cout<< " \'n\'         -  New Snapshot"<< endl;
        cout<< " \'\\\'        -  Unselect all"<< endl;
        cout<< " \'e\'         -  Exit"<< endl;
        common::wait();
    }
    
    /** Removes a snapshot's file. */
    inline bool remove_snapshot(const snapshot::snapshot_data& snap, const std::string& folder)
    {
        using fsys::is_folder;
        using fsys::is_file;
        using fsys::is_symlink;
        using fsys::can_delete;
        using fsys::fdelete;
        using snapshot::snapshot_folder;
        
        std::string file(file_of_snapshot(snap, folder));
        
        if(is_folder(folder).value && !is_symlink(folder).value)
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
                        std::to_string(t.mday()) + ", " + 
                        std::to_string(t.gyear()) + " at " + 
                        std::to_string(t.hour()) + ":" + std::to_string(t.minute()) + 
                        "." + std::to_string(t.second()) + std::string(" ") + 
                        (t.am() ? "AM" : "PM"));
        return temps;
    }

    /** Returns the full path of the record folder given the snapshot
     * folder being used. */
    inline std::string record_folder(const std::string& folder)
    {
        return std::string(folder + fsys::pref_slash() + 
                        std::string("records"));
    }
    
    inline bool create_record_folder(const std::string& f)
    {
        using fsys::is_folder;
        using fsys::is_file;
        using fsys::is_symlink;
        using snapshot::snapshot_folder;
        
        std::string folder(record_folder(f));
        
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
    
    /** Returns a vector of strings containing the elements found in bef, but not in aft. */
    inline std::vector<std::string> difference_between(const std::unordered_set<std::string>& bef, 
                    const std::unordered_set<std::string>& aft)
    {
        std::vector<std::string> diff;
        for(std::unordered_set<std::string>::const_iterator it = bef.begin(); it != bef.end(); ++it)
        {
            if(aft.find(*it) == aft.end()) diff.push_back(*it);
        }
        return diff;
    }
    
    inline std::unordered_set<std::string> load_paths(const std::string& file)
    {
        using fsys::is_file;
        using fsys::is_symlink;
        
        std::unordered_set<std::string> paths;
        std::string temps;
        std::ifstream in;
        
        if(is_file(file).value && !is_symlink(file).value)
        {
            in.open(file.c_str(), std::ios::binary);
            //limit the scope while we skip the header:
            {
                snapshot::snapshot_data tempsnap;
                snapshot::in_header(in, tempsnap);
            }
            in.peek();
            while(in.good() && (in.peek() != struct_delim::value) && (in.peek() != EOF))
            {
                if(common::safe_getline(in, temps, mem_delim::value))
                {
                    paths.insert(temps);
                }
            }
            in.close();
        }
        return paths;
    }
    
    inline void diff_snapshots(const std::string& file_bef, const std::string& file_aft)
    {
        using std::cout;
        using std::endl;
        using fsys::is_file;
        using fsys::is_symlink;
        using snapshot::load_header;
        using snapshot::snapshot_data;
        
        common::cls();
        for(unsigned int x = 0; x < v_center::value; x++) cout<< endl;
        common::center("Comparing, please wait...");
        cout.flush();
        
        std::string file, newbef(file_bef), newaft(file_aft);
        std::unordered_set<std::string> paths_before, paths_after;
        tdata::time_class now;
        std::string temps;
        std::ofstream out;
        
        auto save_diff_result = [](const std::vector<std::string>& paths, 
                        std::ofstream& out, const std::string& title)->void
        {
            using std::endl;
            
            out<< title<< endl<< endl;
            for(std::vector<std::string>::const_iterator it = paths.begin(); it != paths.end(); ++it)
            {
                out<< *it<< endl;
            }
        };
        
        if(create_record_folder(common::parent_folder(file_bef)))
        {
            snapshot_data before, after;
            
            if(load_header(before, file_bef) && load_header(after, file_aft))
            {
                if(after.timestamp < before.timestamp)
                {
                    std::swap(before, after);
                    std::swap(newbef, newaft);
                }
                paths_before = load_paths(newbef);
                paths_after = load_paths(newaft);
                
                now = tdata::current_time();
                temps = (record_folder(common::parent_folder(file_bef)) + fsys::pref_slash() + 
                                now.month_name() + " " + std::to_string(now.mday()) + 
                                ", " + std::to_string(now.gyear()) + "  at " + 
                                std::to_string(now.hour()) + " " + 
                                std::to_string(now.minute()) + " " + 
                                std::to_string(now.second()) + ".txt");
                
                out.open(temps.c_str(), std::ios::out);
                
                if(!out.good()) ethrow(("Out is not good! (file: \"" + temps + "\")"));
                
                out<< "Time frame:  "<< display_time(before.timestamp)<< " - "<< 
                                display_time(after.timestamp)<< endl;
                
                save_diff_result(difference_between(paths_before, paths_after), 
                                out, (std::string(5, '\n') + "DELETED PATHS: "));
                                
                save_diff_result(difference_between(paths_after, paths_before), 
                                out, (std::string(5, '\n') + "CREATED PATHS: "));
                                
                paths_before.erase(paths_before.begin(), paths_before.end());
                paths_after.erase(paths_after.begin(), paths_after.end());
                if(out.is_open()) out.close();
            }
        }
        
        if(is_file(temps).value && !is_symlink(temps).value)
        {
            common::cls();
            for(unsigned int x = 0; x < v_center::value; x++) cout<< endl;
            common::center("DONE!");
            cout<< endl;
            cout<< endl;
            common::center("Records saved in: \"" + temps + "\"");
            common::wait();
        }
    }
    
    inline std::vector<snapshot::snapshot_data> load_all_headers(const std::string& folder)
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
    
    /** Returns the path of a snapshot if it has been saved.  Otherwise it
     * will return nothing. */
    inline std::string file_of_snapshot(const snapshot::snapshot_data& snap, const std::string& folder)
    {
        std::string temps;
        std::unordered_map<unsigned long long, std::string> ids;
        
        if(snap.id != 0)
        {
            ids = snapshot::list_ids(folder);
            if(!ids.empty())
            {
                if(ids.find(snap.id) != ids.end())
                {
                    temps = ids[snap.id];
                }
            }
        }
        return temps;
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
        common_menu::selection_class selection;
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
            cout<< "(F5:  HELP)";
            cout<< endl;
            common::center("Snapshot Menu:");
            cout<< endl;
            
            for(unsigned int x = 0; x < 2; x++) cout<< endl;
            common_menu::display_scroll_window(window, display.size(), selection);
            
            for(unsigned int x = 0; x < 1; x++) cout<< endl;
            cout<< " [SPC] -  Select"<< endl;
            cout<< " n -  NEW snapshot"<< endl;
            cout<< " c -  Compare snaps"<< endl;
            cout<< " \\ -  clear selection"<< endl;
            cout<< " e -  Exit";
            cout.flush();
            
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
                                if(common::inp::is_sure("Do you really want to \
delete the snapshot taken on " + display_time(snapshots.at(window.gpos().whole).timestamp) + "?"))
                                {
                                    //some user feedback for those with slow computers:
                                    common::cls();
                                    for(unsigned int x = 0; x < v_center::value; x++) cout<< endl;
                                    common::center("Please wait while I delete the snapshot...");
                                    cout.flush();
                                    
                                    //only erase the snapshot from the list if we successfully delete the associated file:
                                    if(remove_snapshot(snapshots.at(window.gpos().whole), folder)) 
                                    {
                                        snapshots.erase(snapshots.begin() + window.gpos().whole);
                                        selection.clear();
                                    }
                                    
                                    common::cls();
                                    for(unsigned int x = 0; x < v_center::value; x++) cout<< endl;
                                    common::center("DONE!");
                                    cout.flush();
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
                                            update_display();
                                        }
                                    }
                                }
                                selection.clear();
                            }
                        }
                        break;
                        
                        case 'c':
                        {
                            if(selection.gselection().size() > 1)
                            {
                                diff_snapshots(file_of_snapshot(snapshots[selection[0]], folder), 
                                                file_of_snapshot(snapshots[selection[1]], folder));
                                selection.clear();
                            }
                        }
                        break;
                        
                        case '\\':
                        {
                            selection.clear();
                        }
                        break;
                        
                        case ' ':
                        {
                            if(!snapshots.empty())
                            {
                                if(!selection.is_selected(window.gpos().whole) && (selection.gselection().size() < 2)) selection.add(window.gpos().whole);
                                else if(selection.is_selected(window.gpos().whole)) selection.remove(window.gpos().whole);
                            }
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