@echo off
rmdir /S /Q comm 2>nul
mkdir comm
mkdir comm\read

start /b /belownormal ..\cqn -p0,3
start /b /belownormal ..\cqn -p1,3
start /b /belownormal ..\cqn -p2,3
