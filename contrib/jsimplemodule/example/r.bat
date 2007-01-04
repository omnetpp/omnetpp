@echo off
::
:: Modify this batch file according to your needs!
::
set JDK=%TOOLS_DIR%\jdk1.5.0
PATH=%PATH%;%JDK%\jre\bin\client;%JDK%\bin
set CLASSPATH=%CLASSPATH%;..\simkernel.jar;.\bin

mkdir bin >nul 2>nul
del bin\*.class
javac *.java -d bin || exit 1

..\src\src.exe
