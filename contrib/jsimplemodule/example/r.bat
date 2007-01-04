@echo off
::
:: Modify this batch file according to your needs!
::
REM set JAVA_HOME=%TOOLS_DIR%\JDK1.5.0
PATH=%PATH%;%JAVA_HOME%\jre\bin\client;%JAVA_HOME%\bin
set CLASSPATH=%CLASSPATH%;..\simkernel.jar;.\bin

mkdir bin >nul 2>nul
del bin\*.class
javac *.java -d bin || exit 1

..\src\src.exe
