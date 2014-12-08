#ifndef SETTINGS_MENU_HPP_INCLUDED
#define SETTINGS_MENU_HPP_INCLUDED
#include <vector>
#include <string>

#include "common_menu.hpp"
#include "program_settings.hpp"

namespace menu
{
    common_menu::menu_return_data modify_regex_list(std::vector<settings::regex_data>&, const std::string&);
    common_menu::menu_return_data modify_regex_settings(settings::regex_settings_data&);
    common_menu::menu_return_data modify_editor_settings(settings::editor_data&);
    common_menu::menu_return_data modify_program_settings(settings::settings_data&);
}

#endif