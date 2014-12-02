@echo off
rem
rem usage: runtest [<testfile>...]
rem without args, runs all *.test files in the current directory
rem

set TESTFILES=%*
if "x%TESTFILES%" == "x" set TESTFILES=*.test

:: Extract files
mkdir work 2>nul
call opp_test gen %OPT% -v %TESTFILES% || goto end

cd work || goto end
echo > .project

echo Creating makefiles...
cd %~dp0\work
for /d %%i in (*) do echo %%i && cd %~dp0\work\%%i && nmake "OPP_MAKEMAKE=call opp_nmakemake -f --out ../out" -f buildspec || echo MAKEFILE CREATION FAILED: %%i

echo Build...
cd %~dp0\work
call opp_nmakemake -f -r --nolink && nmake -f Makefile.vc || echo BUILD FAILED && goto end

:: Run the tests
echo.
cd %~dp0
call opp_test run %OPT% -v %TESTFILES% || goto end

echo.
echo Results can be found in work/

:end