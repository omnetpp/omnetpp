@echo off
rem This is the same magic as the opp_neddoc shell script. Cannot be done
rem without using NT command line extensions :-(
if "_%OS%_"=="_Windows_NT_" goto NT
echo opp_nedc: error: this script only works on NT! (NT4/W2K/XP)
goto end
:NT
nedtool.exe -x -e -y -m -o %TEMP%\$$$ %*
xsltproc.exe %~dp0\neddoc.xsl %TEMP%\$$$
del %TEMP%\$$$
:END
