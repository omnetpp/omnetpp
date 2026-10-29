@echo off
rem Starts the MSYS2 bash of the unpacked Windows distribution mounted at Z:\w under Wine.
rem mingwenv.cmd cannot be used: Wine's cmd.exe drops the extra arguments in msys2_shell.cmd.
set HOME=Z:\w
set MSYSTEM=CLANG64
set CHERE_INVOKING=1
cd /d Z:\w
Z:\w\tools\win32.x86_64\usr\bin\bash.exe %*
