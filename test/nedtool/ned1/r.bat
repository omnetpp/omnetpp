@echo off
call ..\..\..\setenv-vc90.bat
del *_n.ned
perl.exe nedgentest.pl %*

