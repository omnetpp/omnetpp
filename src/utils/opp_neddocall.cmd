@echo off
rem
rem Document all NED files in the current directory and subdirectories.
rem *** this file uses NT command extensions, it won't work on 95/98/ME ***
rem output will be created in html/ subdirectory
rem
PATH %~dp0;%PATH%
echo collecting ned and msg files...
dir /s /b *.ned *.msg > %TEMP%\$$$1 || goto CLEANUP
echo transforming to xml...
nedtool.exe -x -e -t -y -m -o %TEMP%\$$$ @%TEMP%\$$$1 || goto CLEANUP
rem --- here we should export gifs ---
echo applying xslt stylesheet...
del /q html\*.* 2>nul
rem --- and if gifs are exported, --stringparam imagesxml html/images.xml should be passed to the stylesheet ---
xsltproc.exe --stringparam outputdir html --stringparam imagesxml '' %~dp0\neddoc.xsl %TEMP%\$$$ > nul || goto CLEANUP
echo formatting comments...
perl %~dp0\neddocproc.pl html\*.html || goto CLEANUP
echo documentation created in html/ -- start page is index.html
:CLEANUP
del %TEMP%\$$$ %TEMP%\$$$1 2>nul

