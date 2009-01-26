@echo off
call ..\..\..\setenv-vc71.bat
nmake -f makefile.vc
start ..\..\tictoc-tutorial\index.html
