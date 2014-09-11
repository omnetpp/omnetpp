package require tkpath 0.3.0

destroy ._fillrule
toplevel ._fillrule
set w ._fillrule.c
pack [tkp::canvas $w -bg white -width 300 -height 300]
$w create path "M 10 10 h 80 v 80 h -80 z m 20 20 h 40 v 40 h -40 z" \
  -fill green -fillrule nonzero

set id [$w create path "M 10 10 h 80 v 80 h -80 z m 20 20 h 40 v 40 h -40 z" \
  -fill blue -fillrule evenodd]
$w move $id 100 0

proc ellipsepathCW {x y rx ry} {
    return "M $x $y a $rx $ry 0 1 1 0 [expr {2*$ry}] a $rx $ry 0 1 1 0 [expr {-2*$ry}] Z"
}

proc ellipsepathCCW {x y rx ry} {
    return "M $x $y a $rx $ry 0 1 0 0 [expr {2*$ry}] a $rx $ry 0 1 0 0 [expr {-2*$ry}] Z"
}

set r1 40
set r2 20
set circleCW "[ellipsepathCW 0 0 $r1 $r1] [ellipsepathCW 0 20 $r2 $r2]"
set id [$w create path $circleCW -fill green -fillrule nonzero]
$w move $id 50 120

set circleCCW "[ellipsepathCW 0 0 $r1 $r1] [ellipsepathCCW 0 20 $r2 $r2]"
set id [$w create path $circleCCW -fill blue -fillrule evenodd]
$w move $id 150 120

$w create text  50 240 -text "nonzero"
$w create text 150 240 -text "evenodd"
