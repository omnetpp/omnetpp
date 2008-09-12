@echo off
call ..\..\..\setenv-vc71.bat
doxygen doxyfile
copy *.gif ..\..\tictoc-tutorial
copy *.png ..\..\tictoc-tutorial
start ..\..\tictoc-tutorial\index.html
