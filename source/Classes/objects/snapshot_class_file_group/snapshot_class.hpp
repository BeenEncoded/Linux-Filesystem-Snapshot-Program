#ifndef SNAPSHOT_CLASS_HPP_INCLUDED
#define SNAPSHOT_CLASS_HPP_INCLUDED
#include <string>
#include <vector>
#include <iostream>

#include "time_class.hpp"

namespace snapshot
{
    struct snapshot_data;
    
    std::istream& operator>>(std::istream&, snapshot_data&) noexcept;
    std::ostream& operator<<(std::ostream&, const snapshot_data&) noexcept;
    
    unsigned long long take_snapshot(const std::string&) noexcept;
    std::ostream& out_header(std::ostream&, const snapshot_data&) noexcept;
    std::istream& in_header(std::istream&, snapshot_data&) noexcept;
    
    struct snapshot_data
    {
        explicit snapshot_data() noexcept {}
        ~snapshot_data() noexcept {}
        
        const snapshot_data& operator=(const snapshot_data&) noexcept;
        bool operator==(const snapshot_data&) const noexcept;
        bool operator!=(const snapshot_data&) const noexcept;
        bool operator<(const snapshot_data&) const noexcept;
        
        
        std::string root;
        std::vector<std::string> paths;
        tdata::time_class timestamp;
        unsigned long long id = 0;
    };
    
}

#endif