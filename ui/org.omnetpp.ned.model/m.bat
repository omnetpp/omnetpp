@echo off
call ..\..\setenv.cmd
nmake -f Makefile.vc %*
