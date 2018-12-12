#!/bin/sh

BATIK_HOME=$HOME/batik-1.5.1
BATIK=java -jar "${BATIK_HOME}/batik-rasterizer.jar"
CONVERT=convert

batik() {
  # $1=abstract,device,... $2=pixel size  $3=size suffix(_l,_s,...)
  $BATIK -w $2 -h $2 "$1/*.svg"
  mkdir -p ../png/$1
  for i in $1/*.png; do
     png=`echo $i | sed 's/\.png$//'`; png=../png/$png$3.png
     $CONVERT -trim $i $png
     $CONVERT $png \( +clone -fill '#c0c0c0' -draw "color 0,0 reset" \) -compose Dst_Over -composite __tmp.png
     $CONVERT $png __tmp.png -compose In -composite $png
  done
}

batikall() {
  echo "Creating icons in size: $1"
  batik abstract $1 $2
  batik block $1 $2
  batik device $1 $2
  batik msg $1 $2
  batik misc $1 $2
  batik status $1 $2
}

mkdir ../png

batikall 60 _l
batikall 24 _s
batikall 16 _vs
batikall 40

