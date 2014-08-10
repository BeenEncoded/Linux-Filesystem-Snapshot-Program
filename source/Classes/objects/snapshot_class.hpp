#ifndef SNAPSHOT_CLASS_HPP_INCLUDED
#define SNAPSHOT_CLASS_HPP_INCLUDED
#include <string>
#include <vector>
#include <iostream>
#include <ctime>

#include "date_class.hpp"

namespace snapshot
{
    struct snapshot_data;
    
    
    std::istream& operator>>(std::istream&, snapshot_data&);
    std::ostream& operator<<(std::ostream&, snapshot_data&);
    
    struct snapshot_data
    {
        std::vector<std::string> paths;
        date::date_val timestamp;
    };
    
    bool take_snapshot(snapshot_data&);
    
}

#endif