#ifndef SNAPSHOT_CLASS_HPP_INCLUDED
#define SNAPSHOT_CLASS_HPP_INCLUDED
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <ctime>

#include "date_class.hpp"

namespace snapshot
{
    struct snapshot_data;
    
    std::istream& operator>>(std::istream&, snapshot_data&);
    std::ostream& operator<<(std::ostream&, const snapshot_data&);
    
    struct snapshot_data
    {
        const snapshot_data& operator=(const snapshot_data&);
        bool operator==(const snapshot_data&) const;
        bool operator!=(const snapshot_data&) const;
        
        std::string root;
        std::vector<std::string> paths;
        date::date_val timestamp;
        unsigned long long id = 0;
    };
    
    unsigned long long take_snapshot(const std::string&);
    
}

#endif