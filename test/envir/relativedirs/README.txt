Tests that names of included ini files and list files are understood 
as relative to the location of ini file and list file containing them.

Test 1: Run nedtool @foo/list1.lst -- this should complete without error,
        and should generate _n.cc files for all NED files in this dir and
        in foo/ as well.

Test 2: When running the executable, there should be no error. (The model 
        itself does nothing.)

