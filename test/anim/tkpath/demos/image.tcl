package require tkpath 0.3.0

set t .c_image
toplevel $t
set w $t.c
pack [tkp::canvas $w -width 400 -height 400 -bg white]

set dir [file dirname [info script]]
set imageFile [file join $dir trees.gif]
set name [image create photo -file $imageFile]
set x 20
set y 20
$w create pimage $x $y -image $name

$w create prect $x $y \
  [expr $x+[image width $name]] [expr $y+[image height $name]]

set m [::tkp::transform rotate 0.5]
lset m {2 0} 220
lset m {2 1} -120
$w create pimage 100 100 -image $name -matrix $m

set m [::tkp::transform scale 2 0.8]
$w create pimage 10 300 -image $name -matrix $m




