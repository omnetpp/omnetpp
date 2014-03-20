#!/bin/sh

from_dir=images
to_dir=clearlooks


# transpose
for t in $from_dir/sbthumb-v*.png; do
    echo transpose $t
    mv -f $t tmp.png
    convert tmp.png -transpose $t
    rm -f tmp.png
done
for t in progress scaletrough; do
    echo transpose $t
    convert $from_dir/$t-h.png -transpose $from_dir/$t-v.png
done


# convert to gif
for f in $from_dir/*.png; do
    t=`basename $f .png`
    echo convert $t
    pngtopnm images/$t.png | ppmtogif -transparent==red > $to_dir/$t.gif
done



