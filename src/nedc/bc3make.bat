@echo off
rem platform: MS-DOS/Borland C++ 3.1
echo *******************************************************
echo ****  Building the NEDC (formerly JAR) compiler... ****
echo *******************************************************

rem Creating makefile from project file
prj2mak nedc.prj

rem Build NEDC
make -fnedc.mak

rem Cleanup unneeded files
del nedc.mak
del nedc.cfg
