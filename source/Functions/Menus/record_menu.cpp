#include <string>
#include <iostream>
#include <vector>

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
        std::size_t pos(s.rfind(fsys::pref_slash()));
        if(pos != std::string::npos)
        {
            temps.erase(temps.begin(), (temps.begin() + pos + 1));
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
    
    
}

namespace menu
{
    common_menu::menu_return_data manage_records(settings::settings_data& psettings)
    {
        using common_menu::menu_return_data;
        using scrollDisplay::window_data_class;
        using std::cout;
        using std::endl;
        
        menu_return_data result;
        bool finished(false);
        std::vector<std::string> record_files(paths_of_ext(psettings.global.records_folder, ".txt"));
        window_data_class<std::string> window(record_files, update_display);
        
        do
        {
            common::cls();
            cout<< endl;
            common::center("Record Management");
            cout<< std::string(4, '\n');
            cout.flush();
            
            
        }while(!finished);
        return result;
    }
    
    
}