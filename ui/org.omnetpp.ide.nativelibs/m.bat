@echo off
call ..\..\setenv-vc.bat
nmake -f Makefile.vc SHARED_LIBS=no %*
