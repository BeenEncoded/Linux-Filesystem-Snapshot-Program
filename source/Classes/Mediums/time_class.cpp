#include <ctime>
#include <chrono>
#include <iostream>
#include <cstring>

#include "time_class.hpp"
#include "global_defines.hpp"
#include "common.hpp"

namespace
{
    std::vector<unsigned short> month_days(const unsigned int&);
    unsigned int total_days_in_m(const int&, const int&);
    
    
    inline std::vector<unsigned short> month_days(const unsigned int& year)
    {
        return std::vector<unsigned short>({
            31,
                    (__isleap(year) ? (unsigned short)29 : (unsigned short)28),
                    31,
                    30,
                    31,
                    30,
                    31,
                    31,
                    30,
                    31,
                    30,
                    31});
    }
    
    inline unsigned int total_days_in_m(const int& m, const int& year)
    {
        unsigned int days(0);
        
        for(int x = 0, month = 0; x < (m + 1); x++, (month = ((month + 1) % 12)))
        {
            days += month_days(year)[month];
        }
        return days;
    }
    
    
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
    time_class::time_class() : cur_time(current_time())
    {
    }
    
    time_class::~time_class()
    {
    }
    
    const struct tm& time_class::value() const
    {
        return this->cur_time;
    }
    
    const time_class& time_class::operator=(const time_class& t)
    {
        if(this != &t)
        {
            std::memcpy(&(this->cur_time), &t, sizeof(t));
        }
        return *this;
    }
    
    const time_class& time_class::operator=(const struct tm& t)
    {
        if(&(this->cur_time) != (&t))
        {
            std::memcpy(&(this->cur_time), &t, sizeof(t));
        }
        return *this;
    }
    
    bool time_class::operator==(const time_class& t) const
    {
        return ((this->cur_time.tm_sec == t.cur_time.tm_sec) && 
                    (this->cur_time.tm_min == t.cur_time.tm_min) && 
                    (this->cur_time.tm_hour == t.cur_time.tm_hour) && 
                    (this->cur_time.tm_mday == t.cur_time.tm_mday) && 
                    (this->cur_time.tm_wday == t.cur_time.tm_wday) && 
                    (this->cur_time.tm_yday == t.cur_time.tm_yday) && 
                    (this->cur_time.tm_mon == t.cur_time.tm_mon) && 
                    (this->cur_time.tm_year == t.cur_time.tm_year));
    }
    
    bool time_class::operator!=(const time_class& t) const
    {
        return ((this->cur_time.tm_sec != t.cur_time.tm_sec) && 
                    (this->cur_time.tm_min != t.cur_time.tm_min) && 
                    (this->cur_time.tm_hour != t.cur_time.tm_hour) && 
                    (this->cur_time.tm_mday != t.cur_time.tm_mday) && 
                    (this->cur_time.tm_wday != t.cur_time.tm_wday) && 
                    (this->cur_time.tm_yday != t.cur_time.tm_yday) && 
                    (this->cur_time.tm_mon != t.cur_time.tm_mon) && 
                    (this->cur_time.tm_year != t.cur_time.tm_year));
    }
    
    bool time_class::operator<(const time_class& t) const
    {
        if(this->cur_time.tm_year < t.cur_time.tm_year) return true;
        else if(this->cur_time.tm_year == t.cur_time.tm_year)
        {
            if(this->cur_time.tm_yday < t.cur_time.tm_yday) return true;
            else if(this->cur_time.tm_yday == t.cur_time.tm_yday)
            {
                if(this->cur_time.tm_hour < t.cur_time.tm_hour) return true;
                else if(this->cur_time.tm_hour == t.cur_time.tm_hour)
                {
                    if(this->cur_time.tm_min < t.cur_time.tm_min) return true;
                    else if(this->cur_time.tm_min == t.cur_time.tm_min)
                    {
                        if(this->cur_time.tm_sec < t.cur_time.tm_sec) return true;
                    }
                }
            }
        }
        return false;
    }
    
    bool time_class::operator<=(const time_class& t) const
    {
        return (this->operator<(t) || this->operator==(t));
    }
    
    bool time_class::operator>(const time_class& t) const
    {
        return (this->operator!=(t) && !(this->operator<(t)));
    }
    
    bool time_class::operator>=(const time_class& t) const
    {
        return !(this->operator<(t));
    }
    
    const time_class& time_class::operator++(int t __attribute__((unused)))
    {
        this->cur_time.tm_sec++;
        if(this->cur_time.tm_sec > 59)
        {
            this->cur_time.tm_sec %= 60;
            this->cur_time.tm_min++;
            if(this->cur_time.tm_min > 59)
            {
                this->cur_time.tm_min %= 60;
                this->cur_time.tm_hour++;
                if(this->cur_time.tm_hour > 23)
                {
                    this->cur_time.tm_hour %= 24;
                    this->cur_time.tm_yday++;
                    this->cur_time.tm_wday++;
                    this->cur_time.tm_wday %= 7;
                    if(this->cur_time.tm_yday > (__isleap(this->cur_time.tm_year) ? 364 : 365))
                    {
                        this->cur_time.tm_year++;
                        this->cur_time.tm_yday = 0;
                    }
                    this->sync_to_yday();
                }
            }
        }
        return *this;
    }
    
