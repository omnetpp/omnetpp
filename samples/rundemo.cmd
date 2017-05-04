@echo off
set OMNETPP_ROOT=%~dp0\..
set OMNETPP_IMAGE_PATH=.\images;%OMNETPP_ROOT%\images
set PATH=%PATH%;%OMNETPP_ROOT%\bin;%OMNETPP_ROOT%\tools\win64\usr\bin

rem add visualc tools path if we are building for visualc
IF EXIST "%OMNETPP_ROOT%\lib\oppsim.lib" GOTO clangc2
IF EXIST "%OMNETPP_ROOT%\lib\oppsimd.lib" GOTO clangc2
GOTO cont1
:clangc2
set PATH=%OMNETPP_ROOT%\tools\win64\visualc\bin;%PATH%
start wish86 %~dp0\rundemo
:cont1

rem add mingw64 binary path if we are using mingw
IF EXIST "%OMNETPP_ROOT%\lib\liboppsim.a" GOTO mingw
IF EXIST "%OMNETPP_ROOT%\lib\liboppsimd.a" GOTO mingw
GOTO cont2
:mingw
set PATH=%OMNETPP_ROOT%\tools\win64\mingw64\bin;%PATH%
start wish %~dp0\rundemo
exit
:cont2
echo You must build the simulator before trying to run the demos.
