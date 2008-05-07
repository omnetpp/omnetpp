@echo off
if not "x%1" == "x" echo Please change to the directory you want to convert and run the script there without any command line argument! && goto endlabel
echo.
echo Press ENTER to convert all INI files under the current directory:
cd
echo.
pause

dir /s /b *.ini >inifiles.lst

echo The following files will be checked/modified:
type inifiles.lst | more

echo.
echo Press ENTER to start the conversion, or CTRL-C to quit.
pause

perl %~dp0\migrateini.pl inifiles.lst

:endlabel