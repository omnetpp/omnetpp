@echo off
rem platform: MS-DOS/Borland C++ 3.1
echo *******************************************************
echo ****	Building CMDENV library...		****
echo *******************************************************

rem Create makefile from project file
if not exist cmdenv.mak prj2mak cmdenv.prj

rem Make object files
make -fcmdenv.mak

rem Build lib file
del cmdenv.lib
tlib /0 cmdenv.lib @makelib.lst
