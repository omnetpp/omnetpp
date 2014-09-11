package require tkpath 0.3.0

set t .c_apple
toplevel $t
set w $t.c
pack [tkp::canvas $w -width 400 -height 400 -bg white]

set grad [$w gradient create linear -stops \
  {{0.0 "#00bb00"} {0.35 "#00bb00"} {0.35 "#ffff00"} {0.50 "#ffff00"} \
  {0.50 "#ff6600"} {0.65 "#ff6600"} {0.65 "#dd0000"} {0.8 "#dd0000"} \
  {0.8 "#3366cc"} {1.0 "#3366cc"}} \
  -lineartransition {0 0 0 1}]

$w create path "M 0 0 C 20 0 40 -20 70 -20 S 130 30 130 60 \
  110 200  60 200   20 180 0 180   \
  -10 200 -60 200   -130 90 -130 60  \
  -110 -20 -70 -20  -20 0 0 0 z \
  M 0 -10 Q -10 -60 50 -80 Q 50 -20 0 -10 z" \
  -fill $grad -stroke "" -tags apple

$w move apple 200 120




