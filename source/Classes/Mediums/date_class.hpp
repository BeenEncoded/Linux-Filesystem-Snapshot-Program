#ifndef DATE_CLASS_HPP_INCLUDED
#define DATE_CLASS_HPP_INCLUDED
#include <ctime>

#define YDAYS(year) (__isleap(year) ? 366 : 365)

//testing required
namespace date
{
    struct date_val;
    
    std::string display(const date_val&);
    
    /* Used to apply math to dates.  Allows
     addition and subraction of days from/to
     dates.  Simple object to allow flexible manipulation
     of dates, and converting that into, and from, struct tm.*/
    struct date_val
    {
        void operator=(const date_val&);
        void operator=(const struct tm&);
        
        bool operator==(const date_val&) const;
        bool operator!=(const date_val&) const;
        bool operator<(const date_val&) const;
        bool operator<=(const date_val&) const;
        
        void to_tm(struct tm&) const;
        
        date_val operator++(int);
        date_val operator--(int);
        
        date_val operator+(const unsigned int&) const;
        date_val operator-(const unsigned int&) const;
        
        date_val operator+=(const unsigned int&);
        date_val operator-=(const unsigned int&);
        
        std::string month_name() const;
        std::string wday_name() const;
        
        /** all vars start from 0. (except for the year)*/
        unsigned int year = 0, yday = 0;
        unsigned short month = 0, mday = 0, weekday = 0;
    };
}

#endif
