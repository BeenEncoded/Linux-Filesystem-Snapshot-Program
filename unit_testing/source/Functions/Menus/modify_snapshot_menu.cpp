#include <string>
#include <vector>
#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <stdexcept>
#include <algorithm>
#include <fstream>
#include <utility>
#include <cstring>
#include <regex>

#include "common.hpp"
#include "global_defines.hpp"
#include "snapshot_class.hpp"
#include "snapshot_file_loader.hpp"
#include "common_menu.hpp"
#include "time_class.hpp"
#include "filesystem.hpp"
#include "program_settings.hpp"
#include "modify_snapshot_menu.hpp"


namespace
{
    typedef struct menu_data menu_data;
    
    bool remove_snapshot(const menu_data&, const unsigned long long&);
    std::string display_time(const tdata::time_class&);
    bool create_folder(const std::string&);
    void load_all_headers(menu_data&);
    std::vector<std::string> difference_between(const std::unordered_set<std::string>&, 
                    const std::unordered_set<std::string>&);
    std::unordered_set<std::string> load_paths(const std::string&);
    bool compare_snapshots(const snapshot::snapshot_data&, const snapshot::snapshot_data&);
    void man_regex();
    void snapshot_display(const std::vector<snapshot::snapshot_data>&, std::vector<std::string>&);
    bool match_settings(const std::string&, const std::pair<std::vector<settings::regex_data>, std::vector<settings::regex_data> >&);
    
    
    
    /** Data that this menu, and most functions is calls, needs 
     * for a lot of operations.*/
    typedef struct menu_data
    {
        std::unordered_map<unsigned long long, std::string> ids;
        std::vector<snapshot::snapshot_data> snapshots;
        settings::settings_data *settings;
    } menu_data;
    
