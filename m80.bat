call setenv-vc8.bat
del src\nedc\nedtool.exe
nmake -f makefile.vc %*
