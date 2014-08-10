#include <string>
#include <vector>
#include <iostream>

#include "snapshot_class.hpp"
#include "common.hpp"
#include "date_class.hpp"
#include "global_defines.hpp"
#include "filesystem.hpp"

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
            while(in.good() && (in.peek() != struct_delim::value))
            {
                if(safe_getline(in, temps, mem_delim::value)) snap.paths.push_back(temps);
            }
            if(in.peek() == struct_delim::value) in.get();
            
            in.peek();
        }
        return in;
    }
    
    
}

/* Misc functions: */
namespace snapshot
{
    bool take_snapshot(snapshot_data& snap)
    {
        fsys::tree_riterator_class it;
    }
    
    
}