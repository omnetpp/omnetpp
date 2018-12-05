@echo off
set HOME=%~dp0
cd "%HOME%\tools"

IF EXIST "opp-tools-win64-msys.7z" (
cls
echo.
echo We need to unpack the MinGW toolchain before continuing.
echo This can take a while, please be patient.
echo.
pause
7za x -y opp-tools-win64-msys.7z
del opp-tools-win64-msys.7z
)

IF EXIST "opp-tools-win64-extra.7z" (
7za x -y opp-tools-win64-extra.7z
del opp-tools-win64-extra.7z
)

IF EXIST "opp-tools-win64-mingw64.7z" (
7za x -y opp-tools-win64-mingw64.7z
del opp-tools-win64-mingw64.7z

"win64\mingw64\bin\qtbinpatcher.exe" --qt-dir=win64\mingw64
echo *** MinGW-64 toolchain extracted. ***
pause
)

cd "%HOME%"
rem Open the MinGW command shell (you may add -full-path to force the MSYS shell to inherit the current system path)
if "%1" == "ide" (
rem if the first paramter is "ide" we start the IDE instead of the shell. This can be used to start the IDE from a shortcut
call "%HOME%\bin\omnetpp.cmd"
) else (
call "%HOME%\tools\win64\msys2_shell.cmd" -mingw64
)
