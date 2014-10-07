#ifndef MENU_GLOBALS_HPP_INCLUDED
#define MENU_GLOBALS_HPP_INCLUDED
#include <unordered_set>


#include "scroll_display.hpp"
#include "global_defines.hpp"
#include "snapshot_class.hpp"

/** Stuff common to all menus, such as return data, that have no place 
 * in common. */

namespace common_menu
{
    typedef const_int_type<10> wsize;
    
    struct menu_return_data;
    class selection_class;
    
    enum scroll_display_type
    {
        def = 0,
        multi_select = 1
    };
    
    void display_scroll_window(scrollDisplay::scroll_display_class&, const unsigned int&);
    
    namespace snapshot
    {
        bool save_snapshot(const ::snapshot::snapshot_data&);
        
    }
    
    struct menu_return_data
    {
        bool canceled = false, modified = false;
    };
    
    class selection_class
    {
    public:
        explicit selection_class();
        ~selection_class();
        
        const selection_class& operator=(const selection_class&);
        
        bool add(const unsigned int&);
        bool is_selected(const unsigned int&) const;
        bool remove(const unsigned int&);
        unsigned int count() const;
        
    private:
        std::unordered_set<unsigned int> selection;
        
    };
    
    
}

#endif