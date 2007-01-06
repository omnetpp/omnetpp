@echo off
set JAVA_HOME=%TOOLS_DIR%\JDK1.5.0
PATH=%PATH%;%JAVA_HOME%\jre\bin\client;%JAVA_HOME%\bin
set MODEL_DIR=%~dp0\..\example
set CLASSPATH=%CLASSPATH%;%~dp0\distrib\simkernel.jar;%MODEL_DIR%\classes
echo CLASSPATH = %CLASSPATH%
echo.

cd %MODEL_DIR%
mkdir classes >nul 2>nul
del classes\*.class
javac *.java -d classes || exit 1

%~dp0\src.exe
