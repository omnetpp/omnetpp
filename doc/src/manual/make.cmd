call ..\..\..\setenv-vc.bat
set PATH=%PATH%;C:\Latex\MiKTeX 2.9\miktex\bin
nmake -f makefile.vc %* &&  : start ..\..\manual\usman.html
