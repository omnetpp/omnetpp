@echo off
PATH=C:\home\omnetpp\bin;%PATH%
del *_n.ned
C:\perl\bin\perl nedgentest.pl %*

