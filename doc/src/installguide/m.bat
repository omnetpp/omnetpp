call ..\..\..\setenv-vc80.bat
call %TOOLS_DIR%\setenv.cmd

:nmake -f makefile.vc %*
nmake -f makefile.vc what=omnest %*

del tmp*.*