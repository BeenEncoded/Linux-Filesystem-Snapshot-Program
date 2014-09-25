#include <iostream>
#include <string>
#include <vector>

#include "snapshot_file_loaders.hpp"
#include "modify_snapshot_menu.hpp"

namespace
{
    void main_menu();
    
    
    
    inline void main_menu()
    {
        using snapshot_menu::main_snapshot_menu;
        using snapshot::list_snapshot_info;
        using snapshot::snapshot_folder;
        using snapshot::snapshot_data;
        
        std::vector<snapshot_data> snaps(list_snapshot_info(snapshot_folder()));
        
        main_snapshot_menu(snaps);
    }
}

//cur_pos figure out how to link multi-threading
int main(__attribute__((unused)) int c, __attribute__((unused)) char **v)
{
    main_menu();
    return 0;
}