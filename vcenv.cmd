@echo off

: **** NOT FOR RELEASE -- FOR DEVELOPMENT ONLY

rem Start with a clean PATH, INCLUDE and LIB
if defined INCLUDE set INCLUDE=
if defined LIB set LIB=
set PATH=%SystemRoot%\system32;%SystemRoot%;%SystemRoot%\System32\Wbem

rem Specify your MS Visual C++ installation here.
rem  e.g. C:\Program Files\Microsoft Visual Studio 12.0\VC
set VCINSTALLDIR=%VS120COMNTOOLS%\..\..\VC

if NOT EXIST "%VCINSTALLDIR%\bin\vcvars32.bat" goto MESSAGE

set OMNETPP_ROOT=%~dp0
set OMNETPP_IMAGE_PATH=.\images;%OMNETPP_ROOT%\images
set PATH=%OMNETPP_ROOT%\bin;%OMNETPP_ROOT%\tools\win32\usr\bin;%OMNETPP_ROOT%\tools\win32\mingw32\bin;%PATH%

call "%VCINSTALLDIR%"\bin\vcvars32.bat

goto EOF

:MESSAGE
echo You must specify the location of your Visual C++ installation.
echo Open 'vcenv.cmd' with your editor and set the VCINSTALLDIR
echo variable to point to your Visual C++ installation directory.

:EOF
