#ifndef MENU_GLOBALS_HPP_INCLUDED
#define MENU_GLOBALS_HPP_INCLUDED


#include "scroll_display.hpp"
#include "global_defines.hpp"
#include "snapshot_class.hpp"

/** Stuff common to all menus, such as return data, that have no place 
 * in common. */

namespace common_menu
{
    typedef const_int_type<10> wsize;
    
    struct menu_return_data;
    
    struct menu_return_data
    {
        bool canceled = false, modified = false;
    };
    
    enum scroll_display_type
    {
        def = 0,
        multi_select = 1
    };
    
    void display_scroll_window(scrollDisplay::scroll_display_class&, const unsigned int&);
    
}

#endif