    /** shows the official gnu man page on regular expressions: */
    __attribute__((unused)) inline void man_regex()
    {
        using std::cout;
        using std::endl;
        common::cls();
        
        /* I took this straight from the GNU man pages.  Should give the user a good
         * idea of what regular expressions are for. */
        
        std::cout<< "REGEX(7)                                                  Linux Programmer's Manual                                                  REGEX(7)"<< endl;
        std::cout<< endl;
        std::cout<< endl;
        std::cout<< endl;
        std::cout<< "NAME"<< endl;
        std::cout<< "       regex - POSIX.2 regular expressions"<< endl;
        std::cout<< endl;
        std::cout<< "DESCRIPTION"<< endl;
        std::cout<< "       Regular  expressions  (\"RE\"s),  as  defined  in  POSIX.2,  come  in two forms: modern REs (roughly those of egrep; POSIX.2 calls these"<< endl;
        std::cout<< "       \"extended\" REs) and obsolete REs (roughly those of ed(1); POSIX.2 \"basic\" REs).  Obsolete REs mostly exist for backward  compatibility"<< endl;
        std::cout<< "       in  some  old  programs;  they will be discussed at the end.  POSIX.2 leaves some aspects of RE syntax and semantics open; \"(!)\" marks"<< endl;
        std::cout<< "       decisions on these aspects that may not be fully portable to other POSIX.2 implementations."<< endl;
        std::cout<< endl;
        std::cout<< "       A (modern) RE is one(!) or more nonempty(!) branches, separated by '|'.  It matches anything that matches one of the branches."<< endl;
        std::cout<< endl;
        std::cout<< "       A branch is one(!) or more pieces, concatenated.  It matches a match for the first, followed by a match for the second, and so on."<< endl;
        std::cout<< endl;
        std::cout<< "       A piece is an atom possibly followed by a single(!) \'*\', \'+\', \'?\', or bound.  An atom followed by \'*\' matches a sequence of 0 or  more"<< endl;
        std::cout<< "       matches  of the atom.  An atom followed by \'+\' matches a sequence of 1 or more matches of the atom.  An atom followed by \'?\' matches a"<< endl;
        std::cout<< "       sequence of 0 or 1 matches of the atom."<< endl;
        std::cout<< endl;
        std::cout<< "       A bound is \'{\' followed by an unsigned decimal integer, possibly followed by \',\' possibly followed by another unsigned  decimal  inte‐"<< endl;
        std::cout<< "       ger,  always  followed  by  \'}\'.  The integers must lie between 0 and RE_DUP_MAX (255(!)) inclusive, and if there are two of them, the"<< endl;
        std::cout<< "       first may not exceed the second.  An atom followed by a bound containing one integer i and no comma matches a sequence  of  exactly  i"<< endl;
        std::cout<< "       matches  of the atom.  An atom followed by a bound containing one integer i and a comma matches a sequence of i or more matches of the"<< endl;
        std::cout<< "       atom.  An atom followed by a bound containing two integers i and j matches a sequence of i through j (inclusive) matches of the atom."<< endl;
        std::cout<< endl;
        std::cout<< "       An atom is a regular expression enclosed in \"()\" (matching a match for the regular expression), an empty set  of  \"()\"  (matching  the"<< endl;
        std::cout<< "       null string)(!), a bracket expression (see below), \'.\' (matching any single character), \'^\' (matching the null string at the beginning"<< endl;
        std::cout<< "       of a line), \'$\' (matching the null string at the end of a line), a \'\\\' followed by one of the characters \"^.[$()|*+?{\\\" (matching that"<< endl;
        std::cout<< "       character  taken  as  an  ordinary character), a \'\\\' followed by any other character(!)  (matching that character taken as an ordinary"<< endl;
        std::cout<< "       character, as if the \'\\\' had not been present(!)), or a single character with no other significance (matching that character).  A  \'{\'"<< endl;
        std::cout<< "       followed by a character other than a digit is an ordinary character, not the beginning of a bound(!).  It is illegal to end an RE with"<< endl;
        std::cout<< "       \'\\\'."<< endl;
        std::cout<< endl;
        std::cout<< "       A bracket expression is a list of characters enclosed in \"[]\".  It normally matches any  single  character  from  the  list  (but  see"<< endl;
        std::cout<< "       below).   If  the list begins with \'^\', it matches any single character (but see below) not from the rest of the list.  If two charac‐"<< endl;
        std::cout<< "       ters in the list are separated by \'-\', this is shorthand for the full range of characters between those two (inclusive) in the collat‐"<< endl;
        std::cout<< "       ing  sequence,  for  example,  \"[0-9]\"  in ASCII matches any decimal digit.  It is illegal(!) for two ranges to share an endpoint, for"<< endl;
        std::cout<< "       example, \"a-c-e\".  Ranges are very collating-sequence-dependent, and portable programs should avoid relying on them."<< endl;
        std::cout<< endl;
        std::cout<< "       To include a literal \']\' in the list, make it the first character (following a possible \'^\').  To include a literal \'-\', make  it  the"<< endl;
        std::cout<< "       first or last character, or the second endpoint of a range.  To use a literal \'-\' as the first endpoint of a range, enclose it in \"[.\""<< endl;
        std::cout<< "       and \".]\"  to make it a collating element (see below).  With the exception of these and some combinations using  \'[\'  (see  next  para‐"<< endl;
        std::cout<< "       graphs), all other special characters, including \'\\\', lose their special significance within a bracket expression."<< endl;
        std::cout<< endl;
        std::cout<< "       Within  a bracket expression, a collating element (a character, a multicharacter sequence that collates as if it were a single charac‐"<< endl;
        std::cout<< "       ter, or a collating-sequence name for either) enclosed in \"[.\" and \".]\" stands for the sequence of characters of that  collating  ele‐"<< endl;
        std::cout<< "       ment.   The sequence is a single element of the bracket expression's list.  A bracket expression containing a multicharacter collating"<< endl;
        std::cout<< "       element can thus match more than one character, for example, if the collating sequence includes a \"ch\" collating element, then the  RE"<< endl;
        std::cout<< "       \"[[.ch.]]*c\" matches the first five characters of \"chchcc\"."<< endl;
        std::cout<< endl;
        std::cout<< "       Within  a  bracket  expression,  a  collating element enclosed in \"[=\" and \"=]\" is an equivalence class, standing for the sequences of"<< endl;
        std::cout<< "       characters of all collating elements equivalent to that one, including itself.  (If there are no other equivalent collating  elements,"<< endl;
        std::cout<< "       the treatment is as if the enclosing delimiters were \"[.\" and \".]\".)  For example, if o and ^ are the members of an equivalence class,"<< endl;
        std::cout<< "       then \"[[=o=]]\", \"[[=^=]]\", and \"[o^]\" are all synonymous.  An equivalence class may not(!) be an endpoint of a range."<< endl;
        std::cout<< endl;
        std::cout<< "       Within a bracket expression, the name of a character class enclosed in \"[:\" and \":]\" stands for the list of all  characters  belonging"<< endl;
        std::cout<< "       to that class.  Standard character class names are:"<< endl;
        std::cout<< endl;
        std::cout<< "              alnum   digit   punct"<< endl;
        std::cout<< "              alpha   graph   space"<< endl;
        std::cout<< "              blank   lower   upper"<< endl;
        std::cout<< "              cntrl   print   xdigit"<< endl;
        std::cout<< endl;
        std::cout<< "       These  stand  for  the  character classes defined in wctype(3).  A locale may provide others.  A character class may not be used as an"<< endl;
        std::cout<< "       endpoint of a range."<< endl;
        std::cout<< endl;
        std::cout<< "       In the event that an RE could match more than one substring of a given string, the RE matches the one starting earliest in the string."<< endl;
        std::cout<< "       If  the  RE could match more than one substring starting at that point, it matches the longest.  Subexpressions also match the longest"<< endl;
        std::cout<< "       possible substrings, subject to the constraint that the whole match be as long as possible, with subexpressions  starting  earlier  in"<< endl;
        std::cout<< "       the RE taking priority over ones starting later.  Note that higher-level subexpressions thus take priority over their lower-level com‐"<< endl;
        std::cout<< "       ponent subexpressions."<< endl;
        std::cout<< endl;
        std::cout<< "       Match lengths are measured in characters, not collating elements.  A null string is considered longer than no match at all.  For exam‐"<< endl;
        std::cout<< "       ple,  \"bb*\"  matches  the three middle characters of \"abbbc\", \"(wee|week)(knights|nights)\" matches all ten characters of \"weeknights\","<< endl;
        std::cout<< "       when \"(.*).*\" is matched against \"abc\" the parenthesized subexpression matches all three  characters,  and  when  \"(a*)*\"  is  matched"<< endl;
        std::cout<< "       against \"bc\" both the whole RE and the parenthesized subexpression match the null string."<< endl;
        std::cout<< endl;
        std::cout<< "       If  case-independent  matching  is  specified, the effect is much as if all case distinctions had vanished from the alphabet.  When an"<< endl;
        std::cout<< "       alphabetic that exists in multiple cases appears as an ordinary character outside a bracket expression, it is effectively  transformed"<< endl;
        std::cout<< "       into  a  bracket  expression containing both cases, for example, \'x\' becomes \"[xX]\".  When it appears inside a bracket expression, all"<< endl;
        std::cout<< "       case counterparts of it are added to the bracket expression, so that, for example, \"[x]\" becomes \"[xX]\" and \"[^x]\" becomes \"[^xX]\"."<< endl;
        std::cout<< endl;
        std::cout<< "       No particular limit is imposed on the length of REs(!).  Programs intended to be portable should not employ REs longer than 256 bytes,"<< endl;
        std::cout<< "       as an implementation can refuse to accept such REs and remain POSIX-compliant."<< endl;
        std::cout<< endl;
        std::cout<< "       Obsolete  (\"basic\") regular expressions differ in several respects.  \'|\', \'+\', and \'?\' are ordinary characters and there is no equiva‐"<< endl;
        std::cout<< "       lent for their functionality.  The delimiters for bounds are \"\\{\" and \"\\}\", with \'{\' and \'}\' by themselves ordinary  characters.   The"<< endl;
        std::cout<< "       parentheses for nested subexpressions are \"\\(\" and \"\\)\", with \'(\' and \')\' by themselves ordinary characters.  \'^\' is an ordinary char‐"<< endl;
        std::cout<< "       acter except at the beginning of the RE or(!) the beginning of a parenthesized subexpression, \'$\' is an ordinary character  except  at"<< endl;
        std::cout<< "       the  end  of the RE or(!) the end of a parenthesized subexpression, and '*' is an ordinary character if it appears at the beginning of"<< endl;
        std::cout<< "       the RE or the beginning of a parenthesized subexpression (after a possible leading \'^\')."<< endl;
        std::cout<< endl;
        std::cout<< "       Finally, there is one new type of atom, a back reference: \'\\\' followed by a nonzero decimal digit d matches the same sequence of char‐"<< endl;
        std::cout<< "       acters matched by the dth parenthesized subexpression (numbering subexpressions by the positions of their opening parentheses, left to"<< endl;
        std::cout<< "       right), so that, for example, \"\\([bc]\\)\\1\" matches \"bb\" or \"cc\" but not \"bc\"."<< endl;
        std::cout<< endl;
        std::cout<< "BUGS"<< endl;
        std::cout<< "       Having two kinds of REs is a botch."<< endl;
        std::cout<< endl;
        std::cout<< "       The current POSIX.2 spec says that \')\' is an ordinary character in the absence of an unmatched \'(\'; this was an  unintentional  result"<< endl;
        std::cout<< "       of a wording error, and change is likely.  Avoid relying on it."<< endl;
        std::cout<< endl;
        std::cout<< "       Back  references  are  a  dreadful botch, posing major problems for efficient implementations.  They are also somewhat vaguely defined"<< endl;
        std::cout<< "       (does \"a\\(\\(b\\)*\\2\\)*d\" match \"abbbd\"?).  Avoid using them."<< endl;
        std::cout<< endl;
        std::cout<< "       POSIX.2's specification of case-independent matching is vague.  The \"one case implies all cases\" definition  given  above  is  current"<< endl;
        std::cout<< "       consensus among implementors as to the right interpretation."<< endl;
        std::cout<< endl;
        std::cout<< "AUTHOR"<< endl;
        std::cout<< "       This page was taken from Henry Spencer's regex package."<< endl;
        std::cout<< endl;
        std::cout<< "SEE ALSO"<< endl;
        std::cout<< "       grep(1), regex(3)"<< endl;
        std::cout<< endl;
        std::cout<< "       POSIX.2, section 2.8 (Regular Expression Notation)."<< endl;
        std::cout<< endl;
        std::cout<< "COLOPHON"<< endl;
        std::cout<< "       This page is part of release 3.74 of the Linux man-pages project.  A description of the project, information about reporting bugs, and"<< endl;
        std::cout<< "       the latest version of this page, can be found at http://www.kernel.org/doc/man-pages/."<< endl;
        std::cout<< endl;
        std::cout<< endl;
        std::cout<< endl;
        std::cout<< "                                                                  2009-01-12                                                         REGEX(7)"<< endl;
    }
    
