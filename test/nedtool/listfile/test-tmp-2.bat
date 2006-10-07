@echo off
echo Test @@listfile syntax -- files are to the working dir
echo EXPECTED: FAILURE (a.ned not found -- simply it's in another dir)
cd dir
nedtool -V @@../tmp/tmplist.lst