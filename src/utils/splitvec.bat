@echo off
rem splitvec.bat:
rem   Splits an output vector (.vec) file into files with one vector in each
rem	ie: foo.vec --> foo-1.vec, foo-2.vec, foo-3.vec,...
rem Required gawk.exe comes with the OMNeT++ for DOS distribution.

echo %0: splitting file '%1' to separate vector files
gawk -f \omnetpp\utils\splitvec %1
