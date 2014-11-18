#include <string>
#include <vector>
#include <iostream>
#include <thread>
#include <ctime>
#include <fstream>
#include <cstring>

#include "snapshot_class.hpp"
#include "common.hpp"
#include "date_class.hpp"
#include "global_defines.hpp"
#include "snapshot_file_loaders.hpp"
#include "filesystem.hpp"
#include "time_class.hpp"

namespace
{
    typedef const_int_type<75> max_path_size;
    
    typedef struct take_snapshot_proc_data take_snapshot_proc_data;
    
    void construct_tsproc_data(take_snapshot_proc_data&, const std::string&);
    void collect_snapshot(take_snapshot_proc_data*);
    void show_process_output(take_snapshot_proc_data*);
    void display_current_status(take_snapshot_proc_data*);
    std::string filename(const std::string&) noexcept;
    std::string format_current_path(const std::string&, const std::size_t&) noexcept;
    
    
    /* Unifies and limits the scope of the data that is used between
     * the functions of collect_snapshot and show_process_output. */
    typedef struct take_snapshot_proc_data
    {
        std::string root, save_file;
        char *current_path;
        bool finished : 1, canceled : 1, paused : 1;
        unsigned long long sid, count;
        std::ofstream out;
    } take_snapshot_proc_data;
    
    void collect_snapshot(take_snapshot_proc_data *pd)
    {
        using fsys::tree_riterator_class;
        
        if(pd == nullptr) ethrow("pd is null!");
        
        for(tree_riterator_class it(pd->root); (!it.at_end() && !pd->canceled); ++it)
        {
            (pd->out)<< it.value()<< mem_delim::value;
            
            std::strcpy(pd->current_path, std::string(max_path_size::value, '\0').c_str());
            std::strcpy(pd->current_path, format_current_path((it.value() + '\0'), max_path_size::value).c_str());
            
            pd->count++;
            while(pd->paused) std::this_thread::sleep_for(std::chrono::milliseconds(33));
        }
        (pd->out)<< struct_delim::value;
        pd->finished = true;
    }
    
    void show_process_output(take_snapshot_proc_data *pd)
    {
        if(pd == nullptr) ethrow("pd is null!");
        common::cl();
        while(!pd->finished && !pd->canceled)
        {
            display_current_status(pd);
            std::this_thread::sleep_for(std::chrono::milliseconds(33));
            if(common::kbhit())
            {
                pd->paused = true;
                pd->canceled = common::inp::is_sure("Cancel taking this snapshot??");
                pd->paused = false;
                if(!pd->canceled) common::cl();
            }
        }
        if(!pd->canceled)
        {
            using std::cout;
            using std::endl;
            
            common::cls();
            for(unsigned int x = 0; x < v_center::value; x++) cout<< endl;
            common::center("Paths Captured: " + std::to_string(pd->count));
            common::wait();
            common::cls();
            cout<< "running...";
            cout.flush();
        }
    }
    
    inline void display_current_status(take_snapshot_proc_data *pd)
    {
        using std::cout;
        using std::endl;
        if(pd == nullptr) ethrow("pd is null!");
        
        if(pd != nullptr)
        {
            common::cls();
            cout<< "Press any key to cancel";
            for(unsigned int x = 0; x < 8; x++) cout<< endl;
            cout<< "root: \""<< pd->root<< "\""<< endl;
            cout<< "Paths saved: "<< pd->count<< endl;
            cout<< "Currently processing: \""<< pd->current_path<< "\""<< endl;
        }
        else ethrow("WTF, why is pd == nullptr?!?!");
    }
    
    /** Initializes all the data for the snapshot collection.  It opens a file
     * and saves the header to a "new" snapshot, and it should be used before 
     * executing a new snapshot. */
    inline void construct_tsproc_data(take_snapshot_proc_data& pd, const std::string& rt)
    {
        snapshot::snapshot_data head;
        
        if(!fsys::is_folder(snapshot::snapshot_folder()))
        {
            if(!fsys::create_folder(snapshot::snapshot_folder()).value) return;
        }
        pd.root = rt;
        pd.sid = snapshot::new_snapshot_id();
        pd.save_file = (snapshot::snapshot_folder() + fsys::pref_slash() + 
                        std::to_string(pd.sid) + fsyssnap_SNAPSHOT_FILE_EXTENSION);
        pd.canceled = false;
        pd.finished = false;
        pd.paused = false;
        pd.count = 0;
        pd.current_path = new char[max_path_size::value];
        
        //blank the new string:
        std::strcpy(pd.current_path, std::string(max_path_size::value, '\0').c_str());
        
        head.timestamp = tdata::current_time();
        head.root = pd.root;
        head.id = pd.sid;
        pd.out.open(pd.save_file.c_str(), std::ios::binary);
        if(!pd.out.good()) ethrow(std::string("Error, out is not good!  File: \"" + 
                        pd.save_file + "\""));
        snapshot::out_header(pd.out, head);
    }
    
