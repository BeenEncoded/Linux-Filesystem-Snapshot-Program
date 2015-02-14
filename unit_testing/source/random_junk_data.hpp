#ifndef RANDOM_JUNK_DATA_HPP_INCLUDED
#define RANDOM_JUNK_DATA_HPP_INCLUDED
#include <string>

#include "snapshot_class.hpp"

namespace rand_data
{
    unsigned int random_number(const unsigned int&, const unsigned int&);
    char random_char();
    std::string random_str(const unsigned int&, const unsigned int&);
    snapshot::snapshot_data random_snapshot();
    bool random_bool();
    
}

#endif