#ifndef COMMON_HPP_INCLUDED
#define COMMON_HPP_INCLUDED
#include <vector>
#include <ctime>

#include "global_defines.hpp"

namespace common
{
    void cls();
    void cl();
    key_code::key_code_data gkey();
    key_code::key_code_data gkey_funct();
    key_code::key_code_data getch_funct();
    void wait();
    bool is_letter(const char&);
    bool is_number(const char&);
    bool is_special(const char&);
    bool is_char(const char&);
    void center(const std::string&);
    struct tm get_time();
    
    namespace inp
    {
        bool is_sure(const std::string&);
        std::string get_user_string(const std::string&);
        
    }
    
    
}

#endif
