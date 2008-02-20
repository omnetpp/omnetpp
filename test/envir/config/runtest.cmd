@echo off
rem
rem usage: runtest [<testfile>...]
rem without args, runs all *.test files in the current directory
rem

set TESTFILES=%*
if "x%TESTFILES%" == "x" set TESTFILES=*.test

path %~dp0\..\bin;%PATH%
mkdir work 2>nul
del work\work.exe 2>nul

call opp_test -N -g -v %TESTFILES% || goto end

cd work || goto end
call opp_nmakemake -N -x -I../../../../src/common -I../../../../src/envir -I../../../../include/platdep -f -e cc -u cmdenv || goto end
nmake -f makefile.vc || cd .. && goto end
cd .. || goto end

call opp_test -N -r -v %TESTFILES% || goto end

echo.
echo Results can be found in work/

:end