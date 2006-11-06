#=================================================================
#  ANIMATE.TCL - part of
#
#                     OMNeT++/OMNEST
#            Discrete System Simulation in C++
#
#=================================================================

#----------------------------------------------------------------#
#  Copyright (C) 1992-2005 Andras Varga
#
#  This file is distributed WITHOUT ANY WARRANTY. See the file
#  `license' for details on this and other legal matters.
#----------------------------------------------------------------#

set tkenv(animjobs) {}

#
# Called from C++ code. $mode="beg"/"thru"/"end".
#
proc graphmodwin_animate_on_conn {win msgptr gateptr mode} {
    global config tkenv
    # Note on $mode!="end" condition: "end" equals to delivery of msg to
    # the module. It is called *before* processing the event, so it must be
    # animated immediately, regardless of $config(concurrent-anim).
    if {$mode!="end" && $config(concurrent-anim)} {
        # if concurrent-anim is ON, we just store the params here, and will execute inside perform_animations.
        lappend tkenv(animjobs) [list on_conn $win $msgptr $gateptr $mode]
        return
    }

    set c $win.c

    # gate pointer string is the tag of the connection arrow
    set coords [$c coords $gateptr]

    if {$coords == ""} {
        # connection arrow not (no longer?) on canvas: forget animation
        $c delete $msgptr;  # this also works if msg is not (yet) on canvas
        return;
    }

    setvars {x1 y1 x2 y2} $coords
    graphmodwin_do_animate $win $x1 $y1 $x2 $y2 $msgptr $mode

    if {$mode!="beg"} {
       $c delete $msgptr
    }
}


#
# Called from C++ code. $mode="beg"/"thru"/"end".
#
proc graphmodwin_animate_senddirect_horiz {win msgptr mod1ptr mod2ptr mode} {
    global config tkenv
    if {$config(concurrent-anim)} {
        # if concurrent-anim is ON, we just store the params here, and will execute inside perform_animations.
        lappend tkenv(animjobs) [list senddirect_horiz $win $msgptr $mod1ptr $mod2ptr $mode]
        return
    }

    set c $win.c
    set src  [get_submod_coords $c $mod1ptr]
    set dest [get_submod_coords $c $mod2ptr]

    set x1 [expr ([lindex $src 0]+[lindex $src 2])/2]
    set y1 [expr ([lindex $src 1]+[lindex $src 3])/2]
    set x2 [expr ([lindex $dest 0]+[lindex $dest 2])/2]
    set y2 [expr ([lindex $dest 1]+[lindex $dest 3])/2]

    graphmodwin_do_animate_senddirect $win $x1 $y1 $x2 $y2 $msgptr $mode
}


#
# Called from C++ code. $mode="beg"/"thru"/"end".
#
proc graphmodwin_animate_senddirect_ascent {win msgptr parentmodptr modptr mode} {
    global config tkenv
    if {$config(concurrent-anim)} {
        # if concurrent-anim is ON, we just store the params here, and will execute inside perform_animations.
        lappend tkenv(animjobs) [list senddirect_ascent $win $msgptr $parentmodptr $modptr $mode]
        return
    }

    set c $win.c
    set src  [get_submod_coords $c $modptr]

    set x1 [expr ([lindex $src 0]+[lindex $src 2])/2]
    set y1 [expr ([lindex $src 1]+[lindex $src 3])/2]
    set x2 [expr $x1 + $y1/4]
    set y2 0

    graphmodwin_do_animate_senddirect $win $x1 $y1 $x2 $y2 $msgptr $mode
}


#
# Called from C++ code. $mode="beg"/"thru"/"end".
#
proc graphmodwin_animate_senddirect_descent {win msgptr parentmodptr modptr mode} {
    global config tkenv
    if {$config(concurrent-anim)} {
        # if concurrent-anim is ON, we just store the params here, and will execute inside perform_animations.
        lappend tkenv(animjobs) [list senddirect_descent $win $msgptr $parentmodptr $modptr $mode]
        return
    }

    set c $win.c
    set dest [get_submod_coords $c $modptr]

    set x2 [expr ([lindex $dest 0]+[lindex $dest 2])/2]
    set y2 [expr ([lindex $dest 1]+[lindex $dest 3])/2]
    set x1 [expr $x2 - $y2/4]
    set y1 0

    graphmodwin_do_animate_senddirect $win $x1 $y1 $x2 $y2 $msgptr $mode
}


