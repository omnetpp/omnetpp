@echo off
rem Specify the location of your Visual Studio installation
set VS_INSTALL_DIR=C:\Program Files (x86)\Microsoft Visual Studio\2019\BuildTools

rem Specify Visual Studio batch file to start a developer console for 64-bit host/target builds.
set VCVARS64=%VS_INSTALL_DIR%\VC\Auxiliary\Build\vcvars64.bat

rem *** Do not change anything below this line ***
set HOME=
set HOME=%~dp0

cd "%HOME%\tools"
IF EXIST "opp-tools-win32-x86_64-mingw64-toolchain.7z" (
cls
echo.
echo We need to unpack the MinGW toolchain before continuing.
echo This can take a while, please be patient.
echo.
7za x -aos -y -owin32.x86_64 opp-tools-win32-x86_64-mingw64-toolchain.7z && del opp-tools-win32-x86_64-mingw64-toolchain.7z
)

IF EXIST "opp-tools-win32-x86_64-visualc-dependencies.7z" (
cls
echo.
echo We need to unpack the libraries required by the ms-clang toolchain before continuing.
echo This can take a while, please be patient.
echo.
7za x -aos -y -owin32.x86_64\opt\visualc opp-tools-win32-x86_64-visualc-dependencies.7z && del opp-tools-win32-x86_64-visualc-dependencies.7z && win32.x86_64\opt\visualc\bin\qtbinpatcher.exe --qt-dir=win32.x86_64\opt\visualc
echo *** Files for Microsoft ABI compatible toolchain extracted. ***
)

cd "%HOME%"

rem check for the MS Visual C++ installation
if NOT EXIST "%VCVARS64%" goto MESSAGE_STUDIO_MISSING
call "%VCVARS64%"

rem Delete local variables so child process will not inherit it unnecessarily
set VS_INSTALL_DIR=

rem Open the MinGW command shell (you may add -full-path to force the MSYS shell to inherit the current system path)
if "%1" == "ide" (
rem if the first paramter is "ide" we start the IDE instead of the shell. This can be used to start the IDE from a shortcut
call "%HOME%\tools\win32.x86_64\msys2_shell.cmd" -mingw64 -c "nohup >/dev/null 2>/dev/null $HOME/bin/omnetpp"
) else (
call "%HOME%\tools\win32.x86_64\msys2_shell.cmd" -mingw64 -defterm %*
)

goto EOF

:MESSAGE_STUDIO_MISSING
echo You must specify the the location of your Visual Studio installation.
echo.
echo Open 'vcenv.cmd' with your editor and set the VS_INSTALL_DIR
echo variable to point to your installation.

:EOF
