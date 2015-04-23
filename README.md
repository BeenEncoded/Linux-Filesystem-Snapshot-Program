#Filesystem Snapshot Program 
[![Build Status](https://travis-ci.org/BeenEncoded/Linux-Filesystem-Snapshot-Program.svg?branch=developement_branch)](https://travis-ci.org/BeenEncoded/Linux-Filesystem-Snapshot-Program) 
 
--- 
###What does it do? 
 
This is meant to be a tool for those who wish to keep track of the changes made to their filesystems.  **It does not keep track of changes made to the contents of files**.  It only keeps track of which files are created and which ones are deleted. 
 
###How does it work? 
 
If we know all the paths that exist at one point in time, and all the paths that exist at another point in time, we can make a comparison and see which paths were created, and which were deleted.  This is information that can be useful when we want to see what kinds of stuff a program's "installation" clogs up our machine with, or what files are changed over time -- and there are plenty.  This information can tell us a lot about what changes are being made to the disk. 
 
###Features: 
 
--- 
 
-  Take "snapshots" of the filesystem. 
-  Compare two snapshots, and save the record of the comparison. 
-  Manage comparison records, and view/edit them in your favorite editor. 
-  Filter the results of a comparison with regular expressions. 
 
###Future plans: 
 
--- 
 
-  Hashing files so that a diff can include whether or not a file has been changed, and not just it's location or name. 
 
--- 
 
###Build Dependencies: 
 
-  Linux 
-  Boost Library
-  NCurses
