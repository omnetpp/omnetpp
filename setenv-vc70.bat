@echo off              
set INCLUDE=
set LIB=
set PATH=%SystemRoot%\system32;%SystemRoot%;%SystemRoot%\System32\Wbem

call D:\home\tools\vc70\vcvars32.bat
call d:\home\tools\setenv.cmd
PATH %~dp0\bin;%PATH%

