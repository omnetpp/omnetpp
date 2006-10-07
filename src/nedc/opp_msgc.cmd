@echo off
rem *** this file uses NT command extensions, it won't work on 95/98/ME ***
PATH %~dp0;%PATH%
perl %~dpn0 %*
