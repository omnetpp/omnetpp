@echo off
if not "%1" == "" goto dump

echo opp_stacktool: print reserved stack size for a Windows executable
echo Usage: opp_stacktool ^<program.exe^>
echo  1. Needs dumpbin.exe in the path (part of MS Visual C++); also needs Perl.
echo  2. Currently only prints the stack size. To set the stack size, type:
echo          editbin /stack:^<stack-size^> ^<program.exe^>
echo     Then use this tool to verify the result.
echo.
goto end

:dump
if not exist %1 goto notfound
dumpbin /headers %1 | perl -nae "if (/stack reserve/) {printf('%1: %%.2dK', hex($F[0])/1024)}"
echo.
goto end

:notfound
echo opp_stacktool: program %1 not found

:end

