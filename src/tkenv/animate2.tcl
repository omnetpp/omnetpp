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
# Perform concurrent animations. Each job in the $animjobs list should
# look like this: <op> <window> <msg> <additional-args>...
#
proc do_concurrent_animations {animjobs} {
    global config

    # we should form groups -- anim requests within the group are performed
    # concurrently. group1: first request of each sending; group2: second request
    # of each sending, etc. One group may contain animations on several
    # compound modules.

    # sort jobs into jobgroups; meanwhile collect window list in $winlist()
    foreach job $animjobs {
        set w [lindex $job 1]
        if [winfo exist $w] {
            set winlist($w) {}
            set msg [lindex $job 2]
            if ![info exist msgcount($msg)] {set msgcount($msg) 0}
            incr msgcount($msg)
            set stage $msgcount($msg)
            if ![info exist jobgroup($stage)] {set jobgroup($stage) {}}
            lappend jobgroup($stage) $job
        }
    }

    # WM_DELETE_WINDOW stuff: if user wants to close window (during "update"), postpone it until updateInspectors()
    foreach w [array names winlist] {
        # remember current Close handler and temporarily replace it
        set winlist($w) [wm protocol $w WM_DELETE_WINDOW]
        wm protocol $w WM_DELETE_WINDOW [list opp_markinspectorfordeletion $w]
    }

    # then animate each group, one after another
    foreach stage [lsort -integer [array names jobgroup]] {
        do_animate_group $jobgroup($stage)
    }

    # restore old WM_DELETE_WINDOW handlers
    foreach w [array names winlist] {
        wm protocol $w WM_DELETE_WINDOW $winlist($w)
    }
}

proc do_animate_group {animjobs} {
    set beforelist {}
    set animlist {}
    set afterlist {}

    foreach job $animjobs {
        set op [lindex $job 0]
        switch $op {
            on_conn {
                setvars {cmd win msgptr gateptr mode} $job
                if {$mode=="end"} {error "internal error: mode cannot be 'end'"}

                set c $win.c

                # gate pointer string is the tag of the connection arrow
                set coords [$c coords $gateptr]

                if {$coords == ""} {
                    # connection arrow not (no longer?) on canvas: forget animation
                    $c delete $msgptr;  # this also works if msg is not (yet) on canvas
                } else {
                    setvars {x1 y1 x2 y2} $coords
                    if {$mode=="beg"} {
                        set endpos [graphmodwin_getmessageendpos $x1 $y1 $x2 $y2]
                        setvars {x2 y2} $endpos
                    }

                    lappend animlist [list $win $msgptr $x1 $y1 $x2 $y2]

                    if {$mode!="beg"} {
                       lappend afterlist [list $c delete $msgptr]
                    }
                }
            }

            senddirect_horiz {
                setvars {cmd win msgptr mod1ptr mod2ptr mode} $job
                if {$mode=="end"} {error "internal error: mode cannot be 'end'"}

                set c $win.c
                set src  [get_submod_coords $c $mod1ptr]
                set dest [get_submod_coords $c $mod2ptr]

                set x1 [expr ([lindex $src 0]+[lindex $src 2])/2]
                set y1 [expr ([lindex $src 1]+[lindex $src 3])/2]
                set x2 [expr ([lindex $dest 0]+[lindex $dest 2])/2]
                set y2 [expr ([lindex $dest 1]+[lindex $dest 3])/2]

                if [opp_getsimoption senddirect_arrows] {
                    $c create line $x1 $y1 $x2 $y2 -tags {senddirect} -arrow last -fill blue -dash {.}
                }
                lappend animlist [list $win $msgptr $x1 $y1 $x2 $y2]
                if {$mode!="beg"} {
                   lappend afterlist [list $c delete $msgptr]
                }
            }

            senddirect_ascent {
                setvars {cmd win msgptr parentmodptr modptr mode} $job
                if {$mode=="end"} {error "internal error: mode cannot be 'end'"}

                set c $win.c
                set src  [get_submod_coords $c $modptr]

                set x1 [expr ([lindex $src 0]+[lindex $src 2])/2]
                set y1 [expr ([lindex $src 1]+[lindex $src 3])/2]
                set x2 [expr $x1 + $y1/4]
                set y2 0

                if [opp_getsimoption senddirect_arrows] {
                    $c create line $x1 $y1 $x2 $y2 -tags {senddirect} -arrow last -fill blue -dash {.}
                }
                lappend animlist [list $win $msgptr $x1 $y1 $x2 $y2]
                if {$mode!="beg"} {
                   lappend afterlist [list $c delete $msgptr]
                }
            }

            senddirect_descent {
                setvars {cmd win msgptr parentmodptr modptr mode} $job
                if {$mode=="end"} {error "internal error: mode cannot be 'end'"}

                set c $win.c
                set dest [get_submod_coords $c $modptr]

                set x2 [expr ([lindex $dest 0]+[lindex $dest 2])/2]
                set y2 [expr ([lindex $dest 1]+[lindex $dest 3])/2]
                set x1 [expr $x2 - $y2/4]
                set y1 0

                if [opp_getsimoption senddirect_arrows] {
                    $c create line $x1 $y1 $x2 $y2 -tags {senddirect} -arrow last -fill blue -dash {.}
                }
                lappend animlist [list $win $msgptr $x1 $y1 $x2 $y2]
                if {$mode!="beg"} {
                   lappend afterlist [list $c delete $msgptr]
                }
            }

            senddirect_cleanup {
                set win [lindex $job 1]
                set c $win.c
                lappend afterlist [list $c delete senddirect]
            }

            default {
                error "internal error: unrecognized animation primitive: $op"
            }
        }
    }

    # finally, do the job
    foreach cmd $beforelist {eval $cmd}
    do_animate_concurrent $animlist
    foreach cmd $afterlist {eval $cmd}
}

#
# Ultimate helper function which in fact animates several messages together.
# $animlist should contain a list of anim requests, each request consisting
# of 6 elements: inspectorwindow, msgptr, x1, y1, x2, y2.
# The function animates on the canvases ($w.c) the movements of messages
# (identified by $msgptr as canvas tag) from (x1,y1) to (x2,y2).
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
        if {[llength $req]!=6} {error "wrong number of items in animreq"}
        setvars {w msgptr x1 y1 x2 y2} $req
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
        setvars {w msgptr x1 y1 x2 y2} $req
        $w.c delete $msgptr
        draw_message $w.c $msgptr $x1 $y1

        set len [expr sqrt(($x2-$x1)*($x2-$x1)+($y2-$y1)*($y2-$y1))]
        set dx [expr ($x2-$x1)/double($steps)]
        set dy [expr ($y2-$y1)/double($steps)]
        lappend movecommands [list $w.c move $msgptr $dx $dy]
    }

    # now do the animation
    for {set i 0} {$i<$steps} {incr i} {
       set tbeg [clock clicks]
       update
       foreach movecommand $movecommands {
           eval $movecommand
       }

       # note: we don't need to check whether the inspector is closing ("marked
       # to be deleted"), because with concurrent animations there aren't long waits
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
}

