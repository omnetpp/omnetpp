@echo off
rem 
rem usage: runtest [<testfile>...]
rem without args, runs all *.test files in the current directory   
rem

rem TBD error handling -- if possible with this crap cmd.exe @!$#@!

set TESTFILES=%*
if "x%TESTFILES%" == "x" set TESTFILES=*.test

path ..\bin;..\..\bin;%PATH%
call vcvars32.bat

mkdir work 2>nul
cmd /c opp_test -g -v %TESTFILES%

cd work
cmd /c opp_nmakemake -f -e cc -u cmdenv
nmake -f makefile.vc
cd ..

cmd /c opp_test -r -v %TESTFILES%
echo.
echo Results can be found in work/
