@echo off
rem
rem Document NED files given as arguments.
rem *** this file uses NT command extensions, it won't work on 95/98/ME ***
rem output will be created in html/ subdirectory
rem
PATH %~dp0;%PATH%
echo generating docs without diagrams (lacking conversion from PS to GIF)
echo transforming ned and msg files to xml...
nedtool.exe -x -e -t -y -m -o %TEMP%\$$$ %* || goto CLEANUP
echo applying xslt stylesheet...
mkdir html 2>nul
del /q html\*.* 2>nul
rem --- and if gifs are exported, --stringparam imagesxml html/images.xml should be passed to the stylesheet ---
xsltproc.exe --stringparam outputdir html --stringparam imagesxml "" %~dp0\neddoc.xsl %TEMP%\$$$ > nul || goto CLEANUP
echo formatting comments...
perl %~dp0\neddocproc.pl html\*.html || goto CLEANUP
echo documentation created in html/ -- start page is index.html
:CLEANUP
del %TEMP%\$$$ 2>nul
