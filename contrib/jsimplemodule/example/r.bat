@echo off
set JDK=%TOOLS_DIR%\jdk1.5.0
PATH=%PATH%;%JDK%\jre\bin\client;%JDK%\bin
set CLASSPATH=%CLASSPATH%;..\distrib\simkernel.jar

del *.class
javac *.java || exit 1

..\full.exe