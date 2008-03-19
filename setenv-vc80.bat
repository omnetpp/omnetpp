@echo off              
: &*$@ Windows sets errorlevel if variable doesn't exist
if defined INCLUDE set INCLUDE=
if defined LIB set LIB=
set PATH=%SystemRoot%\system32;%SystemRoot%;%SystemRoot%\System32\Wbem

set OMNETPP_ROOT=%~dp0

call %TOOLS_DIR%\vc80\vcvars32.bat
rem call %TOOLS_DIR%\setenv.cmd
PATH %OMNETPP_ROOT%\bin;%OMNETPP_ROOT%\msys\bin;%PATH%
rem PATH %OMNETPP_ROOT%\bin;%PATH%
