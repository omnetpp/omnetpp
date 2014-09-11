package require tkpath 0.3.0

set t .c_opacity
destroy $t
toplevel $t
set w $t.c
pack [tkp::canvas $w -width 400 -height 400 -bg white]


namespace eval ::opacity {

    variable w $::w

    set r 60
    set d [expr 2*$r]
    set opacity 0.50
    variable rc 100
    
    foreach col {red green blue} {
	$w create circle 0 $r -r $r \
	  -stroke "" -fill $col -fillopacity $opacity -tags $col
    }
    
    $w move root 200 [expr 200-$r]
    
    variable time 0
    variable speed 0.06
    
    proc step {} {
	variable w 
	variable rc 
	variable time 
	variable speed
	
	if {![winfo exists $w]} {
	    return
	}
	set phi [expr $time*$speed]
	
	set tx [expr $rc*cos([expr $phi*11./17.])]
	set ty [expr $rc*sin($phi)]
	set m [list {1 0} {0 1} [list $tx $ty]]
	$w itemconfig red -matrix $m 
	
	set tx [expr $rc*cos($phi)]
	set ty [expr $rc*sin([expr $phi*3./7.])]
	set m [list {1 0} {0 1} [list $tx $ty]]
	$w itemconfig green -matrix $m
	
	set tx [expr $rc*cos([expr $phi*23./29. + 1.0])]
	set ty [expr $rc*sin([expr $phi + 1.0])]
	set m [list {1 0} {0 1} [list $tx $ty]]
	$w itemconfig blue -matrix $m
	
	incr time
	after 40 opacity::step	
    }
    
    step
}


