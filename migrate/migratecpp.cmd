@echo off
if not "x%1" == "x" echo Please change to the directory you want to convert and run the script there without any command line argument! && goto endlabel
echo.
echo Press ENTER to convert all C++ files under the current directory:
cd
echo.
pause

rem We process .msg files as well, because of potential embedded cplusplus{{..}} sections
dir /s /b *.cc *.h *.cpp *.hpp *.msg >cppfiles.lst

echo The following files will be checked/modified:
type cppfiles.lst | more

echo.
echo Press ENTER to start the conversion or CTRL-C to quit.
pause

perl %~dp0\migratecpp.pl cppfiles.lst

:endlabel
