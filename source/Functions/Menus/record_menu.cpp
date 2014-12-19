#include <string>
#include <iostream>
#include <vector>
#include <unistd.h>
#include <thread>

#include "program_settings.hpp"
#include "filesystem.hpp"
#include "common.hpp"
#include "record_menu.hpp"
#include "scroll_display.hpp"

namespace
{
    void update_display(const std::vector<std::string>&, std::vector<std::string>&);
    std::string filename(const std::string&);
    std::string file_ext(const std::string&);
    std::vector<std::string> paths_of_ext(const std::string&, const std::string&);
    std::vector<fsys::result_data_boolean> delete_folder(const std::string&);
    
    
    
    template<typename type>
    bool is_in_tor(const std::vector<type>& t, bool (*match)(const type& t))
    {
        for(typename std::vector<type>::const_iterator it = t.begin(); it != t.end(); ++it)
        {
            if(match(*it)) return true;
        }
        return false;
    }
    
    inline std::string filename(const std::string& s)
    {
        std::string temps(s);
        std::size_t pos(temps.rfind(fsys::pref_slash()));
        if(pos != std::string::npos)
        {
            temps.erase(temps.begin(), (temps.begin() + pos + 1));
        }
        return temps;
    }
    
    inline std::string file_ext(const std::string& s)
    {
        std::string temps(s);
        std::size_t pos(s.rfind('.'));
        if(pos != std::string::npos)
        {
            temps.erase(temps.begin(), (temps.begin() + pos));
        }
        return temps;
    }
    
    inline void update_display(const std::vector<std::string>& paths, std::vector<std::string>& display)
    {
        display.clear();
        display.shrink_to_fit();
        for(unsigned int x = 0; x < paths.size(); ++x)
        {
            display.push_back(filename(paths[x]));
        }
    }
    
    inline std::vector<std::string> paths_of_ext(const std::string& folder, const std::string& ext)
    {
        using fsys::tree_iterator_class;
        using fsys::is_file;
        using fsys::is_symlink;
        using fsys::is_folder;
        
        std::vector<std::string> files;
        
        if(is_folder(folder).value && !is_symlink(folder).value)
        {
            for(tree_iterator_class it(folder); !it.at_end(); ++it)
            {
                if(is_file(it.value()).value && !is_symlink(it.value()).value)
                {
                    if(file_ext(it.value()) == ext) files.push_back(it.value());
                }
            }
        }
        return files;
    }
    
    inline std::vector<fsys::result_data_boolean> delete_folder(const std::string& f)
    {
        using fsys::is_folder;
        using fsys::is_file;
        using fsys::is_symlink;
        using fsys::delete_iterator_class;
        using std::cout;
        using std::endl;
        using fsys::result_data_boolean;
        
        std::vector<result_data_boolean> results;
        
        if(is_folder(f).value && !is_symlink(f).value)
        {
            for(delete_iterator_class it(f); !it.at_end(); ++it)
            {
                results.push_back(it.success);
                if(!it.success.value) it.skip();
            }
        }
        return results;
    }
    
    /** passed to is_in_tor to match results that indicate failures. */
    inline bool match_fres(const fsys::result_data_boolean& res)
    {
        return !res.value;
    }
    
    
}

namespace menu
{
    common_menu::menu_return_data manage_records(settings::settings_data& psettings)
    {
        using common_menu::menu_return_data;
        using scrollDisplay::window_data_class;
        using std::cout;
        using std::endl;
        using key_code::key_code_data;
        using fsys::is_file;
        using fsys::is_symlink;
        using fsys::fdelete;
        
        menu_return_data result;
        bool finished(false);
        std::vector<std::string> record_files(paths_of_ext(psettings.global.records_folder, ".txt"));
        window_data_class<std::string> window(record_files, update_display);
        key_code_data ch;
        
        window.win().window_size() = common_menu::wsize::value;
        
        do
        {
            common::cls();
            cout<< endl;
            common::center("Record Management");
            cout<< std::string(4, '\n');
            cout.flush();
            common_menu::display_scroll_window(window.win(), record_files.size());
            cout<< endl<< endl;
            cout<< " [del] -  Delete record"<< endl;
            cout<< " a -  Delete All records"<< endl;
            cout<< " [BCKSPC] -  Done"<< endl;
            cout<< " [ENTR] -  View Record"<< endl;
            
            /** cur_pos
             * make it so that this menu exits when there are no records left */
            ch = common::gkey_funct();
            if(key_code::is_listed_control(ch))
            {
                using key_code::keys;
                using namespace key_code::code;
                if(ch == keys[backspace::value]) finished = true;
                else if(ch == keys[up::value]) window.win().mv_up();
                else if(ch == keys[down::value]) window.win().mv_down();
                else if(ch == keys[pgup::value]) window.win().pg_up();
                else if(ch == keys[pgdown::value]) window.win().pg_down();
                else if(ch == keys[home::value]) while(window.win().pg_down());
                else if(ch == keys[end::value]) while(window.win().pg_up());
                else if(ch == keys[del::value])
                {
                    if(!record_files.empty())
                    {
                        if(common::inp::is_sure("Do you really want to delete \
the record \"" + window.selected() + "\"?"))
                        {
                            if(is_file(window.selected()).value && !is_symlink(window.selected()).value)
                            {
                                fsys::result_data_boolean tempres(fdelete(window.selected()));
                                if(!tempres.value)
                                {
                                    common::cls();
                                    cout<< std::string(v_center::value, '\n');
                                    common::center("Error, could not delete the record: \"" + tempres.error + "\"");
                                    cout<< endl;
                                    common::wait();
                                    common::cls();
                                    cout<< "please wait...";
                                    cout.flush();
                                }
                                record_files = paths_of_ext(psettings.global.records_folder, ".txt");
                            }
                        }
                    }
                }
            }
            else
            {
                if(!ch.is_control && !ch.control_d.empty())
                {
                    switch(std::tolower((char)ch.control_d.front()))
                    {
                        case 'a':
                        {
                            if(!record_files.empty())
                            {
                                if(common::inp::is_sure("Do you really want to delete \
all snapshot comparison records?  This can not be undone!"))
                                {
                                    if(is_in_tor(delete_folder(psettings.global.records_folder),  match_fres))
                                    {
                                        common::cls();
                                        cout<< std::string(v_center::value, '\n');
                                        cout<< "Errors occured!"<< endl;
                                        common::wait();
                                        common::cls();
                                        cout<< "please wait...";
                                        cout.flush();
                                    }
                                    record_files = paths_of_ext(psettings.global.records_folder, ".txt");
                                    if(record_files.empty()) finished = true;
                                }
                            }
                        }
                        break;
                        
                        case '\n':
                        {
                            if(fsys::is_file(psettings.editor).value && !fsys::is_symlink(psettings.editor).value)
                            {
                                if(!record_files.empty())
                                {
                                    common::cls();
                                    cout<< "Please exit the program to continue...\n";
                                    cout.flush();
                                    std::system((psettings.editor + " \"" + window.selected() + "\"").c_str());
                                }
                            }
                            else
                            {
                                common::cls();
                                cout<< std::string(v_center::value, '\n');
                                common::center("The editor can not be run!");
                                cout.flush();
                                common::wait();
                                common::cls();
                            }
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