@echo off
call ..\..\..\setenv-vc71.bat
doxygen doxyfile
copy *.gif ..\..\tictoc-tutorial
start ..\..\tictoc-tutorial\index.html
