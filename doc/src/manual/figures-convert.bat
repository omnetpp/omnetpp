@echo off
set IM6=D:\Program Files\ImageMagick-6.1.6-Q8

cd figures

:this sucks: for %%I in (*.wmf) do echo %%I to %%~nI.gif && "%IM6%"\convert -density 300 -trim %%I %%~nI.gif

for %%I in (*.gif) do echo crop %%I && "%IM6%"\convert -trim %%I %%I
: convert to png because the gifs still contain some garbage meta-tag which screws up pdf
for %%I in (*.gif) do echo %%I to %%~nI.png && "%IM6%"\convert %%I %%~nI.png
for %%I in (*.png) do echo %%I to %%~nI.pdf && "%IM6%"\convert %%I %%~nI.pdf
del *.png
