#include <iostream>
#include <vector>
#include <string>

#include "settings_menu.hpp"
#include "program_settings.hpp"
#include "scroll_display.hpp"
#include "common_menu.hpp"
#include "common.hpp"
#include "global_defines.hpp"

namespace
{
    void regex_list_display(const std::vector<settings::regex_data>&, 
            std::vector<std::string>&);
    
    
    
    inline void regex_list_display(const std::vector<settings::regex_data>& data, 
                    std::vector<std::string>& display)
    {
        using settings::regex_data;
        
        std::size_t max_size(0);
        for(std::vector<regex_data>::const_iterator it = data.begin(); it != data.end(); ++it)
        {
            if(max_size < it->s.size()) max_size = it->s.size();
            if(max_size > 50)
            {
                max_size = 50;
                break;
            }
        }
        display.clear();
        for(std::vector<regex_data>::const_iterator it = data.begin(); it != data.end(); ++it)
        {
            display.push_back(it->s);
            if(display.back().size() > max_size) display.back().resize(max_size);
            display.back() += std::string(((max_size + 5) - display.back().size()), ' ');
            display.back() += (it->on ? "ON" : "OFF");
        }
    }
    
    
}

namespace menu
{
    /** Allows the user to modify a list of regex_data. */
    common_menu::menu_return_data modify_regex_list(std::vector<settings::regex_data>& orig_list, const std::string& message)
    {
        //test
        using scrollDisplay::window_data_class;
        using common_menu::display_scroll_window;
        using common_menu::menu_return_data;
        using std::cout;
        using settings::regex_data;
        using key_code::key_code_data;
        using std::endl;
        
        bool finished(false);
        
        std::vector<regex_data> regex_list(orig_list);
        window_data_class<regex_data> window(regex_list, regex_list_display);
        menu_return_data result;
        key_code_data key;
        
        do
        {
            common::cls();
            cout<< '\n';
            common::center(message);
            cout<< std::string(4, '\n');
            cout.flush();
            common_menu::display_scroll_window(window.win(), regex_list.size());
            cout<< std::string(3, '\n');
            cout<< "[BCKSPC] - Done  |  [n] - new expression  |  [T] - Toggle enabled  |  [ENTR] - Modify Expression"<< endl;
            
            key = common::gkey_funct();
            if(key_code::is_listed_control(key))
            {
                using namespace key_code::code;
                using key_code::keys;
                
                if(key == keys[backspace::value]) finished = true;
                else if(key == keys[up::value]) window.win().mv_up();
                else if(key == keys[down::value]) window.win().mv_down();
                else if(key == keys[pgdown::value]) window.win().pg_up();
                else if(key == keys[pgup::value]) window.win().pg_down();
                else if(key == keys[home::value]) while(window.win().pg_up());
                else if(key == keys[end::value]) while(window.win().pg_down());
                else if(key == keys[del::value])
                {
                    if(!regex_list.empty())
                    {
                        if(common::inp::is_sure("Do you really want to delete \"" + 
                                window.selected().s + "\"?"))
                        {
                            window.remove_selected();
                            if(!result.modified) result.modified = true;
                        }
                    }
                }
            }
            else
            {
                if(!key.is_control)
                {
                    if(!key.control_d.empty())
                    {
                        switch(std::tolower((char)key.control_d[0]))
                        {
                            case 't':
                            {
                                if(!regex_list.empty())
                                {
                                    window.selected().on = !window.selected().on;
                                    if(!result.modified) result.modified = true;
                                }
                            }
                            break;
                            
                            case 'n':
                            {
                                std::string temps(common::inp::get_user_string("Enter an expression: "));
                                if(temps != GSTRING_CANCEL)
                                {
                                    settings::regex_data tempreg;
                                    tempreg.s = temps;
                                    tempreg.on = true;
                                    regex_list.push_back(tempreg);
                                    if(!result.modified) result.modified = true;
                                }
                            }
                            break;
                            
                            case '\n':
                            {
                                if(!regex_list.empty())
                                {
                                    std::string temps;
                                    temps = common::inp::get_user_string("Expression : \"" + 
                                            window.selected().s + "\"" + 
                                            std::string(4, '\n') + "Enter an expression: ");
                                    if(temps != GSTRING_CANCEL)
                                    {
                                        window.selected().s = temps;
                                        if(!result.modified) result.modified = true;
                                    }
                                }
                            }
                            
                            default:
                            {
                            }
                            break;
                        }
                    }
                }
            }
            
        }while(!finished);
        if(result.modified && !result.canceled) orig_list = regex_list;
        return result;
    }
    
