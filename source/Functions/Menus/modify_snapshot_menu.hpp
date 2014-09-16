#ifndef MODIFY_SNAPSHOT_MENU_HPP_INCLUDED
#define MODIFY_SNAPSHOT_MENU_HPP_INCLUDED
#include "snapshot_class.hpp"
#include "common_menu.hpp"

namespace snapshot_menu
{
    common_menu::menu_return_data main_snapshot_menu(std::vector<snapshot::snapshot_data>&);
    
}

#endif