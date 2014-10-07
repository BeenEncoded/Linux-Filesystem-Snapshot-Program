#ifndef TIME_CLASS_UNIT_TESTS_HPP_INCLUDED
#deifne TIME_CLASS_UNIT_TESTS_HPP_INCLUDED
#include <unittest/UnitTest++.h>
#include <cstring>
#include <ctime>

#include "time_class.hpp"
#include "random_junk_data.hpp"

using std::cout;
using std::endl;

namespace
{
    struct tm random_time();
    
    
    
    inline struct tm random_time()
    {
        using namespace rand_data;
        
        struct tm tempt(tdata::current_time());
        tempt.tm_sec = random_number(0, 59);
        tempt.tm_min = random_number(0, 59);
        tempt.tm_hour = random_number(0, 23);
        tempt.tm_mday = random_number(1, 31);
        tempt.tm_yday = random_number(0, 365);
        tempt.tm_wday = random_number(0, 6);
        tempt.tm_year = random_number(0, 2100);
        return tempt;
    }
    
    
}

TEST(time_class_basic_operators_test_case)
{
    using tdata::time_class;
    
    time_class bef, aft;
    aft = random_time();
    bef = tdata::current_time();
    for(unsigned int x = 0; x < 100; x++) bef++;
    
}

#endif