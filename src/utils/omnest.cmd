@echo off
SET IDEDIR=%~dp0\..\ide
SET LAUNCHER=opp_ide
SET DEFAULT_WORKSPACE_ARGS=-vmargs -Dosgi.instance.area.default=%IDEDIR%\..\samples
echo Starting the OMNEST IDE...
start %IDEDIR%\%LAUNCHER% %* %DEFAULT_WORKSPACE_ARGS% >out.log
