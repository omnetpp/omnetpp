@echo off
rmdir /S /Q comm 2>nul
mkdir comm
mkdir comm\read

start "Partition 0" /b /belownormal ..\cqn -p0,3
start "Partition 1" /b /belownormal ..\cqn -p1,3
start "Partition 2" /b /belownormal ..\cqn -p2,3
