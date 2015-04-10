@echo off
call ..\..\setenv.bat
nmake -f Makefile.vc SHARED_LIBS=no %*
