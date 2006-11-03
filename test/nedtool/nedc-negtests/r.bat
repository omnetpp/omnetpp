@echo off
PATH=C:\home\omnetpp\bin;%PATH%
C:\perl\bin\perl ned_negtest.pl %*
del *.err
