call C:\home\omnetpp40\omnetpp\setenv-vc71.bat
call opp_nmakemake -f
nmake -f makefile.vc MODE=debug %*
:nmake -f makefile.vc MODE=release %*
