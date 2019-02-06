@echo off
echo Test @@listfile syntax -- files are to the working dir
echo EXPECTED: SUCCESS (processing a.ned, b.ned)
nedtool -V @@tmp\tmplist.lst