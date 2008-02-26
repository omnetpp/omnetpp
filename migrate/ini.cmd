@echo off
dir /s /b *.ini >inilist.lst
%TOOLS_DIR%\Perl\bin\perl.exe %~dp0\migrateinifile.pl inilist.lst
