@echo off
::
:: Modify this batch file according to your needs!
::
REM set JAVA_HOME=C:\"Program Files"\Java\JDK1.5.0
PATH=%PATH%;%JAVA_HOME%\jre\bin\client;%JAVA_HOME%\bin
set CLASSPATH=%CLASSPATH%;..\simkernel.jar;.\classes

mkdir classes >nul 2>nul
del classes\*.class
javac *.java -d classes || exit 1

yoursimulation.exe
