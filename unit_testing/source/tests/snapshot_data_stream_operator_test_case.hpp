/** This test case tests the stream operators for snapshot::snapshot_data
 * to make sure that they work properly under all circumstances. */

#include <unittest++/UnitTest++.h>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>

#include "random_junk_data.hpp"
#include "snapshot_class.hpp"
#include "common.hpp"
#include "global_defines.hpp"

using snapshot::snapshot_data;


namespace
{
    std::vector<snapshot::snapshot_data> sstosnap(std::stringstream&);
    std::stringstream& snaptoss(const std::vector<snapshot::snapshot_data>&, std::stringstream&);
    bool snapshot_stream_good(const snapshot::snapshot_data&);
    bool snapshot_tor_stream_good(const std::vector<snapshot::snapshot_data>&);
    std::vector<snapshot::snapshot_data> random_snapshot_list();
    
    
    
    
    inline std::vector<snapshot::snapshot_data> sstosnap(std::stringstream& ss)
    {
        std::vector<snapshot::snapshot_data> snapshots;
        while(ss.good())
        {
            snapshots.push_back(snapshot::snapshot_data());
            ss>> snapshots.back();
            if(!ss.good()) snapshots.pop_back();
        }
        return snapshots;
    }
    
    inline std::stringstream& snaptoss(const std::vector<snapshot::snapshot_data>& s, std::stringstream& ss)
    {
        for(std::vector<snapshot::snapshot_data>::const_iterator it = s.begin(); it != s.end(); ++it)
        {
            ss<< (*it);
        }
        return ss;
    }
    
    inline bool snapshot_stream_good(const snapshot::snapshot_data& snap)
    {
        std::stringstream ss;
        snapshot::snapshot_data tempsnap;
        
        ss<< snap;
        ss>> tempsnap;
        return (tempsnap == snap);
    }
    
    inline bool snapshot_tor_stream_good(const std::vector<snapshot::snapshot_data>& snaps)
    {
        std::stringstream ss;
        std::vector<snapshot::snapshot_data> tempv(sstosnap(snaptoss(snaps, ss)));
        
        return (tempv == snaps);
    }
    
    inline std::vector<snapshot::snapshot_data> random_snapshot_list()
    {
        std::vector<snapshot::snapshot_data> tempv;
        std::vector<snapshot::snapshot_data>::size_type size(rand_data::random_number(0, 100));
        
        for(std::vector<snapshot::snapshot_data>::size_type x = 0; x < size; x++) tempv.push_back(rand_data::random_snapshot());
        return tempv;
    }
    
    
}

SUITE(snapshot_data_tests_test_suite)
{
    TEST(snapshot_data_stream_operator_test_case)
    {
        bool tempb(false);
        for(unsigned int x = 0; x < 20; x++)
        {
            tempb = snapshot_stream_good(rand_data::random_snapshot());
            CHECK(tempb);
            tempb = snapshot_tor_stream_good(random_snapshot_list());
            CHECK(tempb);
        }
    }
    
    
}
