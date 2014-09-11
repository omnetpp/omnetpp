package require tkpath 0.3.0

set t .c_prect
destroy $t
toplevel $t
set w $t.c
pack [tkp::canvas $w -width 400 -height 400 -bg white]

$w create prect 20  20 180 80 -rx 6 -stroke "#c8c8c8" -fill "#e6e6e6"
$w create prect 200 20 260 80 -rx 6 -stroke "#a19de2" -fill "#d6d6ff"

$w create prect 20  100 180 180 -rx 6 -stroke "#9ac790" -fill "#cae2c5"
$w create prect 200 100 260 180 -rx 6 -stroke "#e2a19d" -fill "#ffd6d6"

$w create prect 20 200 260 380 -stroke "#999999" -tags hit
$w create prect 40 220 100 360 -rx 16 -stroke "#666666" -strokewidth 3 -fill "#bdbdbd"

$w create prect 150 240 170 260 -stroke "" -fill red
$w create prect 150 270 170 290 -stroke "" -fill green
$w create prect 150 300 170 320 -stroke "" -fill blue

$w create prect 280 200 360 380 -rx 20 -strokewidth 1 -strokedasharray {8 4 12}

bind $w <Button-1> {puts "distance to gray prect=[%W distance hit %x %y]"}
