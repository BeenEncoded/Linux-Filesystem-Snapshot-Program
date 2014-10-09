#include <vector>
#include <string>
#include <iostream>
#include <unordered_set>
#include <functional>
#include <fstream>

#include "common_menu.hpp"
#include "common.hpp"
#include "snapshot_class.hpp"
#include "snapshot_file_loaders.hpp"
#include "filesystem.hpp"

using std::cout;
using std::endl;

namespace
{
    void display_element(const std::string&, const bool&);
    void display_top_border(scrollDisplay::scroll_display_class&);
    void display_bottom_border(scrollDisplay::scroll_display_class&, const unsigned int&);
    void display_multiselect_element(const std::string&, const bool&, const bool&);
    
    
    inline void display_element(const std::string& s, const bool& selected)
    {
        if(selected) cout<< "[";
        else cout<< " ";
        
        cout<< s;
        
        if(selected) cout<< "]";
    }
    
    inline void display_multiselect_element(const std::string& s, const bool& selected, const bool& multiselected)
    {
        if(multiselected) cout<< ">";
        else cout<< " ";
        
        if(selected) cout<< "[";
        else cout<< " ";
        
        cout<< s;
        
        if(selected) cout<< "]";
        else cout<< " ";
        
        if(multiselected) cout<< "<";
    }
    
    inline void display_top_border(scrollDisplay::scroll_display_class& win)
    {
        using std::cout;
        using std::endl;
        
        unsigned int from_beg(win.window_beg());
        if(from_beg > 0) common::center("^^ " + std::to_string(from_beg) + " ^^");
        cout<< endl;
        cout<< std::string(70, '^')<< endl;
    }
    
    inline void display_bottom_border(scrollDisplay::scroll_display_class& win, const unsigned int& size)
    {
        unsigned int from_end((size - (win.window_beg() + win.window().size())));
        for(unsigned int x = 0; x < ((unsigned int)win.window_size() - win.window().size()); x++)
        {
            cout<< endl;
        }
        cout<< std::string(70, 'V')<< endl;
        if(from_end > 0) common::center("V " + std::to_string(from_end) + " V");
        cout<< endl;
    }
    
    
}

/* Snapshot_menu-specific functions */
namespace common_menu
{
    namespace snapshot
    {
        bool save_snapshot(const ::snapshot::snapshot_data& snap, const std::string& f)
        {
            using std::ofstream;
            using fsys::is_folder;
            using fsys::is_symlink;
            using fsys::create_folder;
            using fsys::result_data_boolean;
            using ::snapshot::snapshot_folder;
            using fsys::pref_slash;
            
            ofstream out;
            result_data_boolean tempres;
            std::string folder(snapshot_folder()), file(f + fsys::pref_slash() + 
                            std::to_string(snap.id) + 
                            fsyssnap_SNAPSHOT_FILE_EXTENSION);
            bool success(false);
            
            if(snap.id != 0)
            {
                if(!is_folder(folder).value)
                {
                    tempres = create_folder(folder);
                    if(!tempres.value)
                    {
                        cout<< "bool save_snapshot(const snapshot::snapshot_class&)\
    : error; \""<< tempres.error<< "\""<< endl;
                    }
                }
                if(is_folder(folder).value && !is_symlink(folder).value)
                {
                    out.open(file.c_str(), std::ios::binary);
                    out<< snap;
                    out.close();
                    success = true;
                }
            }
            else
            {
                std::cerr<< "Snapshot not saved: invalid identification (unsigned \
    long long)"<< std::endl;
            }
            return success;
        }
        
        
    }
}

namespace common_menu
{
    void display_scroll_window(scrollDisplay::scroll_display_class& win, const unsigned int& total_size)
    {
        std::vector<std::string> disp(win.window());
        
        display_top_border(win);
        for(short x = 0; x < (short)disp.size(); x++)
        {
            display_element(disp[x], (x == win.gpos().part));
            cout<< endl;
        }
        display_bottom_border(win, total_size);
    }
    
    
}

/* selection class */
namespace common_menu
{
    selection_class::selection_class() : selection() {}
    
    selection_class::~selection_class()
    {
        this->selection.clear();
    }
    
    const selection_class& selection_class::operator=(const selection_class& s)
    {
        if(this != &s)
        {
            this->selection = s.selection;
        }
        return *this;
    }
    
    bool selection_class::is_selected(const unsigned int& x) const
    {
        return (this->selection.find(x) != this->selection.end());
    }
    
    /* returns true if the element was added to the selection.*/
    bool selection_class::add(const unsigned int& x)
    {
        return this->selection.insert(x).second;
    }
    
    bool selection_class::remove(const unsigned int& x)
    {
        return (this->selection.erase(x) > 0);
    }
    
    /* returns the number of selected elements.*/
    unsigned int selection_class::count() const
    {
        return this->selection.size();
    }
    
    
}