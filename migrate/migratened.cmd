@echo off
dir /s /b *.ned >nedfiles.lst
nedtool -P @nedfiles.lst
