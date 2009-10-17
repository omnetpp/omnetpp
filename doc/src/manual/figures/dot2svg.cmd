REM TODO: make this part of the makefile[.vc]!
set DOT=C:\home\tools\Graphviz\bin\dot.exe
for %%I in (%~dp0\*.dot) do %DOT% %%I -Tsvg > %%~dpnI.svg