    /** Allows the user to modify regular expression settings. */
    common_menu::menu_return_data modify_regex_settings(settings::regex_settings_data& orig_settings)
    {
        //test
        using settings::regex_settings_data;
        using common_menu::menu_return_data;
        using std::cout;
        using std::endl;
        
        menu_return_data result;
        bool finished(false);
        key_code::key_code_data key;
        regex_settings_data settings(orig_settings);
        
        do
        {
            common::cls();
            cout<< endl;
            common::center("Modify Regular Expression Settings");
            cout<< endl;
            for(unsigned int x = 0; x < 3; x++) cout<< endl;
            cout<< "(note: matched strings will not be matched against the \"exclude\" list)"<< endl<< endl;
            cout<< " 1 -  Regular expression filter: "<< (settings.use_regex ? "ON" : "OFF")<< endl;
            if(settings.use_regex)
            {
                cout<< " 2 -  Use matching expressions: "<< (settings.use_match ? "ON" : "OFF")<< endl;
                cout<< " 3 -  Use excluded expressions: "<< (settings.use_not_match ? "ON" : "OFF")<< endl;
                cout<< " 4 -  Edit matching regular expressions"<< endl;
                cout<< " 5 -  Edit excluded regular expressions"<< endl;
            }
            cout<< endl;
            cout<< " C -  Cancel"<< endl;
            cout<< " [BCKSPCE] -  Done"<< endl;
            
            key = common::gkey_funct();
            if(!key_code::is_listed_control(key))
            {
                if(!key.control_d.empty())
                {
                    if(!settings.use_regex && (key.control_d[0] == '1'))
                    {
                        settings.use_regex = !settings.use_regex;
                        if(!result.modified) result.modified = true;
                    }
                    else
                    {
                        if((std::string("12345c").find(std::tolower(key.control_d[0])) != std::string::npos) && 
                                !result.modified)
                        {
                            result.modified = true;
                        }
                        switch(std::tolower(key.control_d[0]))
                        {
                            case '1':
                            {
                                settings.use_regex = !settings.use_regex;
                            }
                            break;
                            
                            case '2':
                            {
                                settings.use_match = !settings.use_match;
                            }
                            break;
                            
                            case '3':
                            {
                                settings.use_not_match = !settings.use_not_match;
                            }
                            break;
                            
                            case '4':
                            {
                                modify_regex_list(settings.match, "Edit matching expressions");
                            }
                            break;
                            
                            case '5':
                            {
                                modify_regex_list(settings.not_match, "Modify excluded expressions");
                            }
                            break;
                            
                            case 'c':
                            {
                                result.canceled = true;
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
            }
            else
            {
                using namespace key_code::code;
                using key_code::keys;
                if(key == keys[backspace::value])
                {
                    finished = true;
                }
            }
        }while(!finished);
        
        if(result.modified && !result.canceled) orig_settings = settings;
        return result;
    }
    
    /** Allows the user to modify program settings */
    common_menu::menu_return_data modify_program_settings(settings::settings_data& cur_settings)
    {
        //test
        using settings::settings_data;
        using common_menu::menu_return_data;
        using std::cout;
        using std::endl;
        using key_code::key_code_data;
        
        menu_return_data result;
        settings_data settings(cur_settings);
        bool finished(false);
        key_code_data key;
        
        do
        {
            common::cls();
            cout<< endl;
            common::center("Program Settings");
            cout<< std::string(4, '\n');
            cout<< " 1 -  Regular Expression Settings ("<< (settings.regex_settings.use_regex ? "ON" : "OFF")<< ")"<< endl;
            cout<< endl;
            cout<< " c -  Cancel"<< endl;
            cout<< " [BCKSPCE] -  Done"<< endl;
            
            key = common::gkey_funct();
            if(!key.control_d.empty())
            {
                if(!key_code::is_listed_control(key))
                {
                    if(!key.is_control)
                    {
                        using namespace key_code::code;
                        using key_code::keys;
                        switch(std::tolower((char)key.control_d[0]))
                        {
                            case '1':
                            {
                                menu_return_data tempres(modify_regex_settings(settings.regex_settings));
                                if(tempres.modified) result.modified = true;
                            }
                            break;
                            
                            case 'c':
                            {
                                result.canceled = true;
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
                else
                {
                    using namespace key_code::code;
                    using key_code::keys;
                    if(key == keys[backspace::value]) finished = true;
                }
            }
        }while(!finished);
        if(result.modified && !result.canceled)
        {
            cur_settings = settings;
        }
        return result;
    }
    
    
}