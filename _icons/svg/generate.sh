#!/util/cygwin/bin/sh

#JRE_HOME=D:/PROGRA~1/Java/j2re1.4.2_04
#BATIK_HOME=d:/Java/batik-1.5.1
#IM6=D:/PROGRA~1/ImageMagick-6.1.6-Q16

JRE_HOME=$JAVA_HOME/jre
BATIK_HOME=c:/Java/batik-1.5.1
IM6=C:/PROGRA~1/ImageMagick-6.2.0-Q16

batik() {
  # $1=abstract,device,... $2=pixel size  $3=size suffix(_l,_s,...)
  $JRE_HOME/bin/java -cp ".;${JRE_HOME}/lib/rt.jar" -jar "${BATIK_HOME}/batik-rasterizer.jar" -w $2 -h $2 "$1/*.svg"
  mkdir -p ../png/$1
  mkdir -p ../gif/$1
  for i in $1/*.png; do 
     png=`echo $i | sed 's/\.png$//'`; png=../png/$png$3.png
     gif=`echo $i | sed 's/\.png$//'`; gif=../gif/$gif$3.gif
     $IM6/convert -trim $i $png
     $IM6/convert $png \( +clone -fill '#c0c0c0' -draw "color 0,0 reset" \) -compose Dst_Over -composite __tmp.png
     $IM6/convert $png __tmp.png -compose In -composite $png
# do not convert to gif, because the image magic png2gif conversion is buggy
# leaving the displacement parameters in the gif file (what is not understood
# by any known program), use an other program like ACDC to convert everything to GIF
#     $IM6/convert $png __tmp.png -compose In -composite $gif
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
mkdir ../gif

batikall 60 _l
batikall 24 _s
batikall 16 _vs
batikall 40

