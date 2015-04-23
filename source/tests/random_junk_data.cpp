#if UNIT_TEST_PROG == true
#include <string>
#include <cstdlib>

#include "global_defines.hpp"
#include "random_junk_data.hpp"
#include "snapshot_class.hpp"
#include "common.hpp"
#include "time_class.hpp"

namespace
{
}

namespace rand_data
{
    /** Generates a random number from min to max (inclusive) */
    unsigned int random_number(const unsigned int& min, const unsigned int& max)
    {
        return ((rand() % ((max + 1) - min)) + min);
    }
    
    char random_char()
    {
        std::string temps(LETTERS + std::string(NUMBERS) + SPECIALS);
        return temps.at(random_number(0, (temps.size() - 1)));
    }
    
    std::string random_str(const unsigned int& min, const unsigned int& max)
    {
        std::string temps;
        unsigned int size(random_number(min, max));
        for(unsigned int x = 0; x < size; x++) temps += random_char();
        return temps;
    }
    
    snapshot::snapshot_data random_snapshot()
    {
        using snapshot::snapshot_data;
        
        snapshot_data tempsnap;
        unsigned int size(random_number(5000, 10000));
        
        tempsnap.timestamp = tdata::current_time();
        tempsnap.root = "/";
        tempsnap.id = random_number(1, 500);
        for(unsigned int x = 0; x < size; x++)
        {
            tempsnap.paths.push_back(random_str(5, 75));
        }
        return tempsnap;
    }
    
    bool random_bool()
    {
        return random_number(0, 1);
    }
    
    
}

#endif