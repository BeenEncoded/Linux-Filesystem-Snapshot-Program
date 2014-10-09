#include <unittest++/UnitTest++.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>

#include "filesystem.hpp"

namespace
{
    bool delete_folder(const std::string&);
    std::string root_path();
    std::vector<std::string> folder_names();
    std::vector<std::string> fpaths();
    bool copy_folder(const std::string&);
    
    
    
    inline bool delete_folder(const std::string& s)
    {
        using fsys::delete_iterator_class;
        
        if(fsys::is_folder(s).value && !fsys::is_symlink(s).value)
        {
            for(delete_iterator_class it(s); !it.at_end(); ++it) if(!it.err.value) it.skip();
            if(fsys::can_delete(s)) fsys::fdelete(s);
        }
        return (!fsys::is_folder(s).value && !fsys::is_symlink(s).value);
    }
    
    inline std::string filename(const std::string& s)
    {
        std::string temps(s);
        std::string::size_type tempi(temps.rfind(fsys::pref_slash()) + 1);
        if(tempi != std::string::npos) temps.erase(temps.begin(), (temps.begin() + tempi));
        return temps;
    }
    
    /* for the sake of the test, copies a folder into "./copy folder" */
    inline bool copy_folder(const std::string& s)
    {
        if(!fsys::is_folder(s).value) return false;
        using fsys::copy_iterator_class;
        using fsys::tree_riterator_class;
        using fsys::is_folder;
        using fsys::is_symlink;
        
        std::string dest(fsys::current_path() + fsys::pref_slash() + "copy folder");
        
        if(!fsys::is_folder(dest).value)
        {
            if(!fsys::create_folder(dest).value) return false;
        }
        
        auto count_contents = [](const std::string& temps)->unsigned int
        {
            using fsys::is_folder;
            using fsys::is_symlink;
            
            unsigned int count(0);
            
            std::string formated(temps);
            
            if(!formated.empty())
            {
                if(formated.back() == '\\') formated.erase((formated.begin() + (formated.size() - 1)));
            }
            if(is_folder(formated).value && !is_symlink(formated).value)
            {
                for(tree_riterator_class it(formated); !it.at_end(); ++it) count++;
            }
            return count;
        };
        
        for(copy_iterator_class it(s, dest); !it.at_end();)
        {
            ++it;
            if(!it.err.value)
            {
                std::ofstream out(std::string("copy errors.txt").c_str(), std::ios::app);
                out<< it.value()<< "   ERROR: \""<< it.err.error<< "\""<< std::endl;
                out.close();
            }
        }
        return (count_contents(s) == count_contents(std::string(dest + fsys::pref_slash() + filename(s))));
    }
    
    /* Returns the folder that we will be modifying contents of. */
    inline std::string root_path()
    {
        return "/home/jonathan/Desktop/C++_Dev/Current_Projects/filesystem_snapshot_program/unit_testing/build/test_folder";
    }
    
    /* Returns folder names that will be used for the test.  They are appended 
     * to the end of root_path() to create a full path.*/
    inline std::vector<std::string> folder_names()
    {
        return std::vector<std::string>{
            "directory symlinks only",
            "empty folder",
            "files only",
            "folders only",
            "mix of everything",
            "symlinks only"
        };
    }
    
    /* Uses root_path() and folder_names() to create a list of the full paths
     * used for the test.*/
    inline std::vector<std::string> fpaths()
    {
        std::vector<std::string> paths;
        unsigned int size(folder_names().size());
        
        for(unsigned int x = 0; x < size; x++)
        {
            paths.push_back(root_path() + fsys::pref_slash() + folder_names()[x]);
        }
        return paths;
    }
    
    
}


/*
TEST(filesystem_class_delete_iterator_complex_folder_test_case)
{
    using fsys::delete_iterator_class;
    using fsys::is_folder;
    using fsys::is_symlink;
    
    bool tempb(false);
    
    if(is_folder(root_path()).value && !is_symlink(root_path()).value)
    {
        for(delete_iterator_class it(root_path()); !it.at_end(); ++it);
    }
    tempb = (!is_folder(root_path()).value && !is_symlink(root_path()).value);
    CHECK(tempb);
}*/

SUITE(filesystem_copy_iterator_test_cases_test_suite)
{
    TEST(filesystem_copy_iterator_directory_symlinks_only_test)
    {
        bool tempb(copy_folder(fpaths()[0]));
        CHECK(tempb);
    }
    
    TEST(filesystem_copy_iterator_empty_folder_test)
    {
        bool tempb(copy_folder(fpaths()[1]));
        CHECK(tempb);
    }
    
    TEST(filesystem_copy_iterator_files_only_test)
    {
        bool tempb(copy_folder(fpaths()[2]));
        CHECK(tempb);
    }
    
    TEST(filesystem_copy_iterator_folders_only_test)
    {
        bool tempb(copy_folder(fpaths()[3]));
        CHECK(tempb);
    }
    
    TEST(filesystem_copy_iterator_mix_of_everything_test)
    {
        bool tempb(copy_folder(fpaths()[4]));
        CHECK(tempb);
    }
    
    TEST(filesystem_copy_iterator_symlinks_only_test)
    {
        bool tempb(copy_folder(fpaths()[5]));
        CHECK(tempb);
    }
}

SUITE(filesystem_class_delete_iterator_test_cases_test_suite)
{
    TEST(filesystem_delete_iterator_folder_symlinks_only_case)
    {
        bool tempb(delete_folder(fpaths()[0]));
        CHECK(tempb);
    }
    
    TEST(filesystem_delete_iterator_empty_folder_case)
    {
        bool tempb(delete_folder(fpaths()[1]));
        CHECK(tempb);
    }
    
    TEST(filesystem_delete_iterator_files_only_case)
    {
        bool tempb(delete_folder(fpaths()[2]));
        CHECK(tempb);
    }
    
    TEST(filesystem_delete_iterator_folders_only_case)
    {
        bool tempb(delete_folder(fpaths()[3]));
        CHECK(tempb);
    }
    
    TEST(filesystem_delete_iterator_mix_of_everything_case)
    {
        bool tempb(delete_folder(fpaths()[4]));
        CHECK(tempb);
    }
    
    TEST(filesystem_delete_iterator_file_symlinks_only_case)
    {
        bool tempb(delete_folder(fpaths()[5]));
        CHECK(tempb);
    }
}

