#include <vector>
#include <string>
#include <iostream>

#include "common_menu.hpp"
#include "common.hpp"

using std::cout;
using std::endl;

namespace
{
    void display_element(const std::string&, const bool&);
    
    
    inline void display_element(const std::string& s, const bool& selected)
    {
        if(selected) cout<< "[";
        else cout<< " ";
        
        cout<< s;
        
        if(selected) cout<< "]";
    }
    
    
}

namespace common_menu
{
    void display_scroll_window(scrollDisplay::scroll_display_class& win, const unsigned int& total_size)
    {
        unsigned int from_beg(win.window_beg()), from_end((total_size - (win.window_beg() + win.window_size())));
        std::vector<std::string> disp(win.window());
        
        if(from_beg > 0) common::center("^^ " + std::to_string(from_beg) + " ^^");
        for(unsigned int x = 0; x < 2; x++) cout<< endl;
        cout<< std::string(70, '^')<< endl;
        
        for(short x = 0; x < (short)disp.size(); x++)
        {
            display_element(disp[x], (x == win.gpos().part));
            cout<< endl;
        }
        for(unsigned int x = 0; x < ((unsigned int)win.window_size() - win.window().size()); x++)
        {
            cout<< endl;
        }
        cout<< std::string(70, 'V')<< endl;
        if(from_end > 0) common::center("V " + std::to_string(from_end) + " V");
        cout<< endl;
    }
    
    
}