@echo off
echo Test @listfile syntax -- files are relative to the listfile, working dir is irrelevant
echo EXPECTED: SUCCESS (a.ned, b.ned)
nedtool -V @list.lst