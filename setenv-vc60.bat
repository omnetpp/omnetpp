@echo off              
: &*$@ Windows sets errorlevel if variable doesn't exist
if defined INCLUDE set INCLUDE=
if defined LIB set LIB=
set PATH=%SystemRoot%\system32;%SystemRoot%;%SystemRoot%\System32\Wbem

set OMNETPP_ROOT=%~dp0

call %TOOLS_DIR%\vc98\vcvars32.bat
call %TOOLS_DIR%\setenv.cmd
PATH %~dp0\bin;%PATH%

