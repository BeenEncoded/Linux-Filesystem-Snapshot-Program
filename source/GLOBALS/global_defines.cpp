#include <vector>

#include "global_defines.hpp"

namespace
{
}

namespace key_code
{
    key_code_data::key_code_data(const key_code_data& k)
    {
        this->is_control = k.is_control;
        if(this->is_control) this->control_d = k.control_d;
        else this->ch_d = k.ch_d;
    }
    
    key_code_data::key_code_data(const char& c)
    {
        new (&(this->ch_d)) char(c);
        this->is_control = false;
    }
    
    key_code_data::key_code_data(const std::vector<int>& c)
    {
        new (&(this->control_d)) std::vector<int>(c);
        this->is_control = true;
    }
    
    key_code_data::key_code_data()
    {
        this->is_control = false;
        this->ch_d = 0x00;
    }
    
    key_code_data::~key_code_data()
    {
    }
    
    char& key_code_data::ch()
    {
        if(this->is_control)
        {
            this->control_d.~vector();
            new (&(this->ch_d)) char();
            this->is_control = false;
        }
        return this->ch_d;
    }
    
    /* Warning: reading constant value can only happen directly, 
     so if this isn't being used as a character, it will throw on error.*/
    const char& key_code_data::ch() const
    {
        if(!this->is_control) return this->ch_d;
        else throw "NOT A CHARACTER; You must first write a character to be able to read.";
        
        return this->ch_d;
    }
    
    std::vector<int>& key_code_data::control()
    {
        if(!this->is_control)
        {
            new (&(this->control_d)) std::vector<int>();
            this->is_control = true;
        }
        return this->control_d;
    }
    
    /* Warning: reading constant value can only happen directly, 
     so if this isn't being used as a control, it will throw on error.*/
    const std::vector<int>& key_code_data::control() const
    {
        if(this->is_control) return this->control_d;
        else throw "NOT A CONTROL KEY; You must first write a control key to be able to read.";
        
        return this->control_d;
    }
    
    const key_code_data& key_code_data::operator=(const key_code_data& k)
    {
        if(this != &k)
        {
            if(this->is_control) this->control_d.~vector();
            
            this->is_control = k.is_control;
            
            if(k.is_control)
            {
                new (&(this->control_d)) std::vector<int>(k.control_d);
            }
            else new (&(this->ch_d)) char(k.ch_d);
        }
        return *this;
    }
    
    bool key_code_data::operator==(const key_code_data& k) const
    {
        /* Due to the nature of the union, we will have to make sure that
         we read from the correct character first: */
        if(this->is_control == k.is_control)
        {
            if(this->is_control) return (this->control_d == k.control_d);
            else return (this->ch_d == k.ch_d);
        }
        
        return false;
    }
    
    bool key_code_data::operator!=(const key_code_data& k) const
    {
        return !(this->operator==(k));
    }
    
    std::vector<key_code_data> keys{
        key_code_data((char)127), //backspace
        key_code_data((char)27),  //escape
        key_code_data(std::vector<int>{27, 91, 51, 126}), //delete
        key_code_data(std::vector<int>{27, 91, 50, 126}), //insert
        key_code_data(std::vector<int>{27, 79, 70}),      //end
        key_code_data(std::vector<int>{27, 79, 72}),      //home
        key_code_data(std::vector<int>{27, 91, 65}),      //up
        key_code_data(std::vector<int>{27, 91, 66}),      //down
        key_code_data(std::vector<int>{27, 91, 68}),      //left
        key_code_data(std::vector<int>{27, 91, 67}),      //right
        key_code_data(std::vector<int>{27, 91, 53, 126}),  //pgup
        key_code_data(std::vector<int>{27, 91, 54, 126}), //pgdown
        key_code_data((char)0x00), //f1 key: could not reproduce; reason: terminal grabbed it
        key_code_data(std::vector<int>{27, 79, 81}), //f2
        key_code_data(std::vector<int>{27, 79, 82}), //f3
        key_code_data(std::vector<int>{27, 79, 83}), //f4
        key_code_data(std::vector<int>{27, 91, 49, 53, 126}), //f5
        key_code_data(std::vector<int>{27, 91, 49, 55, 126}), //f6
        key_code_data(std::vector<int>{27, 91, 49, 56, 126}), //f7
        key_code_data(std::vector<int>{27, 91, 49, 57, 126}), //f8
        key_code_data(std::vector<int>{27, 91, 50, 48, 126}), //f9
        key_code_data((char)0x00), //could not reproduce F10: terminal grabbed it
        key_code_data((char)0x00), //could not reproduce F11: terminal grabbed it
        key_code_data(std::vector<int>{27, 91, 50, 52, 126}) //f12
    };
    
    bool is_control(const key_code_data& k)
    {
        for(std::vector<key_code_data>::const_iterator it = keys.begin(); it != keys.end(); ++it)
        {
            if(*it == k) return true;
        }
        return false;
    }
    
    bool might_be_control(const key_code_data& k)
    {
        if(k.is_control)
        {
            if(k.control().size() > 0)
            {
                return (k.control()[0] == 27);
            }
        }
        return false;
    }
    
}