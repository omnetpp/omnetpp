@echo off
dir /s /b *.ini >inifiles.lst
perl %~dp0\migrateini.pl inifiles.lst
