#ifndef PROGRAM_SETTINGS_HPP_INCLUDED
#define PROGRAM_SETTINGS_HPP_INCLUDED
#include <regex>
#include <vector>

namespace settings
{
    struct settings_data;
    struct regex_data;
    struct regex_settings_data;
    
    
    /* One Regular expression. */
    struct regex_data
    {
        /* cur_pos write operators for each settings structure.  We'll also 
         * need stream operators for saving them. */
        regex_data& operator=(const regex_data&);
        bool operator==(const regex_data&) const;
        bool operator!=(const regex_data&) const;
        
        bool on : 1;
        std::string s;
    };
    
    struct regex_settings_data
    {
        bool use_regex, use_match, use_not_match;
        std::vector<regex_data> match, not_match;
    };
    
    struct settings_data
    {
        regex_settings_data regex_settings;
    };
    
    
}

#endif