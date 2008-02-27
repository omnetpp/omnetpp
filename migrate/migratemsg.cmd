@echo off
echo.
echo Press ENTER to convert all MSG files in the current directory:
cd
echo.
pause
dir /s /b *.msg >msgfiles.lst
nedtool -P @msgfiles.lst

