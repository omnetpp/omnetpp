package require tkpath 0.3.0

set t .c_text
toplevel $t
set w $t.c
pack [tkp::canvas $w -width 400 -height 400 -bg "#c6ceef" -highlightthickness 0]

$w create ptext 200 360 -text "X" -fontsize 400 -fill "" \
  -stroke gray -strokewidth 2 -textanchor middle
$w create ptext 0 0 -text "Coccinella" -fontfamily Helvetica -fontsize 64 \
  -fill white -fillopacity 0.7 -matrix {{1 0.3} {-0.3 1} {50 80}} \
  -stroke gray -strokewidth 2
$w create ptext 200 300 -text "Made by Mats" -fontfamily Times -fontsize 40 \
  -fill white -textanchor middle