    /** Removes a snapshot's file. */
    inline bool remove_snapshot(const menu_data& data, const unsigned long long& id)
    {
        using fsys::is_file;
        using fsys::is_folder;
        using fsys::is_symlink;
        using fsys::can_delete;
        using fsys::fdelete;
        
        std::string const *snapshot_folder(&data.settings->global.snapshot_folder);
        std::unordered_map<unsigned long long, std::string>::const_iterator pos(data.ids.find(id));
        
        if(is_folder(*snapshot_folder).value && !is_symlink(*snapshot_folder).value && 
                        (pos != data.ids.end()))
        {
            if(is_file(pos->second).value)
            {
                if(can_delete(pos->second))
                {
                    return fdelete(pos->second).value;
                }
            }
        }
        return false;
    }
    
    /** Returns a string representation of time */
    inline std::string display_time(const tdata::time_class& t)
    {
        std::string temps;
        
        temps = (t.wday_name() + " " + t.month_name() + " " + 
                        std::to_string(t.mday()) + ", " + 
                        std::to_string(t.gyear()) + " at " + 
                        std::to_string(t.hour()) + ":" + std::to_string(t.minute()) + 
                        "." + std::to_string(t.second()) + std::string(" ") + 
                        (t.am() ? "AM" : "PM"));
        return temps;
    }

