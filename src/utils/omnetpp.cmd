@echo off
SET IDEDIR=%~dp0\..\ide
SET LAUNCHER=omnetpp
SET PATH=%~dp0;%~dp0\..\msys\usr\bin;%~dp0\..\msys\mingw32\bin;%PATH%
SET DEFAULT_WORKSPACE_ARGS="-vmargs -Dosgi.instance.area.default=%IDEDIR%\..\samples"
echo Starting the OMNeT++ IDE...
start %IDEDIR%\%LAUNCHER% %* %DEFAULT_WORKSPACE_ARGS% >out.log
