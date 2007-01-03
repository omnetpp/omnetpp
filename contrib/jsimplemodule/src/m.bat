@echo off
call c:\home\omnetpp\setenv-vc71.bat
set JDK=%TOOLS_DIR%\jdk1.5.0
PATH=%PATH%;%JDK%\jre\bin\client;%JDK%\bin
@echo on

call opp_nmakemake -f -N
nmake -f Makefile.vc %* || exit 1
