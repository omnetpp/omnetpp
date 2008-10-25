@echo off
rem
rem usage: runtest [<testfile>...]
rem without args, runs all *.test files in the current directory
rem

set TESTFILES=%*
if "x%TESTFILES%" == "x" set TESTFILES=*.test

mkdir work 2>nul
call opp_test %OPT% -g -v %TESTFILES% || goto end

cd work || goto end
for /d %%i in (*) do echo %%i && cd %%i && nmake -f build || goto end
for /d %%i in (*) do echo %%i && cd %%i && nmake -f Makefile.vc || goto end

: nmake -f makefile.vc || cd .. && goto end
: cd .. || goto end
: call opp_test %OPT% -r -v %TESTFILES% || goto end

echo.
echo Results can be found in work/

:end