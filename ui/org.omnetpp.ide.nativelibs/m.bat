@echo off
call ..\..\setenv-vc90.bat
nmake -f Makefile.vc SHARED_LIBS=no %*