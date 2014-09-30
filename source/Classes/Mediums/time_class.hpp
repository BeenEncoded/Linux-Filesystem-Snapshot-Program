#ifndef TIME_CLASS_HPP_INCLUDED
#define TIME_CLASS_HPP_INCLUDED
#include <chrono>
#include <ctime>
#include <ratio>
#include <iostream>

namespace tdata
{
    typedef std::chrono::system_clock clock_type;
    typedef std::chrono::time_point<clock_type> time_point;
    
    std::ostream& operator<<(std::ostream&, time_point&);
    std::istream& operator>>(std::istream&, time_point&);
    
    const struct tm& tmtotp(struct tm&, const time_point&);
    const time_point& tptotm(time_point&, struct tm&);
    
    class time_class
    {
    public:
        explicit time_class();
        ~time_class();
        
        const time_point& value() const;
        
        
    private:
        time_point cur_time;
        
    };
    
    struct tm current_time();
    
}

#endif