    inline std::string filename(const std::string& s) noexcept
    {
        std::string temps(s);
        std::size_t pos(temps.rfind(fsys::pref_slash()));
        
        if(pos != std::string::npos)
        {
            temps.erase(temps.begin(), (temps.begin() + pos + 1));
        }
        return temps;
    }
    
    inline std::string format_current_path(const std::string& s, const std::size_t& size) noexcept
    {
        std::string temps(s), newend("/.../" + filename(s));
        
        if(!temps.empty())
        {
            if(filename(temps).size() < size)
            {
                if(temps.size() > size)
                {
                    temps = common::parent_folder(temps);
                    while(((temps.size() + newend.size()) > size) && !temps.empty() && 
                                    (temps != std::to_string(fsys::pref_slash())))
                    {
                        temps = common::parent_folder(temps);
                    }
                    temps += newend;
                }
            }
            else temps = filename(temps);
        }
        
        if(temps.size() > size) temps.erase(temps.begin(), (temps.begin() + (temps.size() - size)));
        return temps;
    }
    
    
}

/* stream operators: */
namespace snapshot
{
    std::ostream& operator<<(std::ostream& out, const snapshot_data& snap) noexcept
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
    std::istream& operator>>(std::istream& in, snapshot_data& snap) noexcept
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
    
    const snapshot_data& snapshot_data::operator=(const snapshot_data& snap) noexcept
    {
        if(this != &snap)
        {
            this->timestamp = snap.timestamp;
            this->root = snap.root;
            this->id = snap.id;
            
            this->paths.clear();
            this->paths.shrink_to_fit();
            this->paths = snap.paths;
        }
        return *this;
    }
    
    bool snapshot_data::operator==(const snapshot_data& snap) const noexcept
    {
        return (
                (this->id == snap.id) && 
                (this->timestamp == snap.timestamp) && 
                (this->root == snap.root) && 
                (this->paths == snap.paths));
    }
    
    bool snapshot_data::operator!=(const snapshot_data& snap) const noexcept
    {
        return !(this->operator==(snap));
    }
    
    /** Compares two snapshot's times.  provided for sorting algorithms. */
    bool snapshot_data::operator<(const snapshot_data& s) const noexcept
    {
        return (this->timestamp < s.timestamp);
    }
    
    
}

/* take_snapshot member functions: */
namespace snapshot
{
    unsigned long long take_snapshot(const std::string& s) noexcept
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
        delete[] pd->current_path;
        pd->current_path = nullptr;
        delete pd;
        pd = nullptr;
        return id;
    }
    
    std::ostream& out_header(std::ostream& out, const snapshot_data& snap) noexcept
    {
        char *ch(new char[sizeof(unsigned long long)]);
        
        if(out.good())
        {
            out<< snap.timestamp;
            memcpy(ch, &snap.id, sizeof(unsigned long long));
            for(unsigned int x = 0; x < sizeof(unsigned long long); x++) out<< ch[x];
            out<< snap.root<< mem_delim::value;
        }
        delete[] ch;
        return out;
    }
    
    std::istream& in_header(std::istream& in, snapshot_data& snap) noexcept
    {
        using common::safe_getline;
        
        char *ch(new char[sizeof(unsigned long long)]);
        
        in.peek();
        if(in.good())
        {
            in>> snap.timestamp;
            for(unsigned int x = 0; ((x < sizeof(unsigned long long)) && in.good()); x++)
            {
                if(in.peek() != EOF) ch[x] = in.get();
            }
            memcpy(&snap.id, ch, sizeof(unsigned long long));
            safe_getline(in, snap.root, mem_delim::value);
        }
        in.peek();
        return in;
    }
    
    
}