    inline bool create_folder(const std::string& folder) //test
    {
        using fsys::is_folder;
        using fsys::is_file;
        using fsys::is_symlink;
        
        if(!is_folder(folder).value && !is_file(folder).value && !is_symlink(folder).value)
        {
            fsys::create_folder(folder);
        }
        return (is_folder(folder).value && !is_symlink(folder).value);
    }
    
    /** Returns a vector of strings containing the elements found in bef, but not in aft. */
    inline std::vector<std::string> difference_between(const std::unordered_set<std::string>& bef, 
                    const std::unordered_set<std::string>& aft)
    {
        std::vector<std::string> diff;
        for(std::unordered_set<std::string>::const_iterator it = bef.begin(); it != bef.end(); ++it)
        {
            if(aft.find(*it) == aft.end()) diff.push_back(*it);
        }
        return diff;
    }
    
    /** loads all the paths of a snapshot */
    inline std::unordered_set<std::string> load_paths(const std::string& file)
    {
        using fsys::is_file;
        using fsys::is_symlink;
        
        std::unordered_set<std::string> paths;
        std::string temps;
        std::ifstream in;
        
        if(is_file(file).value && !is_symlink(file).value)
        {
            in.open(file.c_str(), std::ios::binary);
            //limit the scope while we skip the header:
            {
                snapshot::snapshot_data tempsnap;
                snapshot::in_header(in, tempsnap);
            }
            in.peek();
            while(in.good() && (in.peek() != struct_delim::value) && (in.peek() != EOF))
            {
                if(common::safe_getline(in, temps, mem_delim::value))
                {
                    paths.insert(temps);
                }
            }
            in.close();
        }
        return paths;
    }
    
