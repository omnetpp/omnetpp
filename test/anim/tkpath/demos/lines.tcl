package require tkpath 0.3.0

set t .c_lines
toplevel $t
set w $t.c
pack [tkp::canvas $w -width 400 -height 400 -bg white]

$w create pline 20  20 180 20
$w create pline 200 20 260 20 -stroke blue

$w create pline 20  30 180 30 -stroke green
$w create pline 200 30 260 30 -stroke red

$w create pline 20 40 260 40 -stroke "#999999"
$w create pline 40 50 120 80 -stroke "#666666" -strokewidth 3

$w create pline 150 60 170 60 -stroke red -strokewidth 4
$w create pline 150 70 170 70 -stroke green -strokewidth 4
$w create pline 150 80 170 80 -stroke blue -strokewidth 4

$w create polyline 20 200 30 200 30 180 50 180 50 200  \
  70 200 70 160 90 160 90 200 110 200 110 120 130 120  \
  130 200

$w create polyline 150 200  200 120  150 120  200 200  -stroke gray50 -strokewidth 4
$w create polyline 220 200  270 120  220 120  270 200  -stroke gray50 -strokewidth 4 \
  -fill gray80

$w create ppolygon 75 237  89 280  134 280  98 307  111 350  75 325  38 350  \
  51 307  15 280  60 280 -stroke "#9ac790" -strokewidth 4 -fill "#cae2c5"

$w create ppolygon 240 250  283 275  283 325  240 350  196 325  196 275 \
  -stroke "#a19de2" -strokewidth 6 -fill "#d6d6ff"

$w create text 300  20  -anchor w -text "pline"
$w create text 300 150  -anchor w -text "polyline"
$w create text 300 300 -anchor w -text "ppolygon"

