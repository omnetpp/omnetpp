@echo off
rem This is the same magic as the opp_neddoc shell script.
rem  Cannot be done without using NT command line extensions :-(
if "_%OS%_"=="_Windows_NT_" goto NT
echo opp_neddoc: error: this script only works on NT! (NT4/W2K/XP)
goto end
:NT
PATH %PATH%;%~dp0
nedtool.exe -x -e -y -m -o %TEMP%\$$$ %* || goto CLEANUP
xsltproc.exe -o neddoc.html %~dp0\neddoc.xsl %TEMP%\$$$ || goto CLEANUP
echo neddoc.html created
:CLEANUP
del %TEMP%\$$$ 2>nul
:END
