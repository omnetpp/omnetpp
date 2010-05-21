@echo off
rem Specify your MS Visual C++ 8/9 installation here.
rem  e.g. C:\Program Files\Microsoft Visual Studio 9.0\VC
set VCINSTALLDIR=

if NOT EXIST "%VCINSTALLDIR%\bin\vcvars32.bat" goto MESSAGE

set OMNETPP_ROOT=%~dp0
set OMNETPP_IMAGE_PATH=.\images;%OMNETPP_ROOT%\images
set PATH=%OMNETPP_ROOT%\bin;%OMNETPP_ROOT%\msys\bin;%PATH%

call "%VCINSTALLDIR%"\bin\vcvars32.bat

goto EOF

:MESSAGE
echo You must specify the location of your Visual C++ installation.
echo Open 'vcenv.cmd' with your editor and set the VCINSTALLDIR
echo variable to point to your Visual C++ installation directory.

:EOF
