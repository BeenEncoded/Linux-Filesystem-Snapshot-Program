#include <string>
#include <vector>
#include <regex>
#include <iostream>
#include <cstring>
#include <utility>
#include <fstream>

#include "program_settings.hpp"
#include "global_defines.hpp"
#include "filesystem.hpp"
#include "common.hpp"
#include "snapshot_file_loader.hpp"

namespace
{
    std::vector<settings::regex_data> apply_filter(const std::vector<settings::regex_data>&);
    unsigned int available(std::istream&);
    
    
    //converts type into memory
    template<typename type>
    char* get_mem(type& t)
    {
        char* ch = new char[sizeof(type)];
        std::memcpy(ch, &t, sizeof(type));
        return ch;
    }
    
    template<char*>
    char* get_mem(char*& t)
    {
        ethrow("[PROGRAMMING ERROR]: Can not convert char* to char* through memory!");
        return NULL;
    }
    
    //converts memory into type
    template<typename type>
    type from_mem(const char* ch)
    {
        type t;
        std::memcpy(&t, ch, sizeof(type));
        return t;
    }
    
    template<char*>
    char* from_mem(const char* ch)
    {
        ethrow("[PROGRAMMING ERROR]: Can not convert char* to char* through memory!");
        return NULL;
    }
    
    template<typename type>
    void out_memory_of(std::ostream& out, type& t)
    {
        char *ch(get_mem<type>(t));
        for(unsigned int x = 0; x < sizeof(type); ++x) out<< ch[x];
        delete[] ch;
    }
    
    template<typename type>
    void in_memory_of(std::istream& in, type& t)
    {
        char *ch(new char[sizeof(type)]);
        in.peek();
        for(unsigned int x = 0; ((x < sizeof(type)) && in.good()); ++x)
        {
            ch[x] = in.get();
            if(ch[x] == EOF) ch[x] = '\0';
        }
        t = from_mem<type>(ch);
        delete[] ch;
    }
    
    inline std::vector<settings::regex_data> apply_filter(const std::vector<settings::regex_data>& list)
    {
        std::vector<settings::regex_data> enabled;
        for(std::vector<settings::regex_data>::const_iterator it = list.begin(); 
                it != list.end(); ++it)
        {
            if(it->on) enabled.push_back(*it);
        }
        return enabled;
    }
    
    inline unsigned int available(std::istream& in)
    {
        std::ios::pos_type pos(in.tellg());
        unsigned int count(in.seekg(0, std::ios::end).tellg() - pos);
        in.seekg(pos);
        return count;
    }
    
    
}

/** regex_data member functions: */
namespace settings
{
    regex_data::regex_data() noexcept : 
                    on(true),
                    s()
    {
    }
    
    regex_data& regex_data::operator=(const regex_data& reg) noexcept
    {
        if(this != &reg)
        {
            this->on = reg.on;
            this->s = reg.s;
        }
        return *this;
    }
    
    bool regex_data::operator==(const regex_data& r) const noexcept
    {
        return (
                    (this->s == r.s) && 
                    (this->on == r.on));
    }
    
    bool regex_data::operator!=(const regex_data& r) const noexcept
    {
        return !(this->operator==(r));
    }
    
    std::ostream& operator<<(std::ostream& out, const regex_data& r)
    {
        //test
        out<< r.s<< mem_delim::value;
        bool tempb(r.on);
        out_memory_of(out, tempb);
        return out;
    }
    
    std::istream& operator>>(std::istream& in, regex_data& r)
    {
        //test
        r = regex_data();
        
        in.peek();
        if(in.good())
        {
            bool tempb;
            if(common::safe_getline(in, r.s, mem_delim::value))
            {
                in_memory_of(in, tempb);
                r.on = tempb;
            }
            else
            {
                if(in.eof()) std::cout<< "end of file!"<< std::endl;
                ethrow("read error!");
            }
        }
        in.peek();
        return in;
    }
    
    
}

/** regex_settings_data member functions: */
namespace settings
{
    regex_settings_data::regex_settings_data() noexcept : 
                    use_regex(false),
                    use_match(true),
                    use_not_match(true),
                    match(),
                    not_match()
    {
    }
    
    regex_settings_data& regex_settings_data::operator=(const regex_settings_data& r) noexcept
    {
        if(this != &r)
        {
            this->use_regex = r.use_regex;
            this->use_match = r.use_match;
            this->use_not_match = r.use_not_match;
            this->match = r.match;
            this->not_match = r.not_match;
        }
        return *this;
    }
    
    bool regex_settings_data::operator==(const regex_settings_data& r) const noexcept
    {
        return ((this->use_regex == r.use_regex) && 
                (this->use_match == r.use_match) && 
                (this->use_not_match == r.use_not_match) && 
                (this->match == r.match) && 
                (this->not_match == r.not_match));
    }
    
