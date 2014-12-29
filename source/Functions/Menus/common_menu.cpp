#include <vector>
#include <string>
#include <iostream>
#include <unordered_set>
#include <functional>
#include <fstream>

#include "common_menu.hpp"
#include "common.hpp"
#include "snapshot_class.hpp"
#include "snapshot_file_loader.hpp"
#include "filesystem.hpp"

using std::cout;
using std::endl;

namespace
{
    void display_element(const std::string&, const bool&, const bool&);
    void display_top_border(scrollDisplay::scroll_display_class&);
    void display_bottom_border(scrollDisplay::scroll_display_class&, const unsigned int&);
    std::string filename(const std::string&);
    std::string extension(const std::string&);
    bool folder_contains_ext(const std::string&, const std::string&);
    
    
    inline void display_element(const std::string& s, const bool& selected, 
                    const bool& special_selected)
    {
        cout<< (special_selected ? ">" : " ");
        
        if(selected) cout<< "[";
        else cout<< " ";
        
        cout<< s;
        
        if(selected) cout<< "]";
        if(special_selected) cout<< "<";
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
    
    inline std::string filename(const std::string& f) //test
    {
        std::size_t pos(f.rfind(fsys::pref_slash()));
        std::string temps(f);
        if(pos != std::string::npos)
        {
            temps.erase(temps.begin(), (temps.begin() + pos + 1));
        }
        return temps;
    }
    
    inline std::string extension(const std::string& file) //test
    {
        std::string temps(filename(file));
        std::size_t pos(temps.rfind('.'));
        
        if(pos != std::string::npos)
        {
            temps.erase(temps.begin(), (temps.begin() + pos));
        }
        return temps;
    }
    
    inline bool folder_contains_ext(const std::string& folder, const std::string& ext) //test
    {
        using fsys::tree_iterator_class;
        using fsys::is_folder;
        using fsys::is_file;
        using fsys::is_symlink;
        
        if(!is_folder(folder).value || is_symlink(folder).value || is_file(folder).value) return false;
        for(tree_iterator_class it(folder); !it.at_end(); ++it)
        {
            if(is_file(it.value()).value && !is_symlink(it.value()).value)
            {
                if(extension(it.value()) == ext) return true;
            }
        }
        return false;
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
                std::cout<< "Snapshot not saved: invalid identification (unsigned \
    long long)"<< std::endl;
            }
            return success;
        }
        
        
    }
}

namespace common_menu
{
    void display_scroll_window(scrollDisplay::scroll_display_class& win, 
                    const unsigned int& total_size)
    {
        std::vector<std::string> disp(win.window());
        
        display_top_border(win);
        for(short x = 0; x < (short)disp.size(); x++)
        {
            display_element(disp[x], (x == win.gpos().part), false);
            cout<< endl;
        }
        display_bottom_border(win, total_size);
    }
    
    void display_scroll_window(scrollDisplay::scroll_display_class& win, 
                    const unsigned int& total_size,
                    const selection_class& selection)
    {
        std::vector<std::string> disp(win.window());
        
        display_top_border(win);
        for(short x = 0; x < (short)disp.size(); x++)
        {
            display_element(disp[x], (x == win.gpos().part), selection.is_selected(win.window_beg() + x));
            cout<< endl;
        }
        display_bottom_border(win, total_size);
    }
    
    
    /** Used to define how the program tells whether records exist. */
    bool records_exist(const std::string& rfolder)
    {
        return folder_contains_ext(rfolder, ".txt");
    }
    
    /** Used to define how the program tells whether snapshots exist. */
    bool snapshots_exist(const std::string& folder)
    {
        return folder_contains_ext(folder, fsyssnap_SNAPSHOT_FILE_EXTENSION);
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
    
    const unsigned int& selection_class::operator[](int x) const
    {
        int tempi(0);
        if(!this->selection.empty())
        {
            for(std::unordered_set<unsigned int>::const_iterator it = this->selection.begin(); 
                            it != this->selection.end(); 
                            ++it)
            {
                if(tempi == x) return *it;
                tempi++;
            }
        }
        throw std::out_of_range("line: " + std::to_string(__LINE__) + ": const unsigned int& \
selection_class::operator[](int x) const: Error: Must be within bounds of the selection!");
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
    
    const std::unordered_set<unsigned int>& selection_class::gselection() const
    {
        return this->selection;
    }
    
    void selection_class::clear()
    {
        this->selection.clear();
    }
    
    
}