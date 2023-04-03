@echo off
SET IDEDIR=%~dp0\..\ide                                                                                        
SET LAUNCHER=omnetpp
cd %IDEDIR%
SET PATH=%~dp0;%~dp0\..\msys\bin;%~dp0\..\mingw\bin;%PATH%
echo Starting the OMNeT++ IDE...
start %LAUNCHER% >out.log