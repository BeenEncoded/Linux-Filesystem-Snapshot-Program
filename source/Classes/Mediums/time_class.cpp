#include <ctime>
#include <chrono>
#include <iostream>

#include "time_class.hpp"
#include "global_defines.hpp"
#include "common.hpp"

namespace
{
}

/** Stream operators: */
namespace tdata
{
    std::ostream& operator<<(std::ostream& out, time_point& t)
    {
        if(out.good())
        {
            out<< std::chrono::duration_cast<std::chrono::seconds>(t.time_since_epoch()).count()<< mem_delim::value;
        }
        return out;
    }
    
    std::istream& operator>>(std::istream& in, time_point& t)
    {
        long tempi(0);
        t = time_point(std::chrono::seconds(0));
        in.peek();
        if(in.good())
        {
            if(common::safe_getline(in, tempi, mem_delim::value))
            {
                t = time_point(std::chrono::seconds(tempi));
            }
        }
        return in;
    }
    
    const time_point& tptotm(time_point& t, struct tm& temptm)
    {
        t = std::chrono::system_clock::from_time_t(mktime(&temptm));
        return t;
    }
    
    const struct tm& tmtotp(struct tm& t, const time_point& tempt)
    {
        time_t lvalue_timet(std::chrono::system_clock::to_time_t(tempt));
        t = (*localtime(&lvalue_timet));
        return t;
    }
    
    
}

/** Time_class member functions: */
namespace tdata
{
}

/** misc functions: */
namespace tdata
{
    struct tm current_time()
    {
        return tptotm(std::chrono::system_clock::now());
    }
    
    
}

