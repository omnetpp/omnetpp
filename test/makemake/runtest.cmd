@echo off
rem
rem usage: runtest [<testfile>...]
rem without args, runs all *.test files in the current directory
rem

set TESTFILES=%*
if "x%TESTFILES%" == "x" set TESTFILES=*.test

mkdir work 2>nul
call opp_test %OPT% -g -v %TESTFILES% || goto end

echo > .project

cd work || goto end

echo Creating makefiles...
cd %~dp0\work
for /d %%i in (*) do echo %%i && cd %~dp0\work\%%i && nmake -f build || goto end

echo Build...
cd %~dp0\work
for /d %%i in (*) do echo %%i && cd %~dp0\work\%%i && nmake -f Makefile.vc || goto end

: nmake -f makefile.vc || cd .. && goto end
: cd .. || goto end
: call opp_test %OPT% -r -v %TESTFILES% || goto end

echo.
echo Results can be found in work/

:end