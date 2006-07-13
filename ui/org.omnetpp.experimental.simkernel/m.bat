@echo off
del *.dll
call ..\..\setenv-vc71.bat
nmake -f Makefile.vc %*

