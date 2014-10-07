#include <unittest++/UnitTest++.h>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <exception>
#include <unordered_map>
#include <utility>

#include "snapshot_class.hpp"
#include "snapshot_file_loaders.hpp"
#include "random_junk_data.hpp"
#include "filesystem.hpp"
#include "global_defines.hpp"

namespace test
{
    typedef std::unordered_map<unsigned long long, snapshot::snapshot_data> ss_hasht;
    
    class fixture_class;
    
    bool save_snapshot(const snapshot::snapshot_data&);
    bool snapshot_header_eq(const snapshot::snapshot_data&, const snapshot::snapshot_data&);
    bool delete_folder(const std::string&);
    bool snapshot_list_eq(const std::vector<snapshot::snapshot_data>&, 
                    const std::vector<snapshot::snapshot_data>&);
    ss_hasht hash_snapshots(const std::vector<snapshot::snapshot_data>&);
    
    
    class fixture_class
    {
    public:
        fixture_class()
        {
            bool tempb(true);
            snapshot::snapshot_data tempsnap;
            
            key_code::keys[key_code::code::backspace::value];
            for(unsigned int x = 0; ((x < 1) && tempb); x++)
            {
                tempsnap = rand_data::random_snapshot();
                tempsnap.id = snapshot::new_snapshot_id();
                tempb = save_snapshot(tempsnap);
                if(tempb) this->snaps.push_back(tempsnap);
            }
        }
        
        ~fixture_class()
        {
            delete_folder(snapshot::snapshot_folder());
        }
        
        
    protected:
        std::string filename;
        std::vector<snapshot::snapshot_data> snaps;
        
    };
    
    inline bool save_snapshot(const snapshot::snapshot_data& snap)
    {
        using fsys::is_folder;
        using fsys::is_symlink;
        
        std::string folder(snapshot::snapshot_folder());
        bool success(false);
        std::ofstream out;
        
        if(!is_folder(folder).value)
        {
            fsys::create_folder(folder);
        }
        if(is_folder(folder).value && !is_symlink(folder).value)
        {
            if(snap.id != 0)
            {
                folder = snapshot::snapshot_path(snap.id);
                out.open(folder.c_str(), std::ios::binary);
                out<< snap;
                out.close();
                success = true;
            }
        }
        return success;
    }
    
    inline bool snapshot_header_eq(const snapshot::snapshot_data& s1, const snapshot::snapshot_data& s2)
    {
        if(s1.id == s2.id)
        {
            s1.root == s2.root;
        }
        s1.timestamp == s2.timestamp;
        s1.root == s2.root;
        return (
                    (s1.id == s2.id) && 
                    (s1.root == s2.root) && 
                    (s1.timestamp == s2.timestamp));
    }
    
    ss_hasht hash_snapshots(const std::vector<snapshot::snapshot_data>& snapshots)
    {
        ss_hasht temph;
        if(!snapshots.empty())
        {
            for(std::vector<snapshot::snapshot_data>::const_iterator it = snapshots.begin();
                            it != snapshots.end(); ++it)
            {
                if(temph.find(it->id) == temph.end()) temph[it->id] = (*it);
            }
        }
        return temph;
    }
    
    inline bool snapshot_list_eq(const std::vector<snapshot::snapshot_data>& s1, 
                    const std::vector<snapshot::snapshot_data>& s2)
    {
        bool tempb(false);
        ss_hasht hash(hash_snapshots(s2));
        
        if(s1.size() == s2.size())
        {
            tempb = true;
            for(std::vector<snapshot::snapshot_data>::const_iterator it = s1.begin(); 
                            ((it != s1.end()) && tempb); ++it)
            {
                tempb = false;
                if(hash.find(it->id) != hash.end())
                {
                    tempb = snapshot_header_eq(hash[it->id], *it);
                }
            }
        }
        else tempb = false;
        return tempb;
    }
    
    inline bool delete_folder(const std::string& s)
    {
        using fsys::is_folder;
        using fsys::is_symlink;
        using fsys::delete_iterator_class;
        
        fsys::result_data_boolean res;
        
        if(is_folder(s).value && !is_symlink(s).value)
        {
            for(delete_iterator_class it(s); !it.at_end(); ++it)
            {
                if(!it.err.value) it.skip();
            }
        }
        return (!is_folder(s).value && !is_symlink(s).value);
    }
    
    
}

using test::fixture_class;

TEST_FIXTURE(fixture_class, retrieve_information_test_case)
{
    using namespace test;
    
    std::vector<snapshot::snapshot_data> tempsnap;
    tempsnap = snapshot::list_snapshot_info(snapshot::snapshot_folder());
    CHECK(snapshot_list_eq(tempsnap, snaps));
}

