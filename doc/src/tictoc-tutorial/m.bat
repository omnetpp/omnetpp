call ..\..\..\vcvars32.bat
doxygen doxyfile
copy *.gif ..\..\tictoc-tutorial
start ..\..\tictoc-tutorial\index.html
