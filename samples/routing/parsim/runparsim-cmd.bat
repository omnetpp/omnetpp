@echo off
echo.
echo *** Distributed execution of the Routing example simulation ***
echo.
if "%*"=="" pause

rmdir /S /Q comm 2>nul
mkdir comm
mkdir comm\read

set NEDPATH=..
start /b /belownormal ..\routing -p0,2 -u Cmdenv %*
start /b /belownormal ..\routing -p1,2 -u Cmdenv %*
