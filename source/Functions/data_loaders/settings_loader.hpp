#ifndef SETTINGS_LOADER_HPP_INCLUDED
#define SETTINGS_LOADER_HPP_INCLUDED
#include <string>

#include "program_settings.hpp"


namespace settings
{
    std::string settings_folder() noexcept;
    settings::settings_data load(const std::string&);
    bool save(const std::string&, const settings::settings_data&);
    
}

#endif