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
# Perform concurrent animations
#
proc do_concurrent_animations {animjobs} {
    global config

    foreach job $animjobs {
        puts "DOING: $job"
        #set config(concurrent-anim) 0
        #eval $job
        #set config(concurrent-anim) 1
    }

    # we should form groups -- anim requests within the group are performed
    # concurrently. group1: first request of each sending; group2: second request
    # of each sending, etc. One group may contain animations on several
    # compound modules.

    #XXX right now, we just lump everything together in a single group
    do_animate_group $animjobs
    puts "----"
}

proc do_animate_group {animjobs} {
    # nothing for now
}


#
# Ultimate helper function which in fact animates several messages together.
# $animlist should contain a list of anim requests, each request consisting
# of 6 elements: canvas, tag, x1, x2, y1, y2. The function animates
# on the canvases the movements of canvas items identified by tag
# from (x1,y1) to (x2,y2).
#
# Example animlist:
#  set animlist {
#     {.c1 tag1 10 10  500 10}
#     {.c1 tag2 10 20  300 20}
#     {.c1 tag3 10 30  50 300}
#     {.c1 tag4 10 40  20  50}
#  }
#
proc do_animate_concurrent {animlist} {
    global fonts clicksPerSec

    foreach req $animlist {
        foreach {c tag x1 y1 x2 y2} $req {}
        set ball [$c create oval -5 -5 5 5 -fill red -outline red -tags $tag]
        $c move $ball $x1 $y1
    }

    # calculate maxlen -- that will determine numsteps
    set maxlen 0
    foreach req $animlist {
        if {[llength $req]!=6} {error "wrong number of items in animreq"}
        foreach {c tag x1 y1 x2 y2} $req {}
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
        foreach {c tag x1 y1 x2 y2} $req {}
        set len [expr sqrt(($x2-$x1)*($x2-$x1)+($y2-$y1)*($y2-$y1))]
        set dx [expr ($x2-$x1)/double($steps)]
        set dy [expr ($y2-$y1)/double($steps)]
        lappend movecommands [list $c move $tag $dx $dy]
    }

    # WM_DELETE_WINDOW stuff: if user wants to close window (during "update"), postpone it until updateInspectors()
    set old_close_handler [wm protocol $win WM_DELETE_WINDOW]
    wm protocol $win WM_DELETE_WINDOW [list opp_markinspectorfordeletion $win]
    for {set i 0} {$i<$steps} {incr i} {
       set tbeg [clock clicks]
       update
       foreach movecommand $movecommands {
           eval $movecommand
       }

       #FIXME TODO:  && ![opp_inspmarkedfordeletion $win]
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
    wm protocol $win WM_DELETE_WINDOW $old_close_handler
}

