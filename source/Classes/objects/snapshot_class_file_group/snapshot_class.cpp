#include <string>
#include <vector>
#include <iostream>
#include <thread>
#include <ctime>
#include <fstream>

#include "snapshot_class.hpp"
#include "common.hpp"
#include "date_class.hpp"
#include "global_defines.hpp"
#include "snapshot_file_loaders.hpp"
#include "filesystem.hpp"

namespace
{
    
    /* Unifies and limits the scope of the data that is used between
     * the functions of collect_snapshot and show_process_output. */
    struct take_snapshot_proc_data
    {
        std::string root, save_file, current_path;
        bool finished : 1, canceled : 1, paused : 1;
        unsigned long long sid, count;
        std::ofstream out;
    };
    
    void construct_tsproc_data(take_snapshot_proc_data&, const std::string&);
    void collect_snapshot(take_snapshot_proc_data*);
    void show_process_output(take_snapshot_proc_data*);
    void display_current_status(take_snapshot_proc_data&);
    
    
    void collect_snapshot(take_snapshot_proc_data *pd)
    {
        using fsys::tree_riterator_class;
        
        for(tree_riterator_class it(pd->root); (!it.at_end() && !pd->canceled); ++it)
        {
            (pd->out)<< it.value()<< mem_delim::value;
            pd->current_path = it.value();
            pd->count++;
            while(pd->paused) std::this_thread::sleep_for(std::chrono::milliseconds(33));
        }
        (pd->out)<< struct_delim::value;
        pd->finished = true;
    }
    
    void show_process_output(take_snapshot_proc_data *pd)
    {
        common::cl();
        while(!pd->finished && !pd->canceled)
        {
            display_current_status(*pd);
            std::this_thread::sleep_for(std::chrono::milliseconds(33));
            if(common::kbhit())
            {
                pd->paused = true;
                pd->canceled = common::inp::is_sure("Cancel taking this snapshot??");
                pd->paused = false;
                if(!pd->canceled) common::cl();
            }
        }
    }
    
    void display_current_status(take_snapshot_proc_data& pd)
    {
        using std::cout;
        using std::endl;
        
        common::cls();
        cout<< "Press any key to cancel";
        for(unsigned int x = 0; x < 8; x++) cout<< endl;
        cout<< "root: \""<< pd.root<< "\""<< endl;
        cout<< "Paths saved: "<< pd.count<< endl;
        cout<< "Currently processing: \""<< pd.current_path<< "\""<< endl;
    }
    
    void construct_tsproc_data(take_snapshot_proc_data& pd, const std::string& rt)
    {
        snapshot::snapshot_data head;
        
        if(!fsys::is_folder(snapshot::snapshot_folder()))
        {
            if(!fsys::create_folder(snapshot::snapshot_folder()).value) return;
        }
        pd.root = rt;
        pd.sid = snapshot::new_snapshot_id();
        pd.save_file = snapshot::snapshot_path(pd.sid);
        pd.canceled = false;
        pd.finished = false;
        pd.paused = false;
        pd.count = 0;
        pd.current_path = "";
        
        head.take_time();
        head.root = pd.root;
        head.id = pd.sid;
        pd.out.open(pd.save_file.c_str(), std::ios::binary);
        snapshot::out_header(pd.out, head);
    }
    
    
}

/* stream operators: */
namespace snapshot
{
    std::ostream& operator<<(std::ostream& out, const snapshot_data& snap)
    {
        if(out.good())
        {
            out_header(out, snap);
            for(std::vector<std::string>::const_iterator it = snap.paths.begin(); 
                    it != snap.paths.end(); ++it)
            {
                out<< *it<< mem_delim::value;
            }
            out<< struct_delim::value;
        }
        return out;
    }
    
