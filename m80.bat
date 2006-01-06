call setenv-vc80.bat
del src\nedc\nedtool.exe
nmake -f makefile.vc %*
