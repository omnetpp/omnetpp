package require tkpath 0.3.0

set t .c_arcs
destroy $t
toplevel $t
set w $t.c
pack [tkp::canvas $w -width 500 -height 400 -bg white]

$w create path "M 20 350 l 50 -25 \
  a 25 25  -30 0 1 50 -25 l 50 -25 \
  a 25 50  -30 0 1 50 -25 l 50 -25 \
  a 25 75  -30 0 1 50 -25 l 50 -25 \
  a 25 100 -30 0 1 50 -25 l 50 -25" -stroke red -strokewidth 2

$w create path "M 30 350 h 100 a 25 200 0 0 1 50 0 h 60" \
  -stroke blue -strokewidth 2

$w create path "M 100 100 a 25 25 -30 0 1 50 -25 z" -fill yellow -strokewidth 2
$w create path "M 180 100 a 25 25  30 0 1 50  25 z" -fill yellow -strokewidth 2

set r 40
set a 10
set b 6
set b2 [expr {2*$b}]
set r2 [expr {2*$r}]
set ra [expr {$r+$a}]
set a2 [expr {2*$r+$a}]

proc tkp::circlepath {r} {
    
    return [list M -6 -$a l -$b -$b M -6 -$a l -$b $b]
}

$w create path "M 0 0 A $r $r 0 1 1 0 $r2 A $r $r 0 1 1 0 0 Z" \
  -strokewidth 2 -tag acircle

$w create path "M 0 -$a A $ra $ra 0 1 1 6 $a2" \
  -stroke red -tag acircle
$w create path "M 0 -$a v -$b v $b2" \
  -stroke red -tag acircle
$w create path "M 6 $a2 l $b -$b M 6 $a2 l $b $b" \
  -stroke red -tag acircle

$w create path "M 0 $a2 A $ra $ra 0 1 1 -6 -$a" \
  -stroke red -tag acircle
$w create path "M 0 $a2 v -$b v $b2" \
  -stroke red -tag acircle
$w create path "M -6 -$a l -$b -$b M -6 -$a l -$b $b" \
  -stroke red -tag acircle

$w create ptext -20 [expr {$a2+30}] \
  -text "M 0 0 A $r $r 0 1 1 0 $r2 A $r $r 0 1 1 0 0 Z" \
  -textanchor middle -tags acircle

$w move acircle 400 220

# Make an elllipse around origo and put it in place using a transation matrix
namespace import ::tcl::mathop::*
proc ellipsepath {x y rx ry} {
    list \
            M $x [- $y $ry] \
            a $rx $ry 0 1 1 0 [*  2 $ry] \
            a $rx $ry 0 1 1 0 [* -2 $ry] \
            Z
}
set Phi [expr {45 / 180.0 * 3.1415926535}]
set cosPhi [expr {cos($Phi)*4}]
set sinPhi [expr {sin($Phi)*4}]
set msinPhi [- $sinPhi]
set matrix \
        [list [list $cosPhi $msinPhi] [list $sinPhi $cosPhi] \
        [list 200 200]]
$w create path [ellipsepath 0 0 20 10] -stroke purple -matrix $matrix

