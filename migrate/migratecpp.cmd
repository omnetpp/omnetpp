@echo off
echo.
echo Press ENTER to convert all CC,CPP files in the current directory:
cd
echo.
pause
dir /s /b *.cc *.h *.cpp *.hpp >cppfiles.lst
ruby %~dp0\migratecpp.rb
