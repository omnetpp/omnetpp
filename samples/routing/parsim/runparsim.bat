@echo off
echo.
echo *** Distributed execution of the Routing example simulation ***
echo If you're running the simulation under the Tkenv GUI, you'll have to 
echo click [Run] in all windows to start the simulation. 
echo See Readme.txt for more info.
echo.
if "%*"=="" pause

rmdir /S /Q comm 2>nul
mkdir comm
mkdir comm\read

set NEDPATH=..
start /b /belownormal ..\routing -p0,2 %*
start /b /belownormal ..\routing -p1,2 %*
