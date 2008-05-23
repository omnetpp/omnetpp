call ..\..\..\setenv-vc80.bat
set PATH=%PATH%;C:\Latex\MiKTeX-2.7\miktex\bin
nmake -f makefile.vc %* &&  : start ..\..\manual\usman.html
