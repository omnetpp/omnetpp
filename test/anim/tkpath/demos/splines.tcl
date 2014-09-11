package require tkpath 0.3.0

set t .c_splines
toplevel $t
set w $t.c
pack [tkp::canvas $w -width 400 -height 400 -bg white]

$w create text 160 80 -text "Quadratic spline" -anchor w
$w create text 160 100 -text "M 20  100 Q 80 20 140 100" -anchor w
$w create path "M 20  100 L 80 20 140 100" -stroke blue  -strokewidth 1
$w create path "M 20  100 Q 80 20 140 100" -stroke black -strokewidth 3
$w create path "M 15  100 h 10 m -5 -5 v 10" -stroke red
$w create path "M 75  20  h 10 m -5 -5 v 10" -stroke red
$w create path "M 135 100 h 10 m -5 -5 v 10" -stroke red

$w create text 160 220 -text "Cubic spline" -anchor w
$w create text 160 240 -text "M 20 250 C 60 140 100 380 140 250" -anchor w
$w create path "M 20 250 L 60 140 100 380 140 250" -stroke blue  -strokewidth 1
$w create path "M 20 250 C 60 140 100 380 140 250" -stroke black -strokewidth 3
$w create path "M 15  250 h 10 m -5 -5 v 10" -stroke red
$w create path "M 55  140 h 10 m -5 -5 v 10" -stroke red
$w create path "M 95  380 h 10 m -5 -5 v 10" -stroke red
$w create path "M 135 250 h 10 m -5 -5 v 10" -stroke red




