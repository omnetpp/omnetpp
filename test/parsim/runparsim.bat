@echo off
set NEDPATH=.
start /b /belownormal parsim -p0,2 %*
start /b /belownormal parsim -p1,2 %*
