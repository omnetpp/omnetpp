@echo off

set OMNETPP_ROOT=%~dp0
set OMNETPP_IMAGE_PATH=.\images;%OMNETPP_ROOT%\images
set PATH=%OMNETPP_ROOT%\bin;%OMNETPP_ROOT%\msys\bin;%OMNETPP_ROOT%\mingw\bin;%PATH%

rem If you use MS Visual C++, call its vcvars32.bat here
rem modify the path depending on the location of the compiler
set VCINSTALLDIR=C:\Program Files\Microsoft Visual Studio 9.0\VC
call "%VCINSTALLDIR%"\bin\vcvars32.bat
