@echo off
: *** developer's copy of the run script ***
set JAVA_HOME=%TOOLS_DIR%\JDK1.5.0
PATH=%PATH%;%JAVA_HOME%\jre\bin\client;%JAVA_HOME%\bin
set CLASSPATH=%CLASSPATH%;..\src\distrib\simkernel.jar;.\classes

mkdir classes >nul 2>nul
del classes\*.class
javac *.java -d classes || exit 1

..\src\src.exe
