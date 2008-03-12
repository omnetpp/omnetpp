@echo off
if not "x%1" == "x" echo Please change to the directory you want to convert and run the script there without any comnmand line argument! && goto endlabel
echo.
echo Press ENTER to convert all MSG files under the current directory:
cd
echo.
pause

dir /s /b *.msg >msgfiles.lst

echo The following files will be checked/modified:
type msgfiles.lst | more

echo.
echo Press ENTER to start the conversion, or CTRL-C to quit.
pause

nedtool -P @msgfiles.lst

:endlabel