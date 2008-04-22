@echo off
call ..\..\setenv.bat
nmake -f Makefile.vc %*
