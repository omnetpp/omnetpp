@echo off
rem Document all NED files in the current directory and subdirectories.
rem *** this file uses NT command extensions, it won't work on 95/98/ME ***
PATH %PATH%;%~dp0
dir /s /b *.ned > %TEMP%\$$$1 || goto CLEANUP
nedtool.exe -x -e -y -m -o %TEMP%\$$$ @%TEMP%\$$$1 || goto CLEANUP
xsltproc.exe -o neddoc.html %~dp0\neddoc.xsl %TEMP%\$$$ || goto CLEANUP
echo neddoc.html created
:CLEANUP
del %TEMP%\$$$ %TEMP%\$$$1 2>nul
