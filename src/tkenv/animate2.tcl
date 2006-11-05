#=================================================================
#  ANIMATE2.TCL - part of
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


#
# Assigns variables from a list. Example: setvars {a b c} {1 2 3}
#
proc setvars {vars vals} {
    if {[llength $vars] != [llength $vals]} {error "number of vars and length of values list don't match"}
    uplevel [list foreach $vars $vals {}]
}

#
# Perform concurrent animations
#
proc do_concurrent_animations {animjobs} {
    global config

    # we should form groups -- anim requests within the group are performed
    # concurrently. group1: first request of each sending; group2: second request
    # of each sending, etc. One group may contain animations on several
    # compound modules.

    #XXX right now, we just lump everything together in a single group
    do_animate_group $animjobs
    puts "----"
}

proc do_animate_group {animjobs} {
    set animlist {}

    foreach job $animjobs {
        set op [lindex $job 0]
        puts "DOING: $job"
        switch $op {
            on_conn {
                setvars {cmd win gateptr msgptr mode} $job
                puts "==>> $cmd $win"

                if {$mode!="end"} {
                    set c $win.c

                    # gate pointer string is the tag of the connection arrow
                    set coords [$c coords $gateptr]

                    #if {$coords == ""} {
                    #    # connection arrow not (no longer?) on canvas: forget animation
                    #    $c delete $msgptr;  # this also works if msg is not (yet) on canvas
                    #    return;
                    #}

                    setvars {x1 y1 x2 y2} $coords
                    lappend animlist [list $win $msgptr $x1 $y1 $x2 $y2 1]

                    #if {$mode!="beg"} {
                    #   $c delete $msgptr
                    #}
                }
            }

            senddirect_horiz {
                setvars {cmd win mod1ptr mod2ptr msgptr mode} $job
                set c $win.c
                set src  [get_submod_coords $c $mod1ptr]
                set dest [get_submod_coords $c $mod2ptr]

                set x1 [expr ([lindex $src 0]+[lindex $src 2])/2]
                set y1 [expr ([lindex $src 1]+[lindex $src 3])/2]
                set x2 [expr ([lindex $dest 0]+[lindex $dest 2])/2]
                set y2 [expr ([lindex $dest 1]+[lindex $dest 3])/2]

                lappend animlist [list $win $msgptr $x1 $y1 $x2 $y2 1]
            }

            senddirect_ascent {
            }

            senddirect_descent {
            }

            senddirect_delivery {
            }

            senddirect_cleanup {
            }

            default {
                error "internal error: unrecognized animation primitive: $op"
            }
        }
    }

    do_animate_concurrent $animlist
}

