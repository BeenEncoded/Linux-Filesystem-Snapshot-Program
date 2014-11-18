#include <string>
#include <vector>
#include <regex>
#include <iostream>

#include "program_settings.hpp"
#include "global_defines.hpp"
#include "common.hpp"

namespace
{
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
            out<< r.on<< mem_delim::value;
        }
        return out;
    }
    
    std::istream& operator>>(std::istream& in, regex_data& r)
    {
        r = regex_data();
        
        in.peek();
        if(in.good())
        {
            if(common::safe_getline(in, r.s, mem_delim::value))
            {
                bool tempb(false);
                common::safe_getline(in, tempb, mem_delim::value);
                r.on = tempb;
            }
        }
        in.peek();
        return in;
    }
    
    
}
