@echo off
set IM6=D:\Program Files\ImageMagick-6.1.6-Q8

cd figures

for %%I in (*.wmf) do "%IM6%"\convert %%I -trim %%~dpnI.gif
for %%I in (*.gif) do "%IM6%"\convert %%I %%~dpnI.pdf

