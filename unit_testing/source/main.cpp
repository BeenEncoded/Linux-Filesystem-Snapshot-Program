//#include <unittest++/UnitTest++.h>
#include <ctime>
#include <iostream>
#include <string>
#include <chrono>
#include <thread>

#include "time_class.hpp"
#include "common.hpp"
#include "global_defines.hpp"

#define TEST_ID_NUMBER_MAC 0

#if TEST_ID_NUMBER_MAC == 1
    #include "snapshot_data_stream_operator_test_case.hpp"
#elif TEST_ID_NUMBER_MAC == 2
    #include "retrieve_snapshot_information_test_case.hpp"
#elif TEST_ID_NUMBER_MAC == 3
    #include "filesystem_class_test_cases.hpp"
#elif TEST_ID_NUMBER_MAC == 4
    #include "time_class_unit_tests.hpp"
#endif

namespace
{
    void show_clock();
    void display_time(const tdata::time_class&);
    void sleep(const unsigned int&);
    void change_time_menu(tdata::time_class&);
    std::string selected_date_display(const tdata::time_class&, const int&);
    void add_to_selected(tdata::time_class&, const int&, const bool&);
    void add_months(tdata::time_class&, const int&);
    
    
    inline void display_time(const tdata::time_class& t)
    {
        using std::cout;
        using std::endl;
        
        cout<< t.wday_name()<< " "<< t.month_name()<< " "<< t.mday()<< ", "<< t.gyear()<< "  "<< 
                        t.hour()<< ":"<< t.minute()<< ":"<< t.second()<< " "<< 
                        (t.am() ? "am" : "pm")<< endl;
    }
    
    inline void sleep(const unsigned int& x)
    {
        using namespace std::chrono;
        std::this_thread::sleep_for(milliseconds(x));
    }
    
    /* Displays the selected date for the change_time_menu. */
    inline std::string selected_date_display(const tdata::time_class& t, const int& i)
    {
        std::string temps;
        temps += (t.wday_name() + " ");
        if(i == 2) temps += ("[" + t.month_name() + "]");
        else temps += t.month_name();
        
        if(i == 0) temps += (" [" + std::to_string(t.mday()) + "]");
        else temps += (" " + std::to_string(t.mday()));
        
        if(i == 1) temps += (", [" + std::to_string(t.gyear()) + "]");
        else temps += (", " + std::to_string(t.gyear()));
        return temps;
    }
    
    inline void add_months(tdata::time_class& t, const int& i)
    {
        using namespace tdata::t_const;
        using tdata::days_in_month;
        
        int tempi(i);
        
        auto add_month = [&t]()->void
        {
            using namespace tdata::t_const;
            using tdata::days_in_month;
            
            int mday(t.mday());
            
            if(mday > days_in_month(((t.month() + 1) % 12), t.gyear()))
            {
                mday %= days_in_month(((t.month() + 1) % 12), t.gyear());
            }
            while(t.mday() != 1) t += day::value;
            while(t.mday() != mday) t += day::value;
        };
        
        auto subtract_month = [&t]()->void
        {
            using namespace tdata::t_const;
            using tdata::days_in_month;
            
            int mday(t.mday());
            
            if(mday > days_in_month(((t.month() + 11) % 12), t.gyear()))
            {
                mday %= days_in_month(((t.month() + 11) % 12), t.gyear());
            }
            while(t.mday() != 1) t -= day::value;
            t -= day::value;
            while(t.mday() != mday) t -= day::value;
        };
        
        if(tempi < 0) tempi *= (-1);
        for(int x = 0; x < tempi; x++) ((i < 0) ? subtract_month() : add_month());
    }
    
    inline void add_to_selected(tdata::time_class& t, const int& selected, const bool& add)
    {
        using namespace tdata::t_const;
        switch(selected)
        {
            case 0:
            {
                if(add) t += day::value;
                else t -= day::value;
            }
            break;
            
            case 1: 
            {
                if(add) t.syear(t.gyear() + 1);
                else t.syear(t.gyear() - 1);
            }
            break;
            
            case 2: 
            {
                if(add) add_months(t, 1);
                else add_months(t, -1);
            }
            break;
            
            default:
            {
            }
            break;
        }
    }
    
    inline void change_time_menu(tdata::time_class& t)
    {
        using tdata::time_class;
        using std::cout;
        using std::endl;
        using key_code::key_code_data;
        
        bool finished(false);
        int selected(0);
        key_code_data key;
        
        /* 
         * Selected: 
         * 0 = day
         * 1 = year
         * 2 = month */
        
        do
        {
            common::cls();
            for(unsigned int x = 0; x < v_center::value; x++) cout<< endl;
            common::center(selected_date_display(t, selected));
            
            key = common::gkey_funct();
            switch(key_code::is_listed_control(key))
            {
                case true:
                {
                    if(key.is_control)
                    {
                        using namespace key_code::code;
                        using key_code::keys;
                        
                        if(key == keys[up::value])
                        {
                            add_to_selected(t, selected, true);
                        }
                        else if(key == keys[down::value])
                        {
                            add_to_selected(t, selected, false);
                        }
                        else if(key == keys[right::value])
                        {
                            selected = ((selected + 1) % 3);
                        }
                        else if(key == keys[left::value])
                        {
                            selected = ((selected + 2) % 3);
                        }
                        else if(key == keys[backspace::value])
                        {
                            finished = true;
                        }
                    }
                }
                break;
                default:
                {
                }
                break;
            }
        }while(!finished);
    }
    
    inline void show_clock()
    {
        using std::cout;
        using std::endl;
        using namespace tdata::t_const;
        
        tdata::time_class tempt;
        
        common::cl();
        while(!common::kbhit())
        {
            tempt = tdata::current_time();
            common::cls();
            display_time(tempt + day::value);
            cout<< endl;
            cout<< "Today is: ";
            display_time(tempt);
            cout<< endl;
            display_time(tempt - (1 * day::value));
            cout<< endl;
            display_time(tempt - (2 * day::value));
            cout<< endl;
            display_time(tempt - (3 * day::value));
            sleep(1000 / 30);
        }
    }
    
    
}

int main(__attribute__((unused)) int c, __attribute__((unused)) char **v)
{
    srand(std::time(NULL));
    tdata::time_class t(tdata::current_time());
    t.syear(2);
    change_time_menu(t);
    return 0;
}