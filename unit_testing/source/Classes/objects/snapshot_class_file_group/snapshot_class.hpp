#ifndef SNAPSHOT_CLASS_HPP_INCLUDED
#define SNAPSHOT_CLASS_HPP_INCLUDED
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <ctime>

#include "time_class.hpp"

namespace snapshot
{
    struct snapshot_data;
    
    std::istream& operator>>(std::istream&, snapshot_data&);
    std::ostream& operator<<(std::ostream&, const snapshot_data&);
    
    unsigned long long take_snapshot(const std::string&);
    std::ostream& out_header(std::ostream&, const snapshot_data&);
    std::istream& in_header(std::istream&, snapshot_data&);
    
    struct snapshot_data
    {
        const snapshot_data& operator=(const snapshot_data&);
        bool operator==(const snapshot_data&) const;
        bool operator!=(const snapshot_data&) const;
        bool operator<(const snapshot_data&) const;
        
        void take_time();
        
        std::string root;
        std::vector<std::string> paths;
        tdata::time_class timestamp;
        unsigned long long id = 0;
    };
    
}

#endif