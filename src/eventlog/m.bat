@echo off
call ..\..\setenv-vc80.bat
nmake -f Makefile.vc %*

