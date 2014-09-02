#include <vector>
#include <string>
#include <iostream>
#include <unordered_set>
#include <functional>

#include "common_menu.hpp"
#include "common.hpp"
#include "snapshot_class.hpp"

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
        unsigned int from_end((size - (win.window_beg() + win.window_size())));
        for(unsigned int x = 0; x < ((unsigned int)win.window_size() - win.window().size()); x++)
        {
            cout<< endl;
        }
        cout<< std::string(70, 'V')<< endl;
        if(from_end > 0) common::center("V " + std::to_string(from_end) + " V");
        cout<< endl;
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

/* Display selection class */
namespace common_menu
{
    /* Stores and retrieves a set of "selected" data.  This
     * is meant to be used for display purposes.  Given a compare function, 
     * and a group of elements, a */
    template<typename type>
    class selection_class
    {
    public:
        explicit selection_class() : data(nullptr), selection(), compare() {}
        explicit selection_class(const std::vector<type>& v, const std::function<bool(const type&, const type&)>& f) : 
                data(&v), 
                selection(), 
                compare(f) {}
        
        ~selection_class(){}
        
        const selection_class& operator=(const selection_class& s)
        {
            if(this != &s)
            {
                this->data = s.data;
                this->selection = s.selection;
            }
            return *this;
        }
        
        void clear()
        {
            this->data = nullptr;
            this->selection.clear();
        }
        
        bool add(const type& t)
        {
            unsigned int ind(this->index_of(t));
            if(this->selection.find(ind) == this->selection.end())
            {
                return this->selection.insert(ind).second;
            }
            return false;
        }
        
        bool add(const unsigned int& x)
        {
            if(this->selection.find(x) == this->selection.end())
            {
                return this->selection.insert(x).second;
            }
            return false;
        }
        
        bool remove(const unsigned int& x)
        {
            if(this->selection.find(x) != this->selection.end())
            {
                this->selection.erase(this->selection.find(x));
                return true;
            }
            return false;
        }
        
        bool remove(const type& t)
        {
            unsigned int ind(0);
            if(collisions(t) > 1)
            {
                throw "ERROR: bool ::common_menu::selection_class<typename>::remove(const type&): \
can not remove: compare function generates too many collisions for this action to be reliable!";
            }
            else
            {
                if(this->contains(t))
                {
                    ind = this->index_of(t);
                    if(this->selection.find(ind) != this->selection.end())
                    {
                        return this->selection.erase(ind);
                    }
                }
            }
            return false;
        }
        
        bool is_selected(const unsigned int& x) const
        {
            return (this->selection.find(x) != this->selection.end());
        }
        
        bool is_selected(const type& t) const
        {
            if(this->collisions(t) > 1)
            {
                throw "ERROR: bool ::common_menu::selection_class<typename>::is_selected(const type&): \
can not compute: compare function generates too many collisions for this action to be reliable!";
            }
            if(this->contains(t))
            {
                return (this->selection.find(this->index_of(t)) != this->selection.end());
            }
        }
        
        /* since this object can refer to anything, we must be able to check
         * if the compare function provided creates collisions.  This operation
         * is cpu-intensive (0(n!)), so it should only be used when absolutely necessary.
         * 
         * Do NOT abuse this function, or rely on it for reliability checking. */
        bool compare_funct_reliable()
        {
            unsigned int count;
            for(std::vector<std::string>::const_iterator it = this->data->begin(); 
                    it != this->data->end(); ++it)
            {
                for(std::vector<std::string>::const_iterator subit = (it + 1); 
                        ((subit != this->data->end()) && (count < 2)); ++subit)
                {
                    if(this->compare(*it, *subit)) count++;
                }
                if(count == 1) count = 0;
                else return false;
            }
            return true;
        }
        
    private:
        unsigned int index_of(const type& t) const
        {
            for(unsigned int x = 0; x < this->data->size(); x++)
            {
                if(this->compare((*(this->data))[x], t)) return x;
            }
            return 0;
        }
        
        /* returns the number of collisions the compare functions generates. */
        unsigned int collisions(const type& t) const
        {
            unsigned int count(0);
            for(typename std::vector<type>::const_iterator it = this->data->begin(); 
                    it != this->data->end(); ++it)
            {
                if(this->compare(t, *it)) count++;
            }
            return count;
        }
        
        bool contains(const type& t) const
        {
            for(typename std::vector<type>::const_iterator it = this->data->begin(); it != this->data->end(); 
                    ++it)
            {
                if(compare(t, *it)) return true;
            }
            return false;
        }
        
        std::vector<type> *data;
        std::unordered_set<unsigned int> selection;
        std::function<bool(const type&, const type&)> compare;
        
    };
    
}