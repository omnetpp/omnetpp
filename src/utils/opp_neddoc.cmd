@echo off
rem Document NED files given as arguments.
rem *** this file uses NT command extensions, it won't work on 95/98/ME ***
PATH %PATH%;%~dp0
nedtool.exe -x -e -y -m -o %TEMP%\$$$ %* || goto CLEANUP
xsltproc.exe -o neddoc.html %~dp0\neddoc.xsl %TEMP%\$$$ || goto CLEANUP
echo neddoc.html created
:CLEANUP
del %TEMP%\$$$ 2>nul
