@echo off
SET IDEDIR=%~dp0\..\ide                                                                                        
cd %IDEDIR%
SET PATH=%~dp0;%~dp0\..\msys\bin;%PATH%
echo "Starting the OMNeT++ IDE..."
start omnetpp.exe >out.log