@echo off
rem Document all ned files in the current directory and subdirectories.
rem  Cannot be done without using NT command line extensions :-(
if "_%OS%_"=="_Windows_NT_" goto NT
echo opp_neddocall: error: this script only works on NT! (NT4/W2K/XP)
goto end
:NT
PATH %PATH%;%~dp0
dir /s /b *.ned > %TEMP%\$$$1 || goto CLEANUP
nedtool.exe -x -e -y -m -o %TEMP%\$$$ @%TEMP%\$$$1 || goto CLEANUP
xsltproc.exe -o neddoc.html %~dp0\neddoc.xsl %TEMP%\$$$ || goto CLEANUP
echo neddoc.html created
:CLEANUP
del %TEMP%\$$$ %TEMP%\$$$1 2>nul
:END
