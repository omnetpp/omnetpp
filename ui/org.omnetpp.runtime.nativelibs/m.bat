@echo off
del *.dll
call ..\..\setenv.bat
nmake -f Makefile.vc %*
