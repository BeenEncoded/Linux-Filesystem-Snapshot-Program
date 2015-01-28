#include <unittest++/UnitTest++.h>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <exception>
#include <stdexcept>
#include <unordered_map>
#include <utility>

#include "snapshot_class.hpp"
#include "snapshot_file_loader.hpp"
#include "random_junk_data.hpp"
#include "filesystem.hpp"
#include "global_defines.hpp"
#include "program_settings.hpp"

namespace test
{
    typedef std::unordered_map<unsigned long long, snapshot::snapshot_data> ss_hasht;
    
    typedef class fixture_class fixture_class;
    
    bool save_snapshot(const snapshot::snapshot_data&);
    bool snapshot_header_eq(const snapshot::snapshot_data&, const snapshot::snapshot_data&);
    bool delete_folder(const std::string&);
    bool snapshot_list_eq(const std::vector<snapshot::snapshot_data>&, 
                    const std::vector<snapshot::snapshot_data>&);
    ss_hasht hash_snapshots(const std::vector<snapshot::snapshot_data>&);
    std::vector<snapshot::snapshot_data> load_all_headers(const std::string&);
    
    
    typedef class fixture_class
    {
    public:
        fixture_class()
        {
            using std::cout;
            using std::endl;
            
            bool tempb(true);
            snapshot::snapshot_data tempsnap;
            
            for(unsigned int x = 0; ((x < 1) && tempb); x++)
            {
                tempsnap = rand_data::random_snapshot();
                tempsnap.id = snapshot::new_snapshot_id();
                tempb = save_snapshot(tempsnap);
                if(tempb) this->snaps.push_back(tempsnap);
                else 
                {
                    cout<< "Error, failed to set the test up because \
save_snasphot failed!"<< endl;
                    throw std::runtime_error("Test invalid!");
                }
            }
        }
        
        ~fixture_class()
        {
            delete_folder(snapshot::snapshot_folder());
        }
        
        
    protected:
        std::string filename;
        std::vector<snapshot::snapshot_data> snaps;
        
    } fixture_class;
    
    inline bool save_snapshot(const snapshot::snapshot_data& snap)
    {
        using fsys::is_folder;
        using fsys::is_symlink;
        
        std::string folder(settings::settings_data().global.snapshot_folder);
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
                folder = (settings::settings_data().global.snapshot_folder + fsys::pref_slash() + 
                                std::to_string(snap.id) + fsyssnap_SNAPSHOT_FILE_EXTENSION);
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
                if(!it.success.value) 
                {
                    std::cout<< "Error: \""<< it.success.error<< "\""<< std::endl;
                    it.skip();
                }
            }
        }
        return (!is_folder(s).value && !is_symlink(s).value);
    }
    
    inline std::vector<snapshot::snapshot_data> load_all_headers(const std::string& folder)
    {
        std::unordered_map<unsigned long long, std::string> ids;
        std::vector<snapshot::snapshot_data> snaps;
        
        if(fsys::is_folder(folder).value && !fsys::is_symlink(folder).value)
        {
            ids = snapshot::list_ids(folder);
            for(std::unordered_map<unsigned long long, std::string>::const_iterator it = ids.begin(); 
                            it != ids.end(); ++it)
            {
                snaps.push_back(snapshot::snapshot_data());
                if(!snapshot::load_header(snaps.back(), it->second))
                {
                    snaps.pop_back();
                }
            }
        }
        return snaps;
    }
    
    
}

using test::fixture_class;

TEST_FIXTURE(fixture_class, retrieve_information_test_case)
{
    using namespace test;
    
    std::vector<snapshot::snapshot_data> tempsnap;
    tempsnap = load_all_headers(settings::settings_data().global.snapshot_folder);
    CHECK(snapshot_list_eq(tempsnap, snaps));
}

