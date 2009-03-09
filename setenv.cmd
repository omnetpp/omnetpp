@echo off

set OMNETPP_ROOT=%~dp0
set OMNETPP_IMAGE_PATH=.\images;%OMNETPP_ROOT%\images
set PATH=%OMNETPP_ROOT%\bin;%OMNETPP_ROOT%\msys\bin;%OMNETPP_ROOT%\mingw\bin;%PATH%

rem If you use MS Visual C++, call its vcvarsall.bat here
rem call "C:\Program Files\Microsoft Visual Studio 9.0\VC\vcvarsall.bat"
