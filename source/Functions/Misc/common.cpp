#include <iostream>
#include <string>
#include <vector>

#include "iofunctions.hpp"
#include "global_defines.hpp"
#include "common.hpp"

namespace
{
    bool char_match(const std::string&, const char&);
    
    
    inline bool char_match(const std::string& s, const char& ch)
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
    void cl()
    {
        input::cl();
    }
    
    void wait()
    {
        using std::cout;
        using std::endl;
        
        for(unsigned int x = 0; x < 3; x++) cout<< endl;
        cout<< "Press anything to continue..."<< endl;
        input::cl();
        input::getch();
        input::cl();
    }
    
    key_code::key_code_data gkey()
    {
        input::cl();
        return input::getch();
    }
    
    key_code::key_code_data getch_funct()
    {
        using key_code::key_code_data;
        
        key_code_data key;
        char ch;
        
        do
        {
            key.control().push_back((int)input::getch().ch());
        }while(key_code::might_be_control(key) && input::kbhit() && 
                !key_code::is_control(key));
        
        if(!key_code::might_be_control(key) && (key.control().size() > 0) && 
                !key_code::is_control(key))
        {
            ch = key.control()[0];
            key.ch() = ch;
        }
        
        return key;
    }
    
    void cls()
    {
        output::cls();
    }
    
    key_code::key_code_data gkey_funct()
    {
        input::cl();
        return getch_funct();
    }
    
    bool is_letter(const char& ch)
    {
        return char_match(LETTERS, ch);
    }
    
    bool is_number(const char& ch)
    {
        return char_match(NUMBERS, ch);
    }
    
    bool is_special(const char& ch)
    {
        return char_match(SPECIALS, ch);
    }
    
    bool is_char(const char& ch)
    {
        return char_match((NUMBERS + std::string(LETTERS) + SPECIALS), ch);
    }
    
    /*Display a string in the horizontal center of the screen. */
    void center(const std::string& message)
    {
        using std::cout;
        
        int cent(h_center::value - (message.size() / 2));
        if(cent < 0) cent = 0;
        if(cent > 0) cout<< std::string(cent, ' ');
        cout<< message;
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
                if(is_char(ch.ch()))
                {
                    switch(tolower(ch.ch()))
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
                if(is_control(key))
                {
                    if(key == keys[code::backspace::value])
                    {
                        if(input.size() > 0) input.resize((input.size() - 1));
                    }
                    else if(key == keys[code::end::value])
                    {
                        input = GSTRING_CANCEL;
                        finished = true;
                    }
                    else if(key == keys[code::del::value])
                    {
                        if(!input.empty()) input.erase();
                    }
                }
                else
                {
                    if(!key.is_control)
                    {
                        if(is_char(key.ch()))
                        {
                            input += key.ch();
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
