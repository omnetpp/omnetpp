REM TODO: make this part of the makefile[.vc]!
set INKSCAPE="C:\Progra~1\inkscape\inkscape.exe"
for %%I in (%~dp0\*.svg) do %INKSCAPE% %%I -e %%~dpnI.png
