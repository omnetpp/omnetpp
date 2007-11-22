@echo off
PATH %~dp0;%PATH%
perl %~dp0\opp_makemake --nmake %*
