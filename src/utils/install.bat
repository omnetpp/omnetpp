@echo off
rem ---------------------------------------------------------------------
rem Installs OMNeT++ from .arj packages. Not necessary if you downloaded
rem the distribution in a single .zip archive.
rem ---------------------------------------------------------------------
rem You can set the source and the installation directory by changing
rem the following two lines.
set SRC=.
set DEST=f:\omnetpp

if "X%SRC%" == "X" goto set_error
if "X%DEST%" == "X" goto set_error
goto install

:set_error
echo.
echo ERROR: The SRC and DEST environment vars were not set properly!
echo  o If you are running install from within a shell (Norton Commander etc),
echo.   exit it!
echo  o Make sure you have enough environment space!
goto end

:install
echo.
echo Installing OMNeT++
echo ~~~~~~~~~~~~~~~~~~
echo    Source directory: %SRC%
echo    Destination directory: %DEST%
echo.
echo If this is not what you want, kill this batch file and edit it!
echo.
pause

echo.
echo Creating directory structure...
mkdir %DEST%

rem ---------------

rem System files
mkdir %DEST%\src
mkdir %DEST%\src\nedc
mkdir %DEST%\src\gned
mkdir %DEST%\src\plove
mkdir %DEST%\src\sim
mkdir %DEST%\src\envir
mkdir %DEST%\src\envir\cmdenv
mkdir %DEST%\src\envir\tvenv
mkdir %DEST%\src\envir\tkenv
mkdir %DEST%\src\utils

rem Misc/doc
mkdir %DEST%\bitmaps
mkdir %DEST%\doc
mkdir %DEST%\html
mkdir %DEST%\arj
mkdir %DEST%\gnu
mkdir %DEST%\stuff

rem Models
mkdir %DEST%\models
mkdir %DEST%\models\support

rem Samples
mkdir %DEST%\samples
mkdir %DEST%\samples\pvmex
mkdir %DEST%\samples\dyna
mkdir %DEST%\samples\nim
mkdir %DEST%\samples\fddi
mkdir %DEST%\samples\token
mkdir %DEST%\samples\hist
mkdir %DEST%\samples\fifo1
mkdir %DEST%\samples\fifo2
mkdir %DEST%\samples\hcube

echo.
echo Copying the files...
copy %SRC%\*.arj    %DEST%\arj  > NUL

echo Unpacking...

rem ---------------
rem system files
arj x -y -hf2 %SRC%\nedc*.arj   %DEST%\src\nedc           > NUL
arj x -y -hf2 %SRC%\gned*.arj   %DEST%\src\gned           > NUL
arj x -y -hf2 %SRC%\plove*.arj  %DEST%\src\plove          > NUL
arj x -y -hf2 %SRC%\sim*.arj    %DEST%\src\sim            > NUL
arj x -y -hf2 %SRC%\envir*.arj  %DEST%\src\envir          > NUL
arj x -y -hf2 %SRC%\cmden*.arj  %DEST%\src\envir\cmdenv   > NUL
arj x -y -hf2 %SRC%\tvenv*.arj  %DEST%\src\envir\tvenv    > NUL
arj x -y -hf2 %SRC%\tkenv*.arj  %DEST%\src\envir\tkenv    > NUL
arj x -y -hf2 %SRC%\utils*.arj  %DEST%\src\utils          > NUL

rem misc/doc
arj x -y -hf2 %SRC%\bitmap*.arj %DEST%\bitmaps        > NUL
arj x -y -hf2 %SRC%\doc*.arj    %DEST%\doc            > NUL
arj x -y -hf2 %SRC%\html*.arj   %DEST%\html           > NUL
arj x -y -hf2 %SRC%\gnu*.arj    %DEST%\gnu            > NUL
arj x -y -hf2 %SRC%\stuff*.arj  %DEST%\stuff          > NUL

rem models
arj x -y -hf2 %SRC%\supp*.arj   %DEST%\models\support > NUL

rem samples
arj x -y -hf2 %SRC%\fddi*.arj   %DEST%\samples\fddi   > NUL
arj x -y -hf2 %SRC%\dyna*.arj   %DEST%\samples\dyna   > NUL
arj x -y -hf2 %SRC%\pvmex*.arj  %DEST%\samples\pvmex  > NUL
arj x -y -hf2 %SRC%\nim*.arj    %DEST%\samples\nim    > NUL
arj x -y -hf2 %SRC%\token*.arj  %DEST%\samples\token  > NUL
arj x -y -hf2 %SRC%\hist*.arj   %DEST%\samples\hist   > NUL
arj x -y -hf2 %SRC%\fifo1-*.arj %DEST%\samples\fifo1  > NUL
arj x -y -hf2 %SRC%\fifo2-*.arj %DEST%\samples\fifo2  > NUL
arj x -y -hf2 %SRC%\hcube*.arj  %DEST%\samples\hcube  > NUL

echo.
echo Installation finished.
echo.

:end
