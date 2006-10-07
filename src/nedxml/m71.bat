call ..\..\setenv-vc71.bat
nmake -f makefile.vc depend
nmake -f makefile.vc %*
