#include <string>
#include <vector>
#include <iostream>

#include "snapshot_class.hpp"
#include "common.hpp"
#include "date_class.hpp"
#include "global_defines.hpp"

namespace
{
}

/* stream operators: */
namespace snapshot
{
    std::ostream& operator<<(std::ostream& out, const snapshot_data& snap)
    {
        if(out.good())
        {
            out<< snap.timestamp;
            out<< snap.id<< mem_delim::value;
            out<< snap.root<< mem_delim::value;
            for(std::vector<std::string>::const_iterator it = snap.paths.begin(); 
                    it != snap.paths.end(); ++it)
            {
                out<< *it<< mem_delim::value;
            }
            out<< struct_delim::value;
        }
        return out;
    }
    
    std::istream& operator>>(std::istream& in, snapshot_data& snap)
    {
        using common::safe_getline;
        
        std::string temps;
        
        snap = snapshot_data();
        if(in.good())
        {
            in>> snap.timestamp;
            
            if(safe_getline(in, snap.id, mem_delim::value))
                safe_getline(in, snap.root, mem_delim::value);
            
            while(in.good() && (in.peek() != struct_delim::value))
            {
                if(safe_getline(in, temps, mem_delim::value)) snap.paths.push_back(temps);
            }
            if(in.peek() == struct_delim::value) in.get();
            
            in.peek();
        }
        return in;
    }
    
    const snapshot_data& snapshot_data::operator=(const snapshot_data& snap)
    {
        if(this != &snap)
        {
            this->root = snap.root;
            
            this->paths = snap.paths;
            this->paths.shrink_to_fit();
            
            this->timestamp = snap.timestamp;
            this->id = snap.id;
        }
        return this;
    }
    
    
}