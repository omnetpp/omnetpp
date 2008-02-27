@echo off
echo.
echo Press ENTER to convert all NED files in the current directory:
cd
echo.
pause
dir /s /b *.ned >nedfiles.lst
nedtool -P @nedfiles.lst

echo REMINDER1: Please revise import statements. Previously they pointed to files, now they refer to module types.
echo.
echo REMINDER2: Remove unnecessary networks definitions. Compound modules can be used as netwroks directly by changing the "module" keyword to "network"
echo.
echo REMINDER3: The new NED syntax now supports inheritance. Common Submodule icons can be specified in the submodule type directly instead of specifying for each submodule instance.