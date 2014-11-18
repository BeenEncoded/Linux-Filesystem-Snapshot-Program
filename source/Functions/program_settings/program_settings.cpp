#include <string>
#include <vector>
#include <regex>
#include <iostream>
#include <cstring>

#include "program_settings.hpp"
#include "global_defines.hpp"
#include "common.hpp"

namespace
{
    
    
    template<typename type>
    char* get_mem(type& t) noexcept
    {
        char* ch = new char[sizeof(type)];
        std::memcpy(ch, &t, sizeof(type));
        return ch;
    }
    
    template<char*>
    char* get_mem(char*& t)
    {
        ethrow("[PROGRAMMING ERROR]: Can not convert char* to char* through memory!");
        return NULL;
    }
    
    template<typename type>
    type from_mem(const char* ch) noexcept
    {
        type t;
        std::memcpy(&t, ch, sizeof(type));
        return t;
    }
    
    template<char*>
    char* from_mem(const char* ch)
    {
        ethrow("[PROGRAMMING ERROR]: Can not convert char* to char* through memory!");
        return NULL;
    }
    
    template<typename type>
    void out_memory_of(std::ostream& out, type& t)
    {
        char *ch(get_mem(t));
        out<< ch;
        delete[] ch;
    }
    
    template<typename type>
    void in_memory_of(std::istream& in, type& t)
    {
        char *ch(new char[sizeof(type)]);
        in.peek();
        for(unsigned int x = 0; ((x < sizeof(type)) && in.good()); ++x)
        {
            ch[x] = in.get();
            if(ch[x] == EOF) ch[x] = '\0';
        }
        t = from_mem<type>(ch);
        delete[] ch;
    }
    
    
}

/** regex_data member functions: */
namespace settings
{
    regex_data& regex_data::operator=(const regex_data& reg) noexcept
    {
        if(this != &reg)
        {
            this->on = reg.on;
            this->s = reg.s;
        }
        return *this;
    }
    
    bool regex_data::operator==(const regex_data& r) const noexcept
    {
        return (
                    (this->s == r.s) && 
                    (this->on == r.on));
    }
    
    bool regex_data::operator!=(const regex_data& r) const noexcept
    {
        return !(this->operator==(r));
    }
    
    std::ostream& operator<<(std::ostream& out, const regex_data& r)
    {
        if(out.good())
        {
            out<< r.s<< mem_delim::value;
            out_memory_of(out, r.on);
        }
        return out;
    }
    
    std::istream& operator>>(std::istream& in, regex_data& r)
    {
        r = regex_data();
        
        in.peek();
        if(in.good())
        {
            bool tempb;
            if(common::safe_getline(in, r.s, mem_delim::value))
            {
                in_memory_of(in, tempb);
                r.on = tempb;
            }
        }
        in.peek();
        return in;
    }
    
    
}

/** regex_settings_data member functions: */
namespace settings
{
    regex_settings_data& regex_settings_data::operator=(const regex_settings_data& r) noexcept
    {
        if(this != &r)
        {
            this->use_regex = r.use_regex;
            this->use_match = r.use_match;
            this->use_not_match = r.use_not_match;
            this->match = r.match;
            this->not_match = r.not_match;
        }
        return *this;
    }
    
    bool regex_settings_data::operator==(const regex_settings_data& r) const noexcept
    {
        return ((this->use_regex == r.use_regex) && 
                (this->use_match == r.use_match) && 
                (this->use_not_match == r.use_not_match) && 
                (this->match == r.match) && 
                (this->not_match == r.not_match));
    }
    
    bool regex_settings_data::operator!=(const regex_settings_data& r) const noexcept
    {
        return !(this->operator==(r));
    }
    
    std::ostream& operator<<(std::ostream& out, const regex_settings_data& r) noexcept
    {
        if(out.good())
        {
            out_memory_of(out, r.use_regex);
            out_memory_of(out, r.use_match);
            out_memory_of(out, r.use_not_match);
            for(std::vector<regex_data>::const_iterator it = r.match.begin(); it != r.match.end(); ++it)
            {
                out<< *it;
            }
            out<< struct_delim::value;
            for(std::vector<regex_data>::const_iterator it = r.not_match.begin(); it != r.not_match.end(); ++it)
            {
                out<< *it;
            }
            out<< struct_delim::value;
        }
        return out;
    }
    
    std::istream& operator>>(std::istream& in, regex_settings_data& r) noexcept
    {
        if(in.good())
        {
            bool tempb;
            in_memory_of(in, tempb);
            r.use_regex = tempb;
            in_memory_of(in, tempb);
            r.use_match = tempb;
            in_memory_of(in, tempb);
            r.use_not_match = tempb;
            in.peek();
            while(in.good() && (in.peek() != EOF) && (in.peek() != struct_delim::value))
            {
                r.match.push_back(regex_data());
                in>> r.match.back();
            }
            if(in.peek() == struct_delim::value) in.get();
            while(in.good() && (in.peek() != EOF) && (in.peek() != struct_delim::value))
            {
                r.match.push_back(regex_data());
                in>> r.not_match.back();
            }
            if(in.peek() == struct_delim::value) in.get();
        }
        in.peek();
        return in;
    }
    
    
}

/** settings_data member functions: */
namespace settings
{
    settings_data& settings_data::operator=(const settings_data& s) noexcept
    {
        if(this != &s)
        {
            this->regex_settings = s.regex_settings;
        }
        return *this;
    }
    
    bool settings_data::operator==(const settings_data& s) const noexcept
    {
        return (this->regex_settings == s.regex_settings);
    }
    
    bool settings_data::operator!=(const settings_data& s) const noexcept
    {
        return (this->regex_settings != s.regex_settings);
    }
    
    
}

