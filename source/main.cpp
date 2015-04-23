
#include "main_menu.hpp"
#include "program_settings.hpp"
#include "settings_loader.hpp"

#if UNIT_TEST_PROG == true
#include "tests/tests_included.hpp"
#endif


namespace
{
    void main_menu();
    

    __attribute__((unused)) inline void main_menu()
    {
        using settings::settings_data;
        
        settings_data program_settings;
        program_settings = settings::load(program_settings.global.settings_folder);
        
        menu::main_menu(program_settings);
    }
    
    
}

int main(__attribute__((unused)) int c, __attribute__((unused)) char **v)
{
#if UNIT_TEST_PROG == true
    return run_tests();
#else
    main_menu();
    return 0;
#endif
}
