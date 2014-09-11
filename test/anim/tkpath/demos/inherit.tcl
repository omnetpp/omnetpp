#
# This file demonstrates the inheritance mechanisms. Note that items inherit
# style options set in their parents, but the items own style option
# takes precedence.
#
package require tkpath 0.3.0

set t .c_inherit
destroy $t
toplevel $t
set w $t.c
pack [tkp::canvas $w -width 400 -height 400 -bg white]

array set stroke [list 1 "#c8c8c8" 2 "#a19de2" 3 "#9ac790" 4 "#e2a19d"]
array set fill   [list 1 "#e6e6e6" 2 "#d6d6ff" 3 "#cae2c5" 4 "#ffd6d6"]

$w create prect 10 10 390 390 -rx 20 -strokewidth 4 -stroke gray70 -tags g0

foreach i {1 2 3 4} {

    $w create group -tags g$i -strokewidth 3 -stroke $stroke($i) -fill $fill($i)
    $w create prect 10 10 180 180 -rx 10 -parent g$i -fill ""

    set id [$w create path "M 0 0 l 30 40 h -60 z" -parent g$i]
    $w move $id 60 40
    
    set id [$w create path "M -20 0 h 40 l -40 80 h 40 z" -parent g$i]
    $w move $id 140 40

    set id [$w create ellipse 0 0 -rx 30 -ry 20 -parent g$i -strokewidth 6]
    $w move $id 60 140
}

$w move g1 10 10
$w move g2 200 10
$w move g3 10 200
$w move g4 200 200

unset -nocomplain stroke fill


