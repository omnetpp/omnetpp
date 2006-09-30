@echo off

: nedxml test: parse then regenerate .ned files
: FIXME turn into automated test!

mkdir work 2>nul
del /q work\*.* >nul

D:\home\omnetpp40\omnetpp\src\nedxml\tmp_nedtool.exe -V -n -Q -s .nd_out *.ned
move *.nd_out work >nul
ren work\*.nd_out *.ned

D:\home\omnetpp40\omnetpp\src\nedxml\tmp_nedtool.exe -x -s .xml_out *.ned
move *.xml_out work >nul
ren work\*.xml_out *.xml

