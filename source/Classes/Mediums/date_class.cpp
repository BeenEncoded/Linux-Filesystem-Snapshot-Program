#include <ctime>
#include <string>
#include <sstream>
#include <vector>
#include <exception>
#include <iostream>

#include "date_class.hpp"
#include "global_defines.hpp"
#include "common.hpp"

#include <assert.h>

namespace
{
    std::vector<std::string> day_names();
    std::vector<std::string> month_names();
    std::vector<unsigned short> month_days(const unsigned int&);
    
    
    
    template<class type1, class type2>
    inline type2 conv(const type1& t1)
    {
        std::stringstream ss;
        type2 t2;
        ss<< t1;
        ss>> t2;
        return t2;
    }
    
    inline std::vector<std::string> day_names()
    {
        return std::vector<std::string>({
            "Sunday",
                    "Monday",
                    "Tuesday",
                    "Wednesday",
                    "Thursday",
                    "Friday",
                    "Saturday"});
    }
    
    inline std::vector<std::string> month_names()
    {
        return std::vector<std::string>({
            "January",
                    "February",
                    "March",
                    "April",
                    "May",
                    "June",
                    "July",
                    "August",
                    "September",
                    "October",
                    "November",
                    "December"});
    }
    
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
    
    
}

/** Functions belonging to the date_val data structure:  */
namespace date
{
    std::ostream& operator<<(std::ostream& out, const date_val& d)
    {
        if(out.good())
        {
            out<< d.year<< mem_delim::value;
            out<< d.yday<< mem_delim::value;
            out<< d.weekday<< mem_delim::value;
            out<< d.month<< mem_delim::value;
            out<< d.mday<< mem_delim::value;
        }
        return out;
    }
    
    std::istream& operator>>(std::istream& in, date_val& d)
    {
        using common::safe_getline;
        
        d = date_val();
        if(safe_getline(in, d.year, mem_delim::value))
            if(safe_getline(in, d.yday, mem_delim::value))
                if(safe_getline(in, d.weekday, mem_delim::value))
                    if(safe_getline(in, d.month, mem_delim::value))
                        safe_getline(in, d.mday, mem_delim::value);
        
        return in;
    }
    
    date_val& date_val::operator=(const date_val& i)
    {
        if(this != &i)
        {
            this->mday = i.mday;
            this->yday = i.yday;
            this->month = i.month;
            this->weekday = i.weekday;
            this->year = i.year;
        }
        return *this;
    }

    date_val& date_val::operator=(const struct tm& i)
    {
        this->year = (1900 + i.tm_year);
        this->month = i.tm_mon;
        this->weekday = i.tm_wday;
        this->mday = (i.tm_mday - 1);
        this->yday = i.tm_yday;
        return *this;
    }

    bool date_val::operator==(const date_val& i) const
    {
        return ((this->yday == i.yday) && (this->year == i.year));
    }

    bool date_val::operator!=(const date_val& i) const
    {
        return !(this->operator==(i));
    }
    
    bool date_val::operator<(const date_val& d) const
    {
        bool lessthan(false);
        switch(this->year < d.year)
        {
            case true:
            {
                lessthan = true;
            }
            break;
            
            case false:
            {
                lessthan = (this->yday < d.yday);
            }
            break;
            
            default:
            {
            }
            break;
        }
        return lessthan;
    }
    
    bool date_val::operator<=(const date_val& d) const
    {
        return (this->operator<(d) || this->operator==(d));
    }

    /** Converts this time into a tm structure (using a pre-existing one, 
     for flexibility). */
    void date_val::to_tm(struct tm& i) const
    {
        i.tm_mon = this->month;
        i.tm_mday = (this->mday + 1);
        assert(this->year > 1900);
        i.tm_year = (this->year - 1900);
        i.tm_yday = this->yday;
        i.tm_wday = this->weekday;
    }

    /** All operators add/subract a specified number of days
     to or from the date. */
    date_val date_val::operator++(__attribute__((unused)) int i)
    {
        *this = (*this + 1);
        return *this;
    }

    date_val date_val::operator--(__attribute__((unused)) int i)
    {
        *this = (*this - 1);
        return *this;
    }

    date_val date_val::operator+(const unsigned int& i) const
    {
        date_val tempdate(*this);

        for(unsigned int x = 0; x < i; x++)
        {
            tempdate.yday++;
            tempdate.weekday++;

            tempdate.weekday %= 7;

            //we increment the year-day, and the year, if necessary.
            if(tempdate.yday >= (__isleap(tempdate.year) ? 366 : 365))
            {
                tempdate.year++;
                tempdate.yday = 0;
            }

            //then, we calculate the month and day of month based on the year-day.
            tempdate.mday = 0;
            tempdate.month = 0;

            for(unsigned int x = 0; x < tempdate.yday; x++)
            {
                tempdate.mday++;
                if(tempdate.mday >= (unsigned short)month_days(tempdate.year).at(tempdate.month))
                {
                    tempdate.mday = 0;
                    tempdate.month++;
                }
            }
        }

        return tempdate;
    }

    date_val date_val::operator-(const unsigned int& i) const
    {
        date_val tempdate(*this);

        for(unsigned int x = 0; x < i; x++)
        {
            //otherwise....  well, you know...
            assert(tempdate.year > 0);

            /** Because we are working with unsigned intigers, we can't 
             subract from 0...  We will have to use conditionals.*/

            switch(tempdate.weekday == 0)
            {
                case true:
                {
                    tempdate.weekday = 6;
                }
                break;

                case false:
                {
                    tempdate.weekday--;
                }
                break;

                default:
                {
                }
                break;
            }

            switch(tempdate.yday == 0)
            {
                case true:
                {
                    tempdate.year--;
                    tempdate.yday = ((__isleap(tempdate.year) ? 366 : 365) - 1);
                }
                break;

                case false:
                {
                    tempdate.yday--;
                }
                break;

                default:
                {
                }
                break;
            }

            tempdate.mday = 0;
            tempdate.month = 0;
            for(unsigned int x = 0; x < tempdate.yday; x++)
            {
                tempdate.mday++;
                if(tempdate.mday >= (unsigned short)month_days(tempdate.year).at(tempdate.month))
                {
                    tempdate.mday = 0;
                    tempdate.month++;
                }
            }
        }

        return tempdate;
    }

    date_val date_val::operator+=(const unsigned int& i)
    {
        *this = (*this + i);
        return *this;
    }

    date_val date_val::operator-=(const unsigned int& i)
    {
        *this = (*this - i);
        return *this;
    }
    
    std::string date_val::month_name() const
    {
        return month_names().at(this->month);
    }
    
    std::string date_val::wday_name() const
    {
        return day_names().at(this->weekday);
    }
    
    
}

/** Other functions: */
namespace date
{
    std::string display(const date_val& d)
    {
        std::string disp;
        disp += month_names().at(d.month);
        disp += (" " + std::to_string((d.mday + 1)) + ", ");
        disp += std::to_string(d.year);
        return disp;
    }
    
}