#
# Called from C++ code.
#
proc graphmodwin_animate_senddirect_delivery {win msgptr modptr} {
    # Note: delivery is called *before* processing the event, so it must be
    # animated immediately, regardless of $config(concurrent-anim).

    set c $win.c
    set src  [get_submod_coords $c $modptr]

    # flash the message a few times before removing it
    # WM_DELETE_WINDOW stuff: if user wants to close window (during "update"), postpone it until updateInspectors()
    set old_close_handler [wm protocol $win WM_DELETE_WINDOW]
    wm protocol $win WM_DELETE_WINDOW [list opp_markinspectorfordeletion $win]
    for {set i 0} {$i<3} {incr i} {
       $c itemconfig $msgptr -state hidden
       update
       anim_flashing_delay $win 0.05
       $c itemconfig $msgptr -state normal
       update
       anim_flashing_delay $win 0.05
    }
    wm protocol $win WM_DELETE_WINDOW $old_close_handler

    $c delete $msgptr
}

proc anim_flashing_delay {win d} {
    global clicksPerSec
    if {![opp_simulationisstopping] && ![opp_inspmarkedfordeletion $win]} {
        set tbeg [clock clicks]
        set sp [opp_getsimoption animation_speed]
        if {$sp>3} {set $sp 3}
        if {$sp<0} {set $sp 0}
        set clicks [expr (3-$sp)*$clicksPerSec*$d]
        while {[expr abs([clock clicks]-$tbeg)] < $clicks} {}
    }
}

#
# Helper for senddirect animations
#
proc graphmodwin_do_animate_senddirect {win x1 y1 x2 y2 msgptr mode} {
    set c $win.c

    if [opp_getsimoption senddirect_arrows] {
        #$c create line $x1 $y1 $x2 $y2 -tags {senddirect} -arrow last -fill gray
        $c create line $x1 $y1 $x2 $y2 -tags {senddirect} -arrow last -fill blue -dash {.}
        graphmodwin_do_animate $win $x1 $y1 $x2 $y2 $msgptr "thru"
        #$c delete $arrow -- this will come in _cleanup
    } else {
        graphmodwin_do_animate $win $x1 $y1 $x2 $y2 $msgptr "thru"
    }
    if {$mode!="beg"} {
       $c delete $msgptr
    }
}


#
# Ultimate helper function which in fact performs the animation.
#
proc graphmodwin_do_animate {win x1 y1 x2 y2 msgptr {mode thru}} {
    global fonts clicksPerSec
    set c $win.c

    # remove "phantom messages" if any
    $c delete $msgptr

    # msg will travel at constant speed: $steps proportional to length
    set len [expr sqrt(($x2-$x1)*($x2-$x1)+($y2-$y1)*($y2-$y1))]
    set steps [expr int($len/2)]

    # max 100 steps (otherwise animation takes forever!)
    if {$steps>100} {set steps 100}
    if {$steps==0} {set steps 1}

    if {$mode=="beg"} {
        set endpos [graphmodwin_getmessageendpos $x1 $y1 $x2 $y2]
        setvars {x2 y2} $endpos
    }
    if {$mode=="end"} {
        set endpos [graphmodwin_getmessageendpos $x1 $y1 $x2 $y2]
        setvars {x1 y1} $endpos
        set steps 6
    }

    draw_message $c $msgptr $x1 $y1

    set dx [expr ($x2-$x1)/double($steps)]
    set dy [expr ($y2-$y1)/double($steps)]

    # WM_DELETE_WINDOW stuff: if user wants to close window (during "update"), postpone it until updateInspectors()
    set old_close_handler [wm protocol $win WM_DELETE_WINDOW]
    wm protocol $win WM_DELETE_WINDOW [list opp_markinspectorfordeletion $win]
    for {set i 0} {$i<$steps} {incr i} {
       set tbeg [clock clicks]
       update
       $c move $msgptr $dx $dy
       if {![opp_simulationisstopping] && ![opp_inspmarkedfordeletion $win]} {
           set sp [opp_getsimoption animation_speed]
           if {$sp>3} {set $sp 3}
           if {$sp<0} {set $sp 0}
           # delay has the form of f(x) = c/(x-a)+b, where f(3)=0, f(1)=~1, f(0)=~6
           set d [expr 2/($sp+0.3)-0.6]
           set clicks [expr $d*$clicksPerSec*0.04]
           while {[expr abs([clock clicks]-$tbeg)] < $clicks} {}
       }
    }
    wm protocol $win WM_DELETE_WINDOW $old_close_handler
}

#
# This function is invoked from the module inspector C++ code.
# Removes all dashed arrows at the end of a senddirect animation.
#
proc graphmodwin_animate_senddirect_cleanup {win} {
    global config tkenv
    if {$config(concurrent-anim)} {
        # if concurrent-anim is ON, we just store the params here, and will execute inside perform_animations.
        lappend tkenv(animjobs) [list senddirect_cleanup $win]
        return
    }

    set c $win.c
    $c delete senddirect
}


