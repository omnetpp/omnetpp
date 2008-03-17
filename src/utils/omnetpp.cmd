@echo off
SET IDEDIR=%~dp0\..\ide                                                                                        
cd $IDEDIR
SET PATH=%~dp0;%~dp0\..\msys\bin;%PATH%
eclipse.exe >out.log & 