@echo off

: nedxml test: convert old-syntax .ned files to ned2, then back -- result should be identical (almost)
: note: this must be run with and without -e flag (expr parsing)

: FIXME turn into automated test!

mkdir work 2>nul
del /q work\*.* >nul

mkdir ned2 2>nul
del /q ned2\*.* >nul

C:\home\omnetpp40\omnetpp\src\nedxml\tmp_nedtool.exe -V -n -e -s .nd_out *.ned
move *.nd_out ned2 >nul
ren ned2\*.nd_out *.ned

C:\home\omnetpp40\omnetpp\src\nedxml\tmp_nedtool.exe -V -n -e -Q -s .nd_out ned2\*.ned
move ned2\*.nd_out work >nul
ren work\*.nd_out *.ned

