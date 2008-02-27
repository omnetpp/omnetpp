@echo off
echo.
echo Press ENTER to convert all INI files in the current directory:
cd
echo.
pause
dir /s /b *.ini >inifiles.lst
perl %~dp0\migrateini.pl inifiles.lst

