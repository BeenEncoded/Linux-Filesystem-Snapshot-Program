#ifndef TIME_CLASS_HPP_INCLUDED
#define TIME_CLASS_HPP_INCLUDED
#include <ctime>
#include <iostream>

#include "global_defines.hpp"

namespace tdata
{
    typedef std::chrono::system_clock clock_type;
    typedef std::chrono::time_point<clock_type> time_point;
    
    class time_class;
    
    std::ostream& operator<<(std::ostream&, time_point&);
    std::istream& operator>>(std::istream&, time_point&);
    
    const struct tm& tmtotp(struct tm&, const time_point&);
    const time_point& tptotm(time_point&, struct tm&);
    
    class time_class
    {
    public:
    
        //time constants: 
        typedef const_int_type<60> vminute;
        typedef const_int_type<(vminute::value * 60)> vhour;
        typedef const_int_type<(vhour::value * 24)> vday;
    
        explicit time_class();
        ~time_class();
        
        const struct tm& value() const;
        
        const time_class& operator=(const time_class&);
        const time_class& operator=(const struct tm&);
        
        //comparisons
        bool operator==(const time_class&) const;
        bool operator!=(const time_class&) const;
        bool operator<(const time_class&) const;
        bool operator>(const time_class&) const;
        bool operator<=(const time_class&) const;
        bool operator>=(const time_class&) const;
        
        //mathematical ops: 
        const time_class& operator++(int);
        time_class        operator+(int) const;
        const time_class& operator+=(int);
        const time_class& operator--(int);
        time_class        operator-(int) const;
        const time_class& operator-=(int);
        
        /* data retrieval.  I hate to do this, but the information
         * must be modified before reads and after writes. */
        
        int& second();//0-59
        int& mhour(); //0-23
        int& minute();//0-59
        int& month(); //0-11 
        int& wday();  //0-6
        int hour() const;
        bool am() const;
        int gyear() const;
        void syear(const int&);
        std::string month_name();
        std::string wday_name();
        
    private:
        struct tm cur_time;
        
        void sync_to_yday();
        
    };
    
    struct tm current_time();
    
}

#endif