# tkpath.tcl --
# 
# 03Sep06RT - fixes
#       - "-return" in named gradient proc
#       - braces all expressions
#       - removed to [expr ... calls in side if {..}
#       - recode polygon helper switch pattern in ::coords (bug fix same as v0.1)
#
#       Various support procedures for the tkpath package.
#       
#  Copyright (c) 2005-2008  Mats Bengtsson
#  
# $Id$

namespace eval ::tkp {}


# ::tkp::transform --
# 
#       Helper for designing the -matrix option from simpler transformations.
#       
# Arguments:
#       cmd         any of rotate, scale, skewx, skewy, or translate
#       
# Results:
#       a transformation matrix

proc ::tkp::transform {cmd args} {
    
    set len [llength $args]
    
    switch -- $cmd {
	rotate {
	    if {($len != 1) && ($len != 3)} {
		return -code error "usage: transform rotate angle ?centerX centerY?"
	    }
	    set phi [lindex $args 0]
	    set cosPhi [expr {cos($phi)}]
	    set sinPhi [expr {sin($phi)}]
	    set msinPhi [expr {-1.0*$sinPhi}]
	    if {$len == 1} {
		set matrix \
		  [list [list $cosPhi $sinPhi] [list $msinPhi $cosPhi] {0 0}]
	    } elseif {$len == 3} {
		set cx  [lindex $args 1]
		set cy  [lindex $args 2]
		set matrix [list \
		  [list $cosPhi $sinPhi] \
		  [list $msinPhi $cosPhi] \
		  [list [expr {-$cx*$cosPhi + $cy*$sinPhi + $cx}] \
		  [expr {-$cx*$sinPhi - $cy*$cosPhi + $cy}]]]
	    }
	}
	scale {
	    if {$len == 1} {
		set sx [lindex $args 0]
		set sy $sx
	    } elseif {$len == 2} {
		set sx [lindex $args 0]
		set sy [lindex $args 1]
	    } else {
		return -code error "usage: transform scale s1 ?s2?"
	    }
	    set matrix [list [list $sx 0] [list 0 $sy] {0 0}]
	}
	skewx {
	    if {$len != 1} {
		return -code error "usage: transform skewx angle"
	    }
	    set tanPhi [expr {tan([lindex $args 0])}]
	    set matrix [list {1 0} [list $tanPhi 1] {0 0}]
	}
	skewy {
	    if {$len != 1} {
		return -code error "usage: transform skewy angle"
	    }
	    set tanPhi [expr {tan([lindex $args 0])}]
	    set matrix [list [list 1 $tanPhi] {0 1} {0 0}]
	}
	translate {
	    if {$len != 2} {
		return -code error "usage: transform translate x y"
	    }
	    set matrix [list {1 0} {0 1} [lrange $args 0 1]]
	}
	mult {
	    if {$len != 2} {
		return -code error "usage: transform mult transform1 transform2"
	    }
	    return tkp::mmult [lindex $args 0] [lindex $args 1]
	}
	default {
	    return -code error "unrecognized transform command: \"$cmd\""
	}
    }
    return $matrix
}

proc ::tkp::mmult {m1 m2} { 
    seteach {{a1 b1} {c1 d1} {tx1 ty1}} $m1 
    seteach {{a2 b2} {c2 d2} {tx2 ty2}} $m2 
    return [list  \
      [list [expr {$a1*$a2 + $c1*$b2}] [expr {$b1*$a2 + $d1*$b2}]] \
      [list [expr {$a1*$c2 + $c1*$d2}] [expr {$b1*$c2 + $d1*$d2}]] \
      [list [expr {$a1*$tx2 + $c1*$ty2 + $tx1}] \
      [expr {$b1*$tx2 + $d1*$ty2 + $ty1}]]] 
}

# Function  : seteach 
# ------------------------------ ------------------------------ ---- 
# Returns : - 
# Parameters : 
# Description : set a list of variables 
# Written : 01/10/2007, Arndt Roger Schneider 
#  roger.schneider@addcom.de 
#
# Rewritten  : 09/24/2007, Roger -- for tkpath::mmult 
# License   : Tcl-License 
# ------------------------------ ------------------------------ ---- 

proc ::tkp::seteach {variables arglist} {
    foreach i $variables j $arglist { 
	set lgi [llength $i]
	if {1 < $lgi && [llength $j] == $lgi} { 
	    uplevel [list seteach $i $j]
	} else {  
	    uplevel [list set $i $j]
	} 
    }
}

# ::tkp::gradientstopsstyle --
# 
#       Utility function to create named example gradient definitions.
#       
# Arguments:
#       name      the name of the gradient
#       args
#       
# Results:
#       the stops list.

proc ::tkp::gradientstopsstyle {name args} {
    
    switch -- $name {
	rainbow {
	    set stops {
		{0.00 "#ff0000"} 
		{0.15 "#ff7f00"} 
		{0.30 "#ffff00"}
		{0.45 "#00ff00"}
		{0.65 "#0000ff"}
		{0.90 "#7f00ff"}
		{1.00 "#7f007f"}
	    }
	    return $stops
	}
	default {
	    return -code error "the named gradient \"$name\" is unknown"
	}
    }    
}

proc ::tkp::ellipsepath {x y rx ry} {
    return "M $x $y a $rx $ry 0 1 1 0 [expr {2*$ry}] a $rx $ry 0 1 1 0 [expr {-2*$ry}] Z"
}

proc ::tkp::circlepath {x y r} {
    return [ellipsepath $x $y $r $r]
}
 	  	 

