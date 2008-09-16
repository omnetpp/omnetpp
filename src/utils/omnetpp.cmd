@echo off
SET IDEDIR=%~dp0\..\ide                                                                                        
cd %IDEDIR%
SET PATH=%~dp0;%~dp0\..\msys\bin;%~dp0\..\mingw\bin;%PATH%
echo Starting the OMNEST IDE...
start omnetpp.exe >out.log