@echo off
rem
rem usage: runtest [<testfile>...]
rem without args, runs all *.test files in the current directory
rem

set TESTFILES=%*
if "x%TESTFILES%" == "x" set TESTFILES=*.test

path %~dp0\..\bin;%PATH%
mkdir work 2>nul
xcopy /i /y lib work\lib
set NEDPATH=.;../lib
set EXTRA_INCLUDES=-I../../../src

call opp_test gen %OPT% -v %TESTFILES% || goto end

cd work || goto end
call opp_nmakemake -f -e cc --deep --no-deep-includes %EXTRA_INCLUDES% || goto end
nmake -f makefile.vc || cd .. && goto end
cd .. || goto end

call opp_test run %OPT% -v %TESTFILES% || goto end

echo.
echo Results can be found in work/

:end