# graphmodwin_animate_methodcall_ascent --
#
# This function is invoked from the module inspector C++ code.
#
proc graphmodwin_animate_methodcall_ascent {win parentmodptr modptr methodlabel} {
    set c $win.c
    set src  [get_submod_coords $c $modptr]

    set x1 [expr ([lindex $src 0]+[lindex $src 2])/2]
    set y1 [expr ([lindex $src 1]+[lindex $src 3])/2]
    set x2 [expr $x1 + $y1/4]
    set y2 0
    graphmodwin_do_draw_methodcall $win $x1 $y1 $x2 $y2 $methodlabel
}

# graphmodwin_animate_methodcall_descent --
#
# This function is invoked from the module inspector C++ code.
#
proc graphmodwin_animate_methodcall_descent {win parentmodptr modptr methodlabel} {
    set c $win.c
    set dest [get_submod_coords $c $modptr]

    set x2 [expr ([lindex $dest 0]+[lindex $dest 2])/2]
    set y2 [expr ([lindex $dest 1]+[lindex $dest 3])/2]
    set x1 [expr $x2 - $y2/4]
    set y1 0
    graphmodwin_do_draw_methodcall $win $x1 $y1 $x2 $y2 $methodlabel
}

# graphmodwin_animate_methodcall_horiz --
#
# This function is invoked from the module inspector C++ code.
#
proc graphmodwin_animate_methodcall_horiz {win fromptr toptr methodlabel} {
    set c $win.c
    set src  [get_submod_coords $c $fromptr]
    set dest [get_submod_coords $c $toptr]

    set x1 [expr ([lindex $src 0]+[lindex $src 2])/2]
    set y1 [expr ([lindex $src 1]+[lindex $src 3])/2]
    set x2 [expr ([lindex $dest 0]+[lindex $dest 2])/2]
    set y2 [expr ([lindex $dest 1]+[lindex $dest 3])/2]
    graphmodwin_do_draw_methodcall $win $x1 $y1 $x2 $y2 $methodlabel
}

# graphmodwin_do_draw_methodcall --
#
# Helper.
#
proc graphmodwin_do_draw_methodcall {win x1 y1 x2 y2 methodlabel} {

    set c $win.c
    #set arrow [$c create line $x1 $y1 $x2 $y2 -tags {methodcall} -width 2 -arrow last -arrowshape {15 20 6} -fill #808080]
    set arrow [$c create line $x1 $y1 $x2 $y2 -tags {methodcall}  -dash {-} -arrow last -fill red]

    set x [expr ($x1+$x2)/2]
    set y [expr ($y1+$y2)/2]
    set txtid  [$c create text $x $y -tags {methodcall} -text " $methodlabel " -anchor c]
    set color #F0F0F0
    #set color #F0F0D0
    #catch {set color [$c itemcget mod -fill]}
    set rectid [$c create rect [$c bbox $txtid] -tags {methodcall} -outline "" -fill $color]
    $c lower $rectid $txtid

    # flash arrow a bit
    # WM_DELETE_WINDOW stuff: if user wants to close window (during "update"), postpone it until updateInspectors()
    set old_close_handler [wm protocol $win WM_DELETE_WINDOW]
    wm protocol $win WM_DELETE_WINDOW [list opp_markinspectorfordeletion $win]
    for {set i 0} {$i<2} {incr i} {
       $c itemconfig $arrow -state hidden
       update
       anim_flashing_delay $win 0.3
       $c itemconfig $arrow -state normal
       update
       anim_flashing_delay $win 0.3
    }
    wm protocol $win WM_DELETE_WINDOW $old_close_handler
}

# graphmodwin_animate_methodcall_wait --
#
# This function is invoked from the module inspector C++ code.
#
proc graphmodwin_animate_methodcall_wait {} {
    update idletasks
    set d [opp_getsimoption methodcalls_delay]
    after $d
}

# graphmodwin_animate_methodcall_cleanup --
#
# This function is invoked from the module inspector C++ code.
#
proc graphmodwin_animate_methodcall_cleanup {win} {
    set c $win.c
    $c delete methodcall
}

proc determine_clocks_per_sec {} {
    global clicksPerSec

    # if counter wraps during measurement, try it again
    while 1 {
        set tbeg [clock clicks]
        after 200
        set tend [clock clicks]
        if [expr $tend>$tbeg] break;
    }
    set clicksPerSec [expr 5*($tend-$tbeg)]
    #puts "Ticks per second: $clicksPerSec"
}

proc animate2:move {c ball dx dy i} {
    $c move $ball $dx $dy
    update idletasks
    incr i -1
    if {$i>0} {
       after 10 "animate2:move $c $ball $dx $dy $i"
    } else {
       $c delete $ball
       global done$c$ball
       set done$c$ball 1
    }
}

#
# Called from C++ code
#
proc perform_animations {} {
    global config tkenv
    if {$config(concurrent-anim)} {
        do_concurrent_animations $tkenv(animjobs)
        set tkenv(animjobs) {}
    }
}