    /** Sets failbit only if it fails to retrieve the snapshot. */
    std::istream& operator>>(std::istream& in, snapshot_data& snap)
    {
        using common::safe_getline;
        
        std::string temps;
        
        snap = snapshot_data();
        
        //peek: this will establish that the stream is good.
        in.peek();
        
        if(in.good())
        {
            in_header(in, snap);
            while(in.good() && (in.peek() != struct_delim::value) && (in.peek() != EOF))
            {
                if(safe_getline(in, temps, mem_delim::value)) snap.paths.push_back(temps);
            }
            if(in.peek() == struct_delim::value)
            {
                in.get();
                
                /* Clear error state if we've reached the end of the file.  
                 * the read was a success, so the stream satte should reflect that.*/
                if(in.peek() == EOF) in.clear();
            }
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
            this->hour = snap.hour;
            this->minute = snap.minute;
        }
        return *this;
    }
    
    bool snapshot_data::operator==(const snapshot_data& snap) const
    {
        return (
                (this->paths == snap.paths) && 
                (this->id == snap.id) && 
                (this->root == snap.root) && 
                (this->timestamp == snap.timestamp) && 
                (this->hour == snap.hour) && 
                (this->minute == snap.minute) &&
                (this->second == snap.second));
    }
    
    bool snapshot_data::operator!=(const snapshot_data& snap) const
    {
        return !(this->operator==(snap));
    }
    
    /** Returns true if this snapshot was taken before snap.*/
    bool snapshot_data::operator<(const snapshot_data& s) const
    {
        bool lessthan(false);
        if(this->timestamp < s.timestamp)
        {
            lessthan = true;
        }
        else if(this->timestamp == s.timestamp)
        {
            if(this->hour < s.hour)
            {
                lessthan = true;
            }
            else if(this->hour == s.hour)
            {
                if(this->minute < s.minute)
                {
                    lessthan = true;
                }
                else if(this->minute == s.minute)
                {
                    if(this->second < s.second) lessthan = true;
                }
            }
        }
        return lessthan;
    }
    
    void snapshot_data::take_time()
    {
        struct tm t(common::get_time());
        this->timestamp = t;
        this->hour = t.tm_hour;
        this->minute = t.tm_min;
        this->second = t.tm_sec;
    }
    
    
}

/* take_snapshot member functions: */
namespace snapshot
{
    unsigned long long take_snapshot(const std::string& s)
    {
        take_snapshot_proc_data *pd(new take_snapshot_proc_data);
        unsigned long long id(0);
        
        construct_tsproc_data(*pd, s);
        
        std::thread collect(collect_snapshot, pd), output(show_process_output, pd);
        
        collect.join();
        output.join();
        if(pd->out.is_open()) pd->out.close();
        if(pd->canceled)
        {
            if(fsys::is_file(pd->save_file).value && 
                            !fsys::is_symlink(pd->save_file).value)
            {
                if(fsys::can_delete(pd->save_file))
                {
                    fsys::fdelete(pd->save_file);
                    pd->sid = 0;
                }
            }
        }
        id = pd->sid;
        delete pd;
        return id;
    }
    
    std::ostream& out_header(std::ostream& out, const snapshot_data& snap)
    {
        if(out.good())
        {
            out<< snap.timestamp;
            out<< snap.hour<< mem_delim::value;
            out<< snap.minute<< mem_delim::value;
            out<< snap.second<< mem_delim::value;
            out<< snap.id<< mem_delim::value;
            out<< snap.root<< mem_delim::value;
        }
        return out;
    }
    
    std::istream& in_header(std::istream& in, snapshot_data& snap)
    {
        using common::safe_getline;
        
        in.peek();
        if(in.good())
        {
            in>> snap.timestamp;
            if(safe_getline(in, snap.hour, mem_delim::value))
            {
                if(safe_getline(in, snap.minute, mem_delim::value))
                {
                    if(safe_getline(in, snap.second, mem_delim::value))
                    {
                        if(safe_getline(in, snap.id, mem_delim::value))
                        {
                            safe_getline(in, snap.root, mem_delim::value);
                        }
                    }
                }
            }
        }
        in.peek();
        return in;
    }
    
    
}

