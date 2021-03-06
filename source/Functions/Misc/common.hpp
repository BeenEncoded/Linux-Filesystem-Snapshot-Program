#ifndef COMMON_HPP_INCLUDED
#define COMMON_HPP_INCLUDED
#include <iostream>
#include <string>
#include <vector>
#include <ctime>
#include <sstream>
#include <unordered_set>

#include "global_defines.hpp"
#include "scroll_display.hpp"

namespace common
{
    //stream safety function:
    namespace
    {
        /*Safely retrieves a line from a file given a specified delimiter.  
         In case of failure, it will leave the stream in the state it was in
         before the function call.  Returns true/false based on success. */
        template<class type>
        inline bool safe_getline(std::istream& in, type& t, const char& delimiter)
        {
            std::string temps;
            bool success(false);
            std::istream::pos_type previous_position(in.tellg());
            
            t = type();
            
            if(in.good())
            {
                std::getline(in, temps, delimiter);
                if(in.good())
                {
                    std::stringstream ss;
                    ss<< temps;
                    ss>> t;
                    success = true;
                }
                else if(!in.eof() && in.fail())
                {
                    /* So, here we are: we didn't reach the end of the file, 
                     but somwhere there was a big mistake... the program will
                     now attempt to salvage the situation: */
                    in.seekg(previous_position);
                    success = false;
                }
            }
            return success;
        }
        
        template<>
        inline bool safe_getline<std::string>(std::istream& in, std::string& s, const char& delimiter)
        {
            bool success(false);
            std::istream::pos_type previous_position(in.tellg());
            
            s.erase();
            
            if(in.good())
            {
                std::getline(in, s, delimiter);
                if(in.good()) success = true;
                else if(!in.eof() && in.fail())
                {
                    /* So, here we are: we didn't reach the end of the file, 
                     but somwhere there was a big mistake... the program will
                     now attempt to salvage the situation: */
                    in.seekg(0, std::ios::beg);
                    in.seekg(previous_position);
                    success = false;
                }
            }
            return success;
        }
        
        
    }
    
    void cls() noexcept;
    void cl() noexcept;
    bool kbhit() noexcept;
    key_code::key_code_data gkey();
    key_code::key_code_data gkey_funct();
    key_code::key_code_data getch_funct();
    void wait() noexcept;
    bool is_letter(const char&) noexcept;
    bool is_number(const char&) noexcept;
    bool is_special(const char&) noexcept;
    bool is_char(const char&) noexcept;
    void center(const std::string&) noexcept;
    struct tm get_time();
    std::string parent_folder(const std::string&) noexcept;
    bool string_begins_with(const std::string&, const std::string&) noexcept;
    bool string_ends_with(const std::string&, const std::string&) noexcept;
    
    namespace inp
    {
        bool is_sure(const std::string&);
        std::string get_user_string(const std::string&);
        
    }
    
    
}

#endif