    bool regex_settings_data::operator!=(const regex_settings_data& r) const noexcept
    {
        return !(this->operator==(r));
    }
    
    std::ostream& operator<<(std::ostream& out, const regex_settings_data& r)
    {
        //test
        bool tempb(r.use_regex);
        out_memory_of<bool>(out, tempb);
        tempb = r.use_match;
        out_memory_of<bool>(out, tempb);
        tempb = r.use_not_match;
        out_memory_of<bool>(out, tempb);
        for(std::vector<regex_data>::const_iterator it = r.match.begin(); it != r.match.end(); ++it)
        {
            out<< *it;
        }
        out<< struct_delim::value;
        for(std::vector<regex_data>::const_iterator it = r.not_match.begin(); it != r.not_match.end(); ++it)
        {
            out<< *it;
        }
        out<< struct_delim::value;
        return out;
    }
    
    std::istream& operator>>(std::istream& in, regex_settings_data& r)
    {
        //test
        r = regex_settings_data();
        if(in.good())
        {
            bool tempb;
            in_memory_of<bool>(in, tempb);
            r.use_regex = tempb;
            in_memory_of<bool>(in, tempb);
            r.use_match = tempb;
            in_memory_of<bool>(in, tempb);
            r.use_not_match = tempb;
            in.peek();
            while(in.good() && (in.peek() != struct_delim::value))
            {
                r.match.push_back(regex_data());
                in>> r.match.back();
            }
            if(in.peek() == struct_delim::value) in.get();
            in.get();
            in.unget();
            while(in.good() && (in.peek() != struct_delim::value))
            {
                r.not_match.push_back(regex_data());
                in>> r.not_match.back();
            }
            if(in.peek() == struct_delim::value) in.get();
        }
        in.peek();
        return in;
    }
    
    
}

/** settings_data member functions: */
namespace settings
{
    settings_data::settings_data() noexcept : regex_settings(),
            editor(),
            global()
    {
    }
    
    settings_data& settings_data::operator=(const settings_data& s) noexcept
    {
        if(this != &s)
        {
            this->editor = s.editor;
            this->regex_settings = s.regex_settings;
        }
        return *this;
    }
    
    bool settings_data::operator==(const settings_data& s) const noexcept
    {
        return ((this->regex_settings == s.regex_settings) && 
                (this->global == s.global) && 
                (this->editor == s.editor));
    }
    
    bool settings_data::operator!=(const settings_data& s) const noexcept
    {
        return ((this->regex_settings != s.regex_settings) ||
                (this->global != s.global) || 
                (this->editor != s.editor));
    }
    
    std::ostream& operator<<(std::ostream& out, const settings_data& s)
    {
        //test
        if(out.good())
        {
            out<< s.regex_settings;
            out<< s.editor<< mem_delim::value;
        }
        return out;
    }
    
    std::istream& operator>>(std::istream& in, settings_data& s)
    {
        //test
        s = settings_data();
        in.peek();
        if(in.good())
        {
            in>> s.regex_settings;
            common::safe_getline(in, s.editor, mem_delim::value);
        }
        return in;
    }
    
    
}

/** setting_constants_data member functions: */
namespace settings
{
    setting_constants_data::setting_constants_data() noexcept : 
            snapshot_folder(snapshot::snapshot_folder()),
            records_folder(snapshot_folder + fsys::pref_slash() + std::string("comparison records"))
    {
    }
    
    setting_constants_data::~setting_constants_data() noexcept
    {
    }
    
    setting_constants_data& setting_constants_data::operator=(const setting_constants_data& s) noexcept
    {
        if(this != &s)
        {
            this->snapshot_folder = s.snapshot_folder;
            this->records_folder = s.records_folder;
        }
        return *this;
    }
    
    bool setting_constants_data::operator==(const setting_constants_data& s) const noexcept
    {
        return ((this->snapshot_folder == s.snapshot_folder) && 
                (this->records_folder == s.records_folder));
    }
    
    bool setting_constants_data::operator!=(const setting_constants_data& s) const noexcept
    {
        return ((this->snapshot_folder != s.snapshot_folder) || 
                (this->records_folder != s.records_folder));
    }
    
    
}

namespace settings
{
    /** Applies user settings to the regular expression lists.  Returns a std::pair 
     * containing a list of regular expressions that can be used. */
    std::pair<std::vector<regex_data>, std::vector<regex_data> > used_expressions(const settings_data& s)
    {
        std::pair<std::vector<regex_data>, std::vector<regex_data> > exp;
        
        if(s.regex_settings.use_regex)
        {
            if(s.regex_settings.use_match)
            {
                exp.first = apply_filter(s.regex_settings.match);
            }
            if(s.regex_settings.use_not_match)
            {
                exp.second = apply_filter(s.regex_settings.not_match);
            }
        }
        return exp;
    }
    
    
}

