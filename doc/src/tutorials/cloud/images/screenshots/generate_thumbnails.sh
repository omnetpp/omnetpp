#!/bin/bash

for i in *.png
do
  # The images will be displayed at 160x90 size max, but the browser might be
  # zoomed on HiDPI displays and on mobile, so let's do 1.5x resolution.
  # This only increases the total size from 168k to 320k (at this moment), and is less ugly.
  convert $i -resize 240x135 ${i/.png/.thumb.jpg}
done
