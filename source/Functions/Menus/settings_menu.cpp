#include <iostream>

#include "settings_menu.hpp"
#include "program_settings.hpp"
#include "common_menu.hpp"
#include "common.hpp"
#include "global_defines.hpp"

namespace
{
}

namespace menu
{
    common_menu::menu_return_data modify_regex_settings(settings::regex_settings_data& orig_settings)
    {
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
            cout<< " [BCKSPCE] -  Done"<< endl;
            
            key = common::gkey_funct();
            if(!key.is_control)
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
                        switch(key.control_d[0])
                        {
                            case '1':
                            {
                                settings.use_regex = !settings.use_regex;
                                if(!result.modified) result.modified = true;
                            }
                            break;
                            
                            case '2':
                            {
                                settings.use_match = !settings.use_match;
                                if(!result.modified) result.modified = true;
                            }
                            break;
                            
                            case '3':
                            {
                                //cur_pos
                                if(!result.modified) result.modified = true;
                            }
                            break;
                            
                            case '4':
                            {
                                if(!result.modified) result.modified = true;
                            }
                            break;
                            
                            case '5':
                            {
                                if(!result.modified) result.modified = true;
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
            else if(key.is_control)
            {
                
            }
        }while(!finished);
        
        if(result.modified && !result.canceled) orig_settings = settings;
        return result;
    }
    
    common_menu::menu_return_data modify_program_settings(settings::settings_data& cur_settings)
    {
        using settings::settings_data;
        using common_menu::menu_return_data;
        
        menu_return_data result;
        
        
        return result;
    }
    
    
}