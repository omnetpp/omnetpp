@echo off
if not "x%1" == "x" echo Please change to the directory you want to convert and run the script there without any comnmand line argument! && goto endlabel
echo.
echo Press ENTER to convert all NED files under the current directory:
cd
echo.
pause

dir /s /b *.ned >nedfiles.lst

echo The following files will be checked/modified:
type nedfiles.lst | more

echo.
echo Press ENTER to start the conversion, or CTRL-C to quit.
pause

nedtool -P @nedfiles.lst

echo.
echo NED files has been converted.
echo.
echo Since there were semantic changes in the NED language, there are details
echo that could not be converted automatically. Please revise the following:
echo.
echo  1. Adjust "import" directives. Previously they pointed to files, now
echo     they refer to module types.
echo.
echo  2. Remove "volatile" qualifiers from parameters that are only read
echo     from the C++ code once, at the beginning of the simulation.
echo.
echo  3. Old "numeric" parameters have been converted to "double", but
echo     you may want to change some of those to "int".
echo.
echo  4. Remove unnecessary networks definitions. Compound modules can now be
echo     used as networks directly, by changing the "module" keyword to
echo     "network".
echo.
echo  5. Additionally, you can change the files to take advantage of new NED
echo     features: module default icons, parameter physical units, inout gates
echo     and bidirectional connections, inheritance, inner types, and so on.
echo.
echo You may safely re-run this script any time you want.

:endlabel