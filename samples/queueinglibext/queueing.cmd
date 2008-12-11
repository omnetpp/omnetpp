@echo off
cd %~dp0
set PATH=%~dp0\..\queueinglib;%PATH%
opp_run -lqueueinglibext %*

