@echo off
set IM6=D:\Program Files\ImageMagick-6.1.6-Q8

cd figures

for %%I in (*.wmf) do echo %%I to %%~nI.gif && "%IM6%"\convert -density 150 -trim %%I %%~nI.gif
for %%I in (*.gif) do echo %%I to %%~nI.pdf && "%IM6%"\convert %%I %%~nI.pdf

