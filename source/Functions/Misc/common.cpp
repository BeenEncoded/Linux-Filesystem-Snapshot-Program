#include <iostream>
#include <string>
#include <vector>
#include <ctime>

#include "iofunctions.hpp"
#include "global_defines.hpp"
#include "common.hpp"
#include "scroll_display.hpp"
#include "filesystem.hpp"

namespace
{
    bool char_match(const std::string&, const char&) noexcept;
    
    
    inline bool char_match(const std::string& s, const char& ch) noexcept
    {
        for(std::string::const_iterator it = s.begin(); it != s.end(); ++it)
        {
            if(*it == ch) return true;
        }
        return false;
    }
    
    
}

namespace common
{
    void cl() noexcept
    {
        input::cl();
    }
    
    void wait() noexcept
    {
        using std::cout;
        using std::endl;
        
        for(unsigned int x = 0; x < 3; x++) cout<< endl;
        cout<< "Press anything to continue..."<< endl;
        input::cl();
        input::getch();
        input::cl();
    }
    
    bool kbhit() noexcept
    {
        return input::kbhit();
    }
    
    key_code::key_code_data gkey()
    {
        input::cl();
        return input::getch();
    }
    
    key_code::key_code_data getch_funct()
    {
        using key_code::key_code_data;
        using key_code::is_listed_control;
        using key_code::might_be_control;
        
        //initialize a complex key code: we need to be ready for anything: 
        key_code_data key;
        
        key.is_control = true;
        do
        {
            key.control_d.push_back(input::getch().control_d.front());
        }while(might_be_control(key) && input::kbhit() && !is_listed_control(key));
        
        if(!might_be_control(key) && !key.control_d.empty() && !is_listed_control(key))
        {
            if(key.control_d.size() == 1) key = key_code_data((char)key.control_d.front());
        }
        
        return key;
    }
    
    void cls() noexcept
    {
        output::cls();
    }
    
    key_code::key_code_data gkey_funct()
    {
        input::cl();
        return getch_funct();
    }
    
    bool is_letter(const char& ch) noexcept
    {
        return char_match(LETTERS, ch);
    }
    
    bool is_number(const char& ch) noexcept
    {
        return char_match(NUMBERS, ch);
    }
    
    bool is_special(const char& ch) noexcept
    {
        return char_match(SPECIALS, ch);
    }
    
    bool is_char(const char& ch) noexcept
    {
        return char_match((NUMBERS + std::string(LETTERS) + SPECIALS), ch);
    }
    
    /*Display a string in the horizontal center of the screen. */
    void center(const std::string& message) noexcept
    {
        using std::cout;
        
        int cent(h_center::value - (message.size() / 2));
        if(cent < 0) cent = 0;
        if(cent > 0) cout<< std::string(cent, ' ');
        cout<< message;
    }
    
    struct tm get_time()
    {
        struct tm temptime;
        time_t rawtime;
        time(&rawtime);
        temptime = (*(localtime(&rawtime)));
        return temptime;
    }
    
    void display_scroll_window(scrollDisplay::scroll_display_class& win, const unsigned int& whole_size)
    {
        using std::cout;
        using std::endl;
        
        output::cls();
        
        std::vector<std::string> tempv(win.window());
        unsigned int size(tempv.size());
        
        if(win.window_beg() > 0)
        {
            common::center("^^ " + std::to_string(win.window_beg()) + " ^^");
            cout<< endl;
            cout<< std::string(70, '^')<< endl;
        }
        else cout<< endl<< endl;
        
        for(int x = 0; x < (signed)size; x++)
        {
            if(win.gpos().part == x) cout<< " [";
            else cout<< "  ";
            
            cout<< tempv[win.gpos().part];
            
            if(win.gpos().part == x) cout<< "]";
            
            cout<< endl;
        }
        
        if((whole_size - (win.window_beg() + win.window_size())) > 0)
        {
            cout<< std::string(70, 'v')<< endl;
            common::center(std::to_string((whole_size - (win.window_beg() + win.window_size()))));
            cout<< endl;
        }
        else cout<< endl<< endl;
    }
    
    std::string parent_folder(const std::string& f) noexcept
    {
        std::string temps(f);
        std::string::size_type pos(temps.rfind(fsys::pref_slash()));
        
        if(!f.empty())
        {
            if(pos != std::string::npos)
            {
                temps.erase((temps.begin() + pos), temps.end());
            }
        }
        return temps;
    }
    
    bool string_begins_with(const std::string& s, const std::string& beg) noexcept
    {
        return (s.find(beg) == 0);
    }
    
    bool string_ends_with(const std::string& s, const std::string& end) noexcept
    {
        return (s.rfind(end) == (s.size() - end.size()));
    }
    
    namespace inp
    {
        
        bool is_sure(const std::string& s)
        {
            using std::cout;
            using std::endl;
            using key_code::key_code_data;
            
            bool sure(false), finished(false);
            key_code_data ch;
            do
            {
                output::cls();
                for(unsigned int x = 0; x < v_center::value; x++) cout<< endl;
                center(((s.size() > 0) ? s : "Are you sure??"));
                cout<< endl;
                cout<< endl;
                center("Y/N");
                
                ch = common::gkey();
                if(!ch.control_d.empty() && !ch.is_control)
                {
                    if(is_char((char)ch.control_d.front()))
                    {
                        switch(tolower((char)ch.control_d.front()))
                        {
                            case 'y':
                            {
                                finished = true;
                                sure = true;
                            }
                            break;
                            
                            case 'n':
                            {
                                finished = true;
                                sure = false;
                            }
                            break;
                            
                            default:
                            {
                            }
                            break;
                        }
                    }
                }
            }while(!finished);
            return sure;
        }
        
        std::string get_user_string(const std::string& message)
        {
            using std::cout;
            using std::endl;
            using namespace key_code;
            
            std::string input;
            key_code_data key;
            bool finished(false);
            
            input::cl();
            do
            {
                if(!input::kbhit())
                {
                    output::cls();
                    cout<< "[END] - CANCEL  |||  [DEL] - Clear input"<< endl;
                    for(unsigned int x = 1; x < v_center::value; x++) cout<< endl;
                    cout<< (message.empty() ? ">> " : message)<< input;
                    cout.flush();
                }
                
                key = getch_funct();
                if(is_listed_control(key))
                {
                    using namespace key_code::code;
                    if(key == keys[backspace::value])
                    {
                        if(input.size() > 0) input.resize((input.size() - 1));
                    }
                    else if(key == keys[end::value])
                    {
                        input = GSTRING_CANCEL;
                        finished = true;
                    }
                    else if(key == keys[del::value])
                    {
                        if(!input.empty()) input.erase();
                    }
                }
                else
                {
                    if(!key.is_control && !key.control_d.empty())
                    {
                        if(is_char((char)key.control_d.front()))
                        {
                            input += (char)key.control_d.front();
                        }
                        else
                        {
                            if(key == key_code_data('\n'))
                            {
                                finished = true;
                            }
                        }
                    }
                }
                
            }while(!finished);
            return input;
        }
        
    }
    
    
}