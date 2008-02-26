@echo off
dir /s /b %1\*.ned >nedlist.lst
dir /s /b %1\*.ini >inilist.lst
nedtool -m -e -x -o allnedfiles.xml @@nedlist.lst