@echo off
PATH=d:\home\omnetpp\bin;%PATH%
del *_n.ned
d:\perl\bin\perl nedgentest.pl %*

