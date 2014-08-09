#ifndef GLOBAL_DEFINES_HPP_INCLUDED
#define GLOBAL_DEFINES_HPP_INCLUDED
#include <vector>

#define LETTERS "qwertyuiopasdfghjklzxcvbnmQWERTYUIOPASDFGHJKLZXCVBNM"
#define NUMBERS "1234567890"
#define SPECIALS "`~!@#$%^&*()-_=+[{]}\\|;:\'\",<.>/? "
#define GSTRING_CANCEL "oiwydfp980  q30f8p90c9-i130894f=i-=23f=-    0w2g]["

template<char>
struct const_char_type
{
    static char value;
};

template<unsigned int>
struct const_int_type
{
    static unsigned int value;
};

template<char type> char const_char_type<type>::value = type;
template<unsigned int type> unsigned int const_int_type<type>::value = type;

typedef const_char_type<0x00> mem_delim;
typedef const_char_type<0x01> struct_delim;
typedef const_char_type<0x02> submem_delim;
typedef const_char_type<0x03> substruct_delim;

typedef const_int_type<40> h_center;
typedef const_int_type<8> v_center;

namespace key_code
{
    struct key_code_data;
    
    class key_code_data
    {
    public:
        key_code_data(const key_code_data&);
        explicit key_code_data(const std::vector<int>&);
        explicit key_code_data(const char&);
        explicit key_code_data();
        
        ~key_code_data();
        
        const key_code_data& operator=(const key_code_data&);
        bool operator==(const key_code_data&) const;
        bool operator!=(const key_code_data&) const;
        
        /* Allows the safe reference of the union variables: */
        char& ch();
        const char& ch() const;
        
        std::vector<int>& control();
        const std::vector<int>& control() const;
        
        //I try to make this as small as possible:
        bool is_control : 1;
        
        union
        {
            char ch_d;
            std::vector<int> control_d;
        };
    };
    
    namespace code
    {
        typedef const_int_type<0> backspace;
        typedef const_int_type<1> escape;
        typedef const_int_type<2> del;
        typedef const_int_type<3> insert;
        typedef const_int_type<4> end;
        typedef const_int_type<5> home;
        typedef const_int_type<6> up;
        typedef const_int_type<7> down;
        typedef const_int_type<8> left;
        typedef const_int_type<9> right;
        typedef const_int_type<10> pgup;
        typedef const_int_type<11> pgdown;
        typedef const_int_type<12> f1;
        typedef const_int_type<13> f2;
        typedef const_int_type<14> f3;
        typedef const_int_type<15> f4;
        typedef const_int_type<16> f5;
        typedef const_int_type<17> f6;
        typedef const_int_type<18> f7;
        typedef const_int_type<19> f8;
        typedef const_int_type<20> f9;
        typedef const_int_type<21> f10;
        typedef const_int_type<22> f11;
        typedef const_int_type<23> f12;
    }
    
    /* A vector containing all the key codes.  Can be changed if necessary.
     I was thinking about adding a "calibration" menu or somthing in case
     the key codes change.*/
    extern std::vector<key_code_data> keys;
    
    bool is_control(const key_code_data&);
    bool might_be_control(const key_code_data&);
    
    
}

#endif