@echo off
echo.
echo *** Distributed execution of the Closed Queueing Network ***
echo If you're running CQN under the GUI, you'll have to click [Run] in all
echo three windows to start the simulation. See Readme.txt for more info.
echo.
pause

rmdir /S /Q comm 2>nul
mkdir comm
mkdir comm\read

start /b /belownormal ..\cqn -p0,3 %*
start /b /belownormal ..\cqn -p1,3 %*
start /b /belownormal ..\cqn -p2,3 %*
