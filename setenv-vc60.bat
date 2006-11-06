@echo off
: &*$@ Windows sets errorlevel if variable doesn't exist
if defined INCLUDE set INCLUDE=
if defined LIB set LIB=
set PATH=%SystemRoot%\system32;%SystemRoot%;%SystemRoot%\System32\Wbem

call C:\home\tools\vc98\vcvars32.bat
call C:\home\tools\setenv.cmd
PATH %~dp0\bin;%PATH%

