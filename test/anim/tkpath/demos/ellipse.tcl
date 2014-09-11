package require tkpath 0.3.0

set t .c_ellipse
toplevel $t
set w $t.c
pack [tkp::canvas $w -width 400 -height 400 -bg white]

$w create circle 60  60 -r 32 -stroke "#c8c8c8" -fill "#e6e6e6"
$w create circle 200 60 -r 32 -stroke "#a19de2" -fill "#d6d6ff"

$w create circle 60  160 -r 40 -stroke "#9ac790" -fill "#cae2c5"
$w create circle 200 160 -r 40 -stroke "#e2a19d" -fill "#ffd6d6"

$w create ellipse 200 280 -rx 20 -ry 60 -stroke "#999999"
$w create ellipse 100 260 -rx 60 -ry 20 -stroke "#666666" -strokewidth 3 -fill "#bdbdbd"

set id [$w create ellipse 280 280 -rx 20 -ry 60]
$w bind $id <Button-1> [list puts "hit $id"]

$w create circle 300 220 -r 8 -fill red -stroke ""
$w create circle 300 240 -r 8 -fill green -stroke ""
$w create circle 300 260 -r 8 -fill blue -stroke ""
