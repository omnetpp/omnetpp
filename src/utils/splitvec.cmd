@echo off
rem splitvec.bat:
rem   Splits an output vector (.vec) file into files with one vector in each
rem	ie: foo.vec --> foo-1.vec, foo-2.vec, foo-3.vec,...
rem Required gawk.exe can be found in CYGWIN or DJGPP

PATH %~dp0;%PATH%
gawk -f %~dpn0 %1
