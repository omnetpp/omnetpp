@echo off
SET IDEDIR=%~dp0\..\ide
SET LAUNCHER=omnetpp
SET PATH=%~dp0;%~dp0\..\msys\bin;%~dp0\..\mingw\bin;%PATH%
SET DEFAULT_WORKSPACE_ARGS="-vmargs -Xms256m -Xmx1024m -XX:MaxPermSize=320m -Dosgi.instance.area.default=%IDEDIR%\..\samples"
echo Starting the OMNeT++ IDE...
start %IDEDIR%\%LAUNCHER% %* %DEFAULT_WORKSPACE_ARGS% >out.log
