@echo off
D:\home\omnetpp40\omnetpp\src\nedxml\tmp_nedtool.exe -n -Q -s .ned_out *.ned

mkdir work 2>nul
del /q work\*.* >nul
move *.ned_out work >nul
ren work\*.ned_out *.ned

