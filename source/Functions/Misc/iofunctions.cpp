#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <termios.h>
#include <fcntl.h>
#include <term.h>
#include <cstring>

#include "iofunctions.hpp"
#include "global_defines.hpp"

namespace input
{
    key_code::key_code_data getch()
    {
        int ch;
        struct termios temp_term, cur_term;
        tcgetattr(STDIN_FILENO, &cur_term);
        temp_term = cur_term;
        temp_term.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &temp_term);
        
        ch = getchar();
        
        tcsetattr(STDIN_FILENO, TCSANOW, &cur_term);
        
        return key_code::key_code_data(ch);
    }
    
    bool kbhit()
    {
        int ch, oldf;
        bool button_hit(false);
        struct termios temp_term, cur_term;
        
        tcgetattr(STDIN_FILENO, &cur_term);
        temp_term = cur_term;
        temp_term.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &temp_term);
        oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
        fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);
        
        ch = getchar();
        
        tcsetattr(STDIN_FILENO, TCSANOW, &cur_term);
        fcntl(STDIN_FILENO, F_SETFL, oldf);
        if(ch != EOF)
        {
            ungetc(ch, stdin);
            button_hit = true;
        }
        return button_hit;
    }
    
    void cl()
    {
        int ch, oldf;
        struct termios cur_term, new_term;
        tcgetattr(STDIN_FILENO, &cur_term);
        new_term = cur_term;
        new_term.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &new_term);
        oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
        fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);
        
        do
        {
            ch = getchar();
        }
        while(ch != EOF);
        ungetc(ch, stdin);
        
        tcsetattr(STDIN_FILENO, TCSANOW, &cur_term);
        fcntl(STDIN_FILENO, F_SETFL, oldf);
    }
}

namespace output
{
    void cls()
    {
        char *temps(new char[10]);
        
        /*the only way I could get the compiler to not perform a direct 
         * initialization of the array (and giving a -Wwrite-strings error due 
         * to implicit conversion from a const char* -> char*): */
        std::memcpy(temps, "clear\0", sizeof "clear\0");
        if (!cur_term)
        {
            int result;
            setupterm( NULL, STDOUT_FILENO, &result );
            if (result <= 0)
            {
                delete[] temps;
                return;
            }
        }
        putp( tigetstr( temps ) );
        delete[] temps;
    }
    
    
}