package require tkpath 0.3.0

set t .c_hittest
toplevel $t
set w $t.c
pack [tkp::canvas $w -width 400 -height 400 -bg white]

set id [$w create path "M 20 20 L 120 20 v 30 h -20 z"]
$w bind $id <Button-1> [list puts "hit $id"]

set id [$w create path "M 10 80 h 100 v 100 z" -fill red]
$w bind $id <Button-1> [list puts "hit $id (red triangle)"]

set id [$w create path "M 20 200 Q 50 120 100 200 T 150 200 200 200"]
$w bind $id <Button-1> [list puts "hit $id (quad bezier)"]

set id [$w create path "M 10 250 h 80 v 80 h -80 z m 20 20 h 40 v 40 h -40 z" \
  -fill green -fillrule nonzero]
$w bind $id <Button-1> [list puts "hit $id (green with nonzero rule)"]

set id [$w create path "M 110 250 h 80 v 80 h -80 z m 20 20 h 40 v 40 h -40 z" \
  -fill blue -fillrule evenodd]
$w bind $id <Button-1> [list puts "hit $id (blue with evenodd rule)"]

set id [$w create path "M 220 50 v 100" -strokewidth 36 -strokelinecap round]
$w bind $id <Button-1> [list puts "hit $id (fat line with rounded caps)"]


