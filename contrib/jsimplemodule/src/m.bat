@echo off
call c:\home\omnetpp\setenv-vc71.bat
set JAVA_HOME=%TOOLS_DIR%\jdk1.5.0
::PATH=%PATH%;%JAVA_HOME%\jre\bin\client;%JAVA_HOME%\bin
PATH=C:\OMNEST\bin;%PATH%;%JAVA_HOME%\jre\bin\client;%JAVA_HOME%\bin
@echo on

::call opp_nmakemake -f -N
call opp_nmakemake -f -N -c C:\OMNEST\configuser.vc
nmake -f Makefile.vc %* || exit 1