proc animate_on_conn {win gateptr msgptr mode} {
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


proc animate_senddirect_horiz {win mod1ptr mod2ptr msgptr mode} {
    set c $win.c
    set src  [get_submod_coords $c $mod1ptr]
    set dest [get_submod_coords $c $mod2ptr]

    set x1 [expr ([lindex $src 0]+[lindex $src 2])/2]
    set y1 [expr ([lindex $src 1]+[lindex $src 3])/2]
    set x2 [expr ([lindex $dest 0]+[lindex $dest 2])/2]
    set y2 [expr ([lindex $dest 1]+[lindex $dest 3])/2]

    graphmodwin_do_animate_senddirect $win $x1 $y1 $x2 $y2 $msgptr $mode
}

proc animate_senddirect_ascent {win parentmodptr modptr msgptr mode} {
    set c $win.c
    set src  [get_submod_coords $c $modptr]

    set x1 [expr ([lindex $src 0]+[lindex $src 2])/2]
    set y1 [expr ([lindex $src 1]+[lindex $src 3])/2]
    set x2 [expr $x1 + $y1/4]
    set y2 0

    graphmodwin_do_animate_senddirect $win $x1 $y1 $x2 $y2 $msgptr $mode
}

proc animate_senddirect_descent {win parentmodptr modptr msgptr mode} {
    set c $win.c
    set dest [get_submod_coords $c $modptr]

    set x2 [expr ([lindex $dest 0]+[lindex $dest 2])/2]
    set y2 [expr ([lindex $dest 1]+[lindex $dest 3])/2]
    set x1 [expr $x2 - $y2/4]
    set y1 0

    graphmodwin_do_animate_senddirect $win $x1 $y1 $x2 $y2 $msgptr $mode
}

proc animate_senddirect_delivery {win modptr msgptr} {
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

proc do_animate_senddirect {win x1 y1 x2 y2 msgptr mode} {
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
# Ultimate helper function which in fact animates several messages together.
# $animlist should contain a list of anim requests, each request consisting
# of 6 elements: inspectorwindow, msgptr, x1, y1, x2, y2, delflag.
# The function animates on the canvases ($w.c) the movements of messages
# (identified by $msgptr as canvas tag) from (x1,y1) to (x2,y2).
# If delflag is true, the message gets removed from the canvas afterwards
#
# Example animlist:
#  set animlist [list
#     [list $w1 $msg1 10 10  500 10 ]
#     [list $w1 $msg2 10 20  300 20]
#     [list $w2 $msg3 10 30  50 300]
#     [list $w2 $msg4 10 40  20  50]
#  ]
#
proc do_animate_concurrent {animlist} {
    global clicksPerSec

    # calculate maxlen -- that will determine numsteps
    set maxlen 0
    foreach req $animlist {
        if {[llength $req]!=7} {error "wrong number of items in animreq"}
        setvars {w msgptr x1 y1 x2 y2 delflag} $req
        set len [expr sqrt(($x2-$x1)*($x2-$x1)+($y2-$y1)*($y2-$y1))]
        if {$len>$maxlen} {set maxlen $len}
    }

    # max 100 steps (otherwise animation takes forever!)
    set steps [expr int($maxlen/2)]
    if {$steps>100} {set steps 100}
    if {$steps==0} {set steps 1}

    # calculate dx,dy pairs and assemble commands from it
    set movecommands {}
    foreach req $animlist {
        setvars {w msgptr x1 y1 x2 y2 delflag} $req
        $w.c delete $msgptr
        draw_message $w.c $msgptr $x1 $y1

        set len [expr sqrt(($x2-$x1)*($x2-$x1)+($y2-$y1)*($y2-$y1))]
        set dx [expr ($x2-$x1)/double($steps)]
        set dy [expr ($y2-$y1)/double($steps)]
        lappend movecommands [list $w.c move $msgptr $dx $dy]
    }

    # WM_DELETE_WINDOW stuff: if user wants to close window (during "update"), postpone it until updateInspectors()
    #FIXME ALL WINDOWS!!!!! set old_close_handler [wm protocol $win WM_DELETE_WINDOW]
    #wm protocol $win WM_DELETE_WINDOW [list opp_markinspectorfordeletion $win]
    for {set i 0} {$i<$steps} {incr i} {
       set tbeg [clock clicks]
       update
       foreach movecommand $movecommands {
           eval $movecommand
       }

       # FIXME && ! someinspmarkedfordeletion
       if {![opp_simulationisstopping]} {
           set sp [opp_getsimoption animation_speed]
           if {$sp>3} {set $sp 3}
           if {$sp<0} {set $sp 0}
           # delay has the form of f(x) = c/(x-a)+b, where f(3)=0, f(1)=~1, f(0)=~6
           set d [expr 2/($sp+0.3)-0.6]
           set clicks [expr $d*$clicksPerSec*0.04]
           while {[expr abs([clock clicks]-$tbeg)] < $clicks} {}
       }
    }
    #wm protocol $win WM_DELETE_WINDOW $old_close_handler

    foreach req $animlist {
        setvars {w msgptr x1 y1 x2 y2 delflag} $req
        if {$delflag} {
            $w.c delete $msgptr
        }
    }
}