    time_class time_class::operator+(int i) const
    {
        time_class tempt(*this);
        if(i < 0) for(int x = 0; x < (i * (-1)); x++) tempt--;
        else if(i > 0) for(int x = 0; x < i; x++) tempt++;
        return tempt;
    }
    
    const time_class& time_class::operator+=(int i)
    {
        (*this) = ((*this) + i);
        return *this;
    }
    
    const time_class& time_class::operator--(int t __attribute__((unused)))
    {
        if(this->cur_time.tm_sec == 0)
        {
            this->cur_time.tm_sec = 59;
            if(this->cur_time.tm_min == 0)
            {
                this->cur_time.tm_min = 59;
                if(this->cur_time.tm_hour == 0)
                {
                    this->cur_time.tm_hour = 23;
                    if(this->cur_time.tm_yday == 0)
                    {
                        if(this->cur_time.tm_year > 0)
                        {
                            this->cur_time.tm_year--;
                        }
                        
                        if(this->cur_time.tm_yday == 0) this->cur_time.tm_yday = (__isleap(this->cur_time.tm_year) ? 364 : 365);
                        else if(this->cur_time.tm_yday > 0) this->cur_time.tm_yday--;
                        
                        if(this->cur_time.tm_wday == 0) this->cur_time.tm_wday = 6;
                        else if(this->cur_time.tm_wday > 0) this->cur_time.tm_wday--;
                    }
                    else if(this->cur_time.tm_yday > 0) this->cur_time.tm_yday--;
                }
                else if(this->cur_time.tm_hour > 0) this->cur_time.tm_hour--;
            }
            else if(this->cur_time.tm_min > 0) this->cur_time.tm_min--;
        }
        else if(this->cur_time.tm_sec > 0) this->cur_time.tm_sec--;
        this->sync_to_yday();
        return *this;
    }
    
    time_class time_class::operator-(int i) const
    {
        time_class tempt(*this);
        if(i < 0) for(int x = 0; x < (i * (-1)); x++) tempt++;
        else if(i > 0) for(int x = 0; x < i; x++) tempt--;
        return tempt;
    }
    
    const time_class& time_class::operator-=(int i)
    {
        (*this) = ((*this) - i);
        return *this;
    }
    
    int& time_class::second()
    {
        return this->cur_time.tm_sec;
    }
    
    int& time_class::minute()
    {
        return this->cur_time.tm_min;
    }
    
    /* Hour in military format */
    int& time_class::mhour()
    {
        return this->cur_time.tm_hour;
    }
    
    int& time_class::month()
    {
        return this->cur_time.tm_mon;
    }
    
    int& time_class::wday()
    {
        return this->cur_time.tm_wday;
    }
    
    /* Hour in 12-hour format */
    int time_class::hour() const
    {
        int tempi(this->cur_time.tm_hour % 12);
        if(tempi == 0) tempi = 12;
        return tempi;
    }
    
    bool time_class::am() const
    {
        return (this->cur_time.tm_hour < 12);
    }
    
    int time_class::gyear() const
    {
        return (this->cur_time.tm_year + 1900);
    }
    
    void time_class::syear(const int& i)
    {
        unsigned int tempyday(this->cur_time.tm_yday);
        int target(i - 1900);
        
        if(target < this->cur_time.tm_year)
        {
            while(target < this->cur_time.tm_year) (*this) = this->operator-(vday::value);
        }
        else if(target > this->cur_time.tm_year)
        {
            while(target > this->cur_time.tm_year) (*this) = this->operator+(vday::value);
        }
        this->cur_time.tm_yday = tempyday;
        this->sync_to_yday();
    }
    
    /* Syncs the month and month day to the day of the year.  This allows
     * us to simply do math with yday, and then "sync" the month and day of 
     * month to the yday. */
    void time_class::sync_to_yday()
    {
        this->cur_time.tm_mday = 1;
        for(this->cur_time.tm_mon = 0; 
                        ((this->cur_time.tm_mon < 12) && ((signed)total_days_in_m(this->cur_time.tm_mon, this->cur_time.tm_year) < this->cur_time.tm_yday)); 
                        this->cur_time.tm_mon++);
        if(this->cur_time.tm_yday < month_days(this->cur_time.tm_year).at(this->cur_time.tm_mon))
        {
            this->cur_time.tm_mday = (this->cur_time.tm_yday + 1);
        }
        else
        {
            this->cur_time.tm_mday = (this->cur_time.tm_yday - total_days_in_m((this->cur_time.tm_mon - 1), this->cur_time.tm_year));
        }
    }
    
    
}

/** misc functions: */
namespace tdata
{
    struct tm current_time()
    {
        time_point now(std::chrono::system_clock::now());
        struct tm temptm;
        tptotm(now, temptm);
        return temptm;
    }
    
    
}

