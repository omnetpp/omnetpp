package require tkpath 0.3.0

set t .c_randlines
toplevel $t
set w $t.c
set size 400
pack [tkp::canvas $w -width $size -height $size -bg white]

set x0 10
set y0 10
for {set i 0} {$i < 100} {incr i} {
    set x [expr {($size - 20)*rand() + 10}]
    set y [expr {($size - 20)*rand() + 10}]
    set red   [expr {int(255*rand())}]
    set green [expr {int(255*rand())}]
    set blue  [expr {int(255*rand())}]
    set color [format "#%02x%02x%02x" $red $green $blue]
    $w create pline $x0 $y0 $x $y -stroke $color -strokewidth 2
    set x0 $x
    set y0 $y
}

