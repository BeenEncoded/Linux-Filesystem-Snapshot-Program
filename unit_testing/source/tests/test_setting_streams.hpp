#ifndef TEST_SETTING_STREAMS_HPP_INCLUDED
#define TEST_SETTING_STREAMS_HPP_INCLUDED
#include <unittest++/UnitTest++.h>
#include <sstream>
#include <iostream>

#include "program_settings.hpp"
#include "random_junk_data.hpp"
#include "global_defines.hpp"

namespace
{
    typedef const_int_type<200> test_rep;
    
    template<class type> bool test_stream_ops(const type&);
    template<class type> std::vector<type> random_vector(type (*)(), const unsigned int&);
    
    settings::regex_data random_regex_data();
    settings::regex_settings_data random_regex_settings();
    settings::settings_data random_settings_data();
    
    
    
    
    template<class type>
    inline bool test_stream_ops(const type& t)
    {
        std::stringstream ss;
        type new_value;
        
        ss<< t;
        ss>> new_value;
        return (t == new_value);
    }
    
    inline settings::regex_data random_regex_data()
    {
        settings::regex_data data;
        data.on = rand_data::random_bool();
        data.s = rand_data::random_str(10, 50);
        return data;
    }
    
    template<class type>
    inline std::vector<type> random_vector(type (*rand_data)(), const unsigned int& max_size)
    {
        unsigned int size(rand_data::random_number((max_size - ((max_size > 49) ? 49 : 0)), max_size));
        std::vector<type> newvec;
        
        for(unsigned int x = 0; x < size; ++x) newvec.push_back(rand_data());
        return newvec;
    }
    
    inline settings::regex_settings_data random_regex_settings()
    {
        using namespace rand_data;
        
        settings::regex_settings_data settings_data;
        settings_data.use_match = random_bool();
        settings_data.use_not_match = random_bool();
        settings_data.use_regex = random_bool();
        settings_data.match = random_vector<settings::regex_data>(random_regex_data, 100);
        settings_data.not_match = random_vector<settings::regex_data>(random_regex_data, 100);
        return settings_data;
    }
    
    inline settings::settings_data random_settings_data()
    {
        settings::settings_data data;
        data.editor = rand_data::random_str(0, 50);
        data.regex_settings = random_regex_settings();
        return data;
    }
    
    
}

SUITE(setting_streams_test_suite)
{
    TEST(test_regex_data_stream_operators_test_case)
    {
        for(unsigned int x = 0; x < test_rep::value; ++x)
        {
            CHECK(test_stream_ops<settings::regex_data>(random_regex_data()));
        }
    }
    
    TEST(test_regex_settings_data_test_case)
    {
        for(unsigned int x = 0; x < test_rep::value; ++x)
        {
            CHECK(test_stream_ops<settings::regex_settings_data>(random_regex_settings()));
        }
    }
    
    TEST(test_settings_data_test_case)
    {
        for(unsigned int x = 0; x < test_rep::value; ++x)
        {
            CHECK(test_stream_ops<settings::settings_data>(random_settings_data()));
        }
    }
    
    
}

#endif