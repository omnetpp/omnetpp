package require tkpath 0.3.0
 
set t .c_clock
toplevel $t
set w $t.c
pack [tkp::canvas $w -width 400 -height 400 -bg white]


namespace eval ::clock {
    
    variable w $::w
    
    set r1 160
    set r2 140
    set r3 120
    set r4 100
    
    for {set i 1} {$i <= 12} {incr i} {
	set phi [expr (30.0*$i - 90.0)*3.14159/180.0]
	set sinPhi [expr sin($phi)]
	set cosPhi [expr cos($phi)]
	set pt1($i) [list [expr $r1*$cosPhi] [expr $r1*$sinPhi]]
	set pt2($i) [list [expr $r2*$cosPhi] [expr $r2*$sinPhi]]
	set pt3($i) [list [expr $r3*$cosPhi] [expr $r3*$sinPhi]]
    }
    
    $w create path \
      "M $pt2(1)  L $pt1(1)  M $pt2(2)  L $pt1(2)  M $pt3(3)  L $pt1(3) \
      M $pt2(4)  L $pt1(4)  M $pt2(5)  L $pt1(5)  M $pt3(6)  L $pt1(6) \
      M $pt2(7)  L $pt1(7)  M $pt2(8)  L $pt1(8)  M $pt3(9)  L $pt1(9) \
      M $pt2(10) L $pt1(10) M $pt2(11) L $pt1(11) M $pt3(12) L $pt1(12)" \
      -tags clock -strokewidth 4 -strokelinecap round
    
    $w create path "M 0 4 L $r4 4  $r4 10  $r2 0  $r4 -10  $r4 -4 0 -4 z" \
      -stroke "" -fill gray50 -tags pointer
    
    $w move clock   200 200
    $w move pointer 200 200
    
    proc ticker {secs} {
	variable w
	if {[winfo exists $w]} {
	    after 1000 [list clock::ticker [expr [incr secs] % 60]]
	    set phi [expr $secs*2.0*3.14159/60.0]
	    set m [::tkp::transform rotate $phi 200 200]
	    $w itemconfig pointer -m $m
	}
    }
    
    ticker -15
}


