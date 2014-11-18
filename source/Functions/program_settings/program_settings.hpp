#ifndef PROGRAM_SETTINGS_HPP_INCLUDED
#define PROGRAM_SETTINGS_HPP_INCLUDED
#include <regex>
#include <vector>
#include <iostream>

namespace settings
{
    typedef struct settings_data settings_data;
    typedef struct regex_data regex_data;
    typedef struct regex_settings_data regex_settings_data;
    
    
    std::istream& operator>>(std::istream&, regex_data&);
    std::ostream& operator<<(std::ostream&, const regex_data&);
    
    /* One Regular expression. */
    typedef struct regex_data
    {
        explicit regex_data() noexcept
        {
        }
        
        ~regex_data() noexcept
        {
        }
        
        /* cur_pos write operators for each settings structure.  We'll also 
         * need stream operators for saving them. */
        regex_data& operator=(const regex_data&) noexcept;
        bool operator==(const regex_data&) const noexcept;
        bool operator!=(const regex_data&) const noexcept;
        
        bool on : 1;
        std::string s;
    } regex_data;
    
    typedef struct regex_settings_data
    {
        bool use_regex, use_match, use_not_match;
        std::vector<regex_data> match, not_match;
    } regex_settings_data;
    
    typedef struct settings_data
    {
        regex_settings_data regex_settings;
    } settings_data;
    
    
}

#endif