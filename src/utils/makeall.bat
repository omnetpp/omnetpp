@echo off
rem You can set the OMNeT++ source base directory by changing the following line:
set OMNETPP_DIR=c:\omnetpp

if "X%OMNETPP_DIR%" == "X" goto set_error
goto cont

:set_error
echo.
echo ERROR: The OMNETPP environment var could not be set properly!
echo  o If you are running install from within a shell (Norton Commander etc),
echo.	exit it!
echo  o Make sure you have enough environment space!
goto end

:cont
echo.
echo Building OMNeT++ libraries
echo ~~~~~~~~~~~~~~~~~~~~~~~~~~
echo Your configuration may need some changes to run this batch file successfully.
echo	1. You must have Borland C++ 3.1 installed on your machine.
echo	2. Borland C++ must be installed in E:\BORLANDC, and E:\BORLANDC\BIN
echo	   must be in your path.
echo	   If you have BC on a different drive, you may use the DOS SUBST
echo	   command to map that drive to E:. (For example, type SUBST E: C:\ if
echo	   BC is in C:\BORLANDC)
echo	   If the directory name is different (for example, E:\BC31 instead of
echo	   E:\BORLANDC), I recommend that you rename the directory. All project
echo	   files (.prj; there are a number of them) in the package assume that
echo	   BC is installed in E:\BORLANDC and would be quite some work to change
echo	   the directory settings in all of them.
echo	3. You must also have the command line compiler (bcc.exe), TASM and some
echo	   tools (make.exe, prj2mak.exe) installed. OMNeT++ compiles in large model
echo	   and Tvenv uses the Turbo Vision library, so make sure you have them
echo	   installed too.
echo	4. You'll also need to have bison and flex in your path to compile JAR
echo	   (they are in C:\OMNETPP\GNU.) They need environment vars; run setenv.bat
echo	   in advance to set them!
echo.
echo Press Ctrl+Break here if you need to massage your machine...
pause
echo OK. Please verify that you have OMNeT++ installed in this directory:
echo	     %OMNETPP_DIR%
echo.
echo If in not, press Ctrl+Break now and edit the this batch file.
pause
echo Watch for errors! 'Linker error: undefined symbol _main' messages are OK.
echo.
cd %OMNETPP_DIR%\src\nedc
call bc3make

cd %OMNETPP_DIR%\src\sim
call bc3make

cd %OMNETPP_DIR%\src\envir
call bc3make

cd %OMNETPP_DIR%\src\envir\cmdenv
call bc3make

cd %OMNETPP_DIR%\src\envir\tvenv
call bc3make

cd %OMNETPP_DIR%

echo.
echo Done! Enjoy OMNeT++.
echo.

:end