    inline bool match_settings(const std::string& s, 
            const std::pair<std::vector<settings::regex_data>, std::vector<settings::regex_data> >& filter)
    {
        using settings::regex_data;
        
        if(filter.first.empty() && filter.second.empty()) return true;
        bool matched(filter.first.empty());
        for(std::vector<regex_data>::const_iterator it = filter.first.begin(); 
                ((it != filter.first.end()) && !matched); ++it)
        {
            matched = std::regex_search(s, std::regex(it->s));
        }
        for(std::vector<regex_data>::const_iterator it = filter.second.begin(); 
                ((it != filter.second.end()) && matched); ++it)
        {
            matched = !std::regex_search(s, std::regex(it->s));
        }
        return matched;
    }
    
    /** Loads all headers, as well as ids, into menu_data. This function also
     * sorts the snapshots. */
    inline void load_all_headers(menu_data& data)
    {
        using fsys::is_folder;
        using fsys::is_symlink;
        
        std::string const *folder(&data.settings->global.snapshot_folder);
        
        data.snapshots.erase(data.snapshots.begin(), data.snapshots.end());
        data.snapshots.shrink_to_fit();
        
        data.ids.erase(data.ids.begin(), data.ids.end());
        
        if(is_folder(*folder).value && !is_symlink(*folder).value)
        {
            data.ids = snapshot::list_ids(*folder);
            if(!data.ids.empty())
            {
                for(std::unordered_map<unsigned long long, std::string>::const_iterator it = data.ids.begin(); 
                                it != data.ids.end(); ++it)
                {
                    data.snapshots.push_back(snapshot::snapshot_data());
                    if(!snapshot::load_header(data.snapshots.back(), it->second)) data.snapshots.pop_back();
                }
            }
        }
        if(!data.snapshots.empty())
        {
            std::sort(data.snapshots.begin(), data.snapshots.end(), compare_snapshots);
        }
    }
    
    /** function used to sort the snapshots. */
    inline bool compare_snapshots(const snapshot::snapshot_data& snap1, 
                    const snapshot::snapshot_data& snap2)
    {
        return (snap1.timestamp > snap2.timestamp);
    }
    
