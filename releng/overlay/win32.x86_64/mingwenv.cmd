@echo off
set HOME=
set HOME=%~dp0
cd "%HOME%\tools"

rem msys environment: clang64 or mingw64
set ENVIRONMENT=mingw64
set USE_MSYS_SEED=no
if "%USE_MSYS_SEED%" == "yes" (
  IF EXIST "opp-tools-win32-x86_64-msys.7z" (
    cls
    echo.
    echo We need to unpack the Msys toolchain before continuing.
    echo.
    7za x -aos -y -owin32.x86_64 opp-tools-win32-x86_64-msys.7z && del opp-tools-win32-x86_64-msys.7z opp-tools-win32-x86_64-mingw64-toolchain.7z opp-tools-win32-x86_64-mingw64-dependencies.7z
    echo Msys installed. Use ./install.sh to install dependencies and build OMNeT++.
  )
)

IF EXIST "opp-tools-win32-x86_64-mingw64-toolchain.7z" (
cls
echo.
echo We need to unpack the MinGW toolchain before continuing.
echo This can take a while, please be patient.
echo.
7za x -aos -y -owin32.x86_64 opp-tools-win32-x86_64-mingw64-toolchain.7z && del opp-tools-win32-x86_64-mingw64-toolchain.7z
)

IF EXIST "opp-tools-win32-x86_64-mingw64-dependencies.7z" (
7za x -aos -y -owin32.x86_64\opt\mingw64 opp-tools-win32-x86_64-mingw64-dependencies.7z && del opp-tools-win32-x86_64-mingw64-dependencies.7z && "win32.x86_64\opt\mingw64\bin\qtbinpatcher.exe" --qt-dir=win32.x86_64\opt\mingw64
echo *** MinGW-64 toolchain extracted. ***
)

cd "%HOME%"
rem Open the MinGW command shell (you may add -full-path to force the MSYS shell to inherit the current system path)
if "%1" == "ide" (
rem if the first paramter is "ide" we start the IDE instead of the shell. This can be used to start the IDE from a shortcut
call "%HOME%\tools\win32.x86_64\msys2_shell.cmd" -%ENVIRONMENT% -c "nohup >/dev/null 2>/dev/null $HOME/bin/omnetpp"
) else (
call "%HOME%\tools\win32.x86_64\msys2_shell.cmd" -%ENVIRONMENT% -defterm %*
)
