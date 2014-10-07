#include <iostream>

#include "snapshot_file_loaders.hpp"
#include "modify_snapshot_menu.hpp"

namespace
{
    void main_menu();
    
    
    
    inline void main_menu()
    {
        snapshot_menu::main_snapshot_menu(snapshot::snapshot_folder());
    }
}

int main(__attribute__((unused)) int c, __attribute__((unused)) char **v)
{
    main_menu();
    return 0;
}