    /** Used to  create a string version of the snapshot.  This is to be used in
     * menus.  */
    inline void snapshot_display(const std::vector<snapshot::snapshot_data>& snaps, std::vector<std::string>& disp)
    {
        unsigned int size(snaps.size());
        
        disp.clear();
        for(unsigned int x = 0; x < size; ++x) disp.push_back(display_time(snaps[x].timestamp));
    }
    
    namespace snapshot_diff_algo
    {
        void diff_snapshots(const menu_data&, const snapshot::snapshot_data&, const snapshot::snapshot_data&);
        std::string record_filename();
        
        
        
        /** returns a string representing the filename of a record file.  
         * A record file name consists of the time and date that the file
         * was created. */
        inline std::string record_filename()
        {
            tdata::time_class now(tdata::current_time());
            return (now.month_name() + " " + std::to_string(now.mday()) + 
                                ", " + std::to_string(now.gyear()) + "  at " + 
                                std::to_string(now.hour()) + " " + 
                                std::to_string(now.minute()) + " " + 
                                std::to_string(now.second()) + ".txt");
        }
        
        inline void save_diff_result(const menu_data& data, const std::vector<std::string>& paths, 
                std::ofstream& out, const std::string& title)
        {
            using std::endl;
            using settings::regex_data;
            
            std::pair<std::vector<regex_data>, std::vector<regex_data> > filter(settings::used_expressions(*(data.settings)));
            
            out<< title<< endl<< endl;
            for(std::vector<std::string>::const_iterator it = paths.begin(); it != paths.end(); ++it)
            {
                if(match_settings(*it, filter)) out<< *it<< endl;
            }
        }
        
        /** Finds the created and deleted paths between two snapshots. */
        inline void diff_snapshots(const menu_data& data, const snapshot::snapshot_data& snap1, 
                        const snapshot::snapshot_data& snap2)
        {
            using std::cout;
            using std::endl;
            using fsys::is_file;
            using fsys::is_symlink;
            using snapshot::load_header;
            using snapshot::snapshot_data;
            
            common::cls();
            for(unsigned int x = 0; x < v_center::value; x++) cout<< endl;
            common::center("Comparing, please wait...");
            cout.flush();
            
            std::unordered_set<std::string> paths_before, paths_after;
            std::string temps;
            snapshot::snapshot_data const *before(((snap1.timestamp < snap2.timestamp) ? &snap2 : &snap1)), 
                    *after(((snap1.timestamp < snap2.timestamp) ? &snap1 : &snap2));
            std::ofstream out;
            
            if((data.ids.find(before->id) != data.ids.end()) && 
                            (data.ids.find(after->id) != data.ids.end()) && 
                            create_folder(data.settings->global.records_folder))
            {
                
                paths_before = load_paths(data.ids.find(before->id)->second);
                paths_after = load_paths(data.ids.find(after->id)->second);
                
                temps = (data.settings->global.records_folder + fsys::pref_slash() + record_filename());
                
                out.open(temps.c_str(), std::ios::out);
                
                if(!out.good()) ethrow(("Out is not good! (file: \"" + temps + "\")"));
                
                out<< "Time frame:  "<< display_time(before->timestamp)<< " - "<< 
                                display_time(after->timestamp)<< endl;
                
                save_diff_result(data, difference_between(paths_before, paths_after), 
                                out, (std::string(5, '\n') + "DELETED PATHS: "));
                                
                save_diff_result(data, difference_between(paths_after, paths_before), 
                                out, (std::string(5, '\n') + "CREATED PATHS: "));
                                
                paths_before.erase(paths_before.begin(), paths_before.end());
                paths_after.erase(paths_after.begin(), paths_after.end());
                if(out.is_open()) out.close();
            }
            else ethrow("Can not use snapshots that are not loaded to diff!");
            
            if(is_file(temps).value && !is_symlink(temps).value)
            {
                common::cls();
                for(unsigned int x = 0; x < v_center::value; x++) cout<< endl;
                common::center("DONE!");
                cout<< endl;
                cout<< endl;
                common::center("Records saved in: \"" + temps + "\"");
                common::wait();
            }
        }
        
        
    }
}

namespace menu
{
    common_menu::menu_return_data manage_snapshots(settings::settings_data& settings)
    {
        using scrollDisplay::window_data_class;
        using common_menu::menu_return_data;
        using snapshot::snapshot_data;
        using std::cout;
        using key_code::key_code_data;
        using std::endl;
        using snapshot_diff_algo::diff_snapshots;
        
        bool finished(false);
        menu_return_data result;
        menu_data data;
        common_menu::selection_class selection;
        window_data_class<snapshot::snapshot_data> win(data.snapshots, snapshot_display);
        key_code_data key;
        
        data.settings = &settings;
        load_all_headers(data);
        win.win().window_size() = common_menu::wsize::value;
        
        while(!finished && common_menu::snapshots_exist(data.settings->global.snapshot_folder))
        {
            common::cls();
            cout<< endl;
            common::center("Manage snapshots: ");
            cout<< std::string(2, '\n');
            common_menu::display_scroll_window(win.win(), data.snapshots.size(), selection);
            cout<< endl;
            if(!selection.gselection().empty()) cout<< " '\\' -  Clear selection";
            cout<< endl;
            cout<< " [DEL] ----  Delete Snapshot"<< endl;
            cout<< " [SPC] ----  Select Snapshot (for comparison)"<< endl;
            cout<< " [ENTR] ---  Compare selected snapshots"<< endl;
            cout<< " [BCKSPC] -  Return to Main Menu";
            cout.flush();
            key = common::gkey_funct();
            
            if(key_code::is_listed_control(key))
            {
                if(!key.control_d.empty())
                {
                    using key_code::keys;
                    using namespace key_code::code;
                    
                    if(key == keys[up::value]) win.win().mv_up();
                    else if(key == keys[down::value]) win.win().mv_down();
                    else if(key == keys[pgup::value]) win.win().pg_up();
                    else if(key == keys[pgdown::value]) win.win().pg_down();
                    else if(key == keys[home::value]) while(win.win().pg_up());
                    else if(key == keys[end::value]) while(win.win().pg_down());
                    else if(key == keys[backspace::value]) finished = true;
                    else if(key == keys[del::value])
                    {
                        if(!data.snapshots.empty())
                        {
                            if(common::inp::is_sure("Do you really want to delete snapshot on \"" + 
                                    display_time(win.selected().timestamp) + "\"??"))
                            {
                                common::cls();
                                cout<< std::string(v_center::value, '\n')<< "Deleting... please wait...";
                                cout.flush();
                                if(remove_snapshot(data, win.selected().id))
                                {
                                    win.remove_selected();
                                    load_all_headers(data);
                                    if(!result.modified) result.modified = true;
                                }
                                else
                                {
                                    common::cls();
                                    cout<< std::string(v_center::value, '\n');
                                    common::center("Can not delete \"" + display_time(win.selected().timestamp) + "\"!");
                                    cout.flush();
                                    common::wait();
                                    common::cls();
                                }
                            }
                        }
                    }
                }
            }
            else
            {
                if(!key.is_control && !key.control_d.empty())
                {
                    switch((char)key.control_d.front())
                    {
                        case ' ':
                        {
                            if(!data.snapshots.empty())
                            {
                                if(!selection.is_selected(win.win().gpos().whole))
                                {
                                    if(selection.gselection().size() < 2) selection.add(win.win().gpos().whole);
                                }
                                else
                                {
                                    if(!selection.remove(win.win().gpos().whole))
                                    {
                                        /* if this happens then somthing awful has occured...  abort immediately! */
                                        ethrow("terrible terrible damage...  (could not remove the selected \
element from the hash map!!)");
                                    }
                                }
                            }
                        }
                        break;
                        
                        case '\\':
                        {
                            if(!selection.gselection().empty()) selection.clear();
                        }
                        break;
                        
                        case '\n':
                        {
                            if((data.snapshots.size() > 1) && (selection.gselection().size() > 1))
                            {
                                diff_snapshots(data, data.snapshots[selection[0]], data.snapshots[selection[1]]);
                                selection.clear();
                            }
                        }
                        break;
                        
                        default:
                        {
                        }
                        break;
                    }
                }
            }
        }
        return result;
    }
    
    
}