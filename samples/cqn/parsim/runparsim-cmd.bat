@echo off
echo.
echo *** Distributed execution of the Closed Queueing Network ***
echo.
if "%*"=="" pause

rmdir /S /Q comm 2>nul
mkdir comm
mkdir comm\read

set NEDPATH=..
start /b /belownormal ..\cqn -p0,3 -c LargeLookahead -u Cmdenv %*
start /b /belownormal ..\cqn -p1,3 -c LargeLookahead -u Cmdenv %*
start /b /belownormal ..\cqn -p2,3 -c LargeLookahead -u Cmdenv %*
