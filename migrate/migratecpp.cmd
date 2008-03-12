@echo off
if not "x%1" == "x" echo Please change to the directory you want to convert and run the script there without any comnmand line argument! && goto endlabel
echo.
echo Press ENTER to convert all CC,CPP files in the current directory:
cd
echo.
pause
dir /s /b *.cc *.h *.cpp *.hpp >cppfiles.lst
perl %~dp0\migratecpp.pl cppfiles.lst

:endlabel
