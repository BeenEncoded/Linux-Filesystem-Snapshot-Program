#ifndef PROGRAM_SETTINGS_HPP_INCLUDED
#define PROGRAM_SETTINGS_HPP_INCLUDED
#include <vector>
#include <string>
#include <utility>
#include <iostream>

namespace settings
{
    typedef struct settings_data settings_data;
    typedef struct regex_data regex_data;
    typedef struct regex_settings_data regex_settings_data;
    typedef struct setting_constants_data setting_constants_data;
    
    std::istream& operator>>(std::istream&, regex_data&);
    std::ostream& operator<<(std::ostream&, const regex_data&);
    
    std::istream& operator>>(std::istream&, regex_settings_data&);
    std::ostream& operator<<(std::ostream&, const regex_settings_data&);
    
    std::ostream& operator<<(std::ostream&, const settings_data&);
    std::istream& operator>>(std::istream&, settings_data&);
    
    std::pair<std::vector<regex_data>, std::vector<regex_data> > used_expressions(const settings_data&);
    
    /* One Regular expression. */
    typedef struct regex_data
    {
        explicit regex_data() noexcept;
        ~regex_data() noexcept {}
        
        regex_data& operator=(const regex_data&) noexcept;
        bool operator==(const regex_data&) const noexcept;
        bool operator!=(const regex_data&) const noexcept;
        
        bool on : 1;
        std::string s;
    } regex_data;
    
    /* A list of regular expression to match, and !match against, as well
     * as a bool that specifies whether to use this feature at all. */
    typedef struct regex_settings_data
    {
        explicit regex_settings_data() noexcept;
        ~regex_settings_data() noexcept {}
        
        regex_settings_data& operator=(const regex_settings_data&) noexcept;
        bool operator==(const regex_settings_data&) const noexcept;
        bool operator!=(const regex_settings_data&) const noexcept;
        
        bool use_regex : 1, use_match : 1, use_not_match : 1;
        std::vector<regex_data> match, not_match;
    } regex_settings_data;
    
    /** Defines global program constants such as folders, etc... */
    typedef struct setting_constants_data
    {
        //modify the constructor's initializer to change the values.
        explicit setting_constants_data() noexcept;
        ~setting_constants_data() noexcept;
        
        setting_constants_data& operator=(const setting_constants_data&) noexcept;
        bool operator==(const setting_constants_data&) const noexcept;
        bool operator!=(const setting_constants_data&) const noexcept;
        
        std::string snapshot_folder, records_folder, settings_folder;
    } setting_constants_data;
    
    /** Program settings */
    typedef struct settings_data
    {
        explicit settings_data() noexcept;
        ~settings_data() noexcept {}
        
        settings_data& operator=(const settings_data&) noexcept;
        bool operator==(const settings_data&) const noexcept;
        bool operator!=(const settings_data&) const noexcept;
        
        regex_settings_data regex_settings;
        std::string editor;
        const setting_constants_data global;
    } settings_data;
    
    
}

#endif