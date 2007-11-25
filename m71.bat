call setenv-vc71.bat
del src\nedc\nedtool.exe
nmake -f makefile.vc MODE=debug CONFIGNAME=vc71-debug %*
