package require tkpath 0.3.0

set t .c_style
destroy $t
toplevel $t
set w $t.c
pack [tkp::canvas $w -width 400 -height 400 -bg white]

proc filteropts {w S} {
    set opts [list]
    foreach spec [$w style config $S] {
	lassign $spec name - - dvalue value
	if {$dvalue ne $value} {
	    lappend opts $name $value
	}
    }
    return $opts
}

set S1 [$w style create -stroke "#c8c8c8" -fill "#e6e6e6"]
set S2 [$w style create -stroke "#a19de2" -fill "#d6d6ff"]
set S3 [$w style create -stroke "#9ac790" -fill "#cae2c5"]
set S4 [$w style create -stroke "#e2a19d" -fill "#ffd6d6"]
set S5 [$w style create -stroke "#666666" -strokewidth 3 -fill "#bdbdbd"]

foreach S [$w style names] {
    puts "style $S : [filteropts $w $S]"
}

$w create prect 20  20 180 80 -rx 6 -style $S1
$w create prect 200 20 260 80 -rx 6 -style $S2
$w create prect 20  100 180 180 -rx 6 -style $S3
$w create prect 200 100 260 180 -rx 6 -style $S4

$w create prect 20 200 260 380 -stroke "#999999"
$w create prect 40 220 100 360 -rx 16 -style $S5


$w create prect 150 240 170 260 -stroke "" -fill red
$w create prect 150 270 170 290 -stroke "" -fill green
$w create prect 150 300 170 320 -stroke "" -fill blue

