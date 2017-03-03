#=================================================================
#  ANIMATE.TCL - part of
#
#                     OMNeT++/OMNEST
#            Discrete System Simulation in C++
#
#=================================================================

#----------------------------------------------------------------#
#  Copyright (C) 1992-2017 Andras Varga
#
#  This file is distributed WITHOUT ANY WARRANTY. See the file
#  `license' for details on this and other legal matters.
#----------------------------------------------------------------#

set tkenv(animjobs) {}

#
# Called from C++ code. $mode="beg"/"thru"/"end".
#
proc ModuleInspector:animateOnConn {insp msgptr gateptr mode} {
    global config tkenv
    # Note on $mode!="end" condition: "end" equals to delivery of msg to
    # the module. It is called *before* processing the event, so it must be
    # animated immediately, regardless of $config(concurrent-anim).
    if {$mode!="end" && $config(concurrent-anim)} {
        # if concurrent-anim is ON, we just store the params here, and will execute inside performAnimations.
        animRememberMsg $msgptr
        lappend tkenv(animjobs) [list on_conn $insp $msgptr $gateptr $mode]
        return
    }

    set c $insp.c

    # gate pointer string is the tag of the connection arrow
    set coords [$c coords $gateptr]

    if {$coords == ""} {
        # connection arrow not (no longer?) on canvas: forget animation
        $c delete $msgptr;  # this also works if msg is not (yet) on canvas
        return;
    }

    setvars {x1 y1 x2 y2} $coords
    ModuleInspector:doAnimate $insp $x1 $y1 $x2 $y2 $msgptr $mode

    if {$mode!="beg"} {
       $c delete $msgptr
    }
}


#
# Called from C++ code. $mode="beg"/"thru"/"end".
#
proc ModuleInspector:animateSenddirectHoriz {insp msgptr mod1ptr mod2ptr mode} {
    global config tkenv
    if {$config(concurrent-anim)} {
        # if concurrent-anim is ON, we just store the params here, and will execute inside performAnimations.
        animRememberMsg $msgptr
        lappend tkenv(animjobs) [list senddirect_horiz $insp $msgptr $mod1ptr $mod2ptr $mode]
        return
    }

    set c $insp.c
    set src  [ModuleInspector:getSubmodCoords $c $mod1ptr]
    set dest [ModuleInspector:getSubmodCoords $c $mod2ptr]

    set x1 [expr ([lindex $src 0]+[lindex $src 2])/2]
    set y1 [expr ([lindex $src 1]+[lindex $src 3])/2]
    set x2 [expr ([lindex $dest 0]+[lindex $dest 2])/2]
    set y2 [expr ([lindex $dest 1]+[lindex $dest 3])/2]

    ModuleInspector:doAnimateSenddirect $insp $x1 $y1 $x2 $y2 $msgptr $mode
}


#
# Called from C++ code. $mode="beg"/"thru"/"end".
#
proc ModuleInspector:animateSenddirectAscent {insp msgptr parentmodptr modptr mode} {
    global config tkenv
    if {$config(concurrent-anim)} {
        # if concurrent-anim is ON, we just store the params here, and will execute inside performAnimations.
        animRememberMsg $msgptr
        lappend tkenv(animjobs) [list senddirect_ascent $insp $msgptr $parentmodptr $modptr $mode]
        return
    }

    set c $insp.c
    set src  [ModuleInspector:getSubmodCoords $c $modptr]

    set x1 [expr ([lindex $src 0]+[lindex $src 2])/2]
    set y1 [expr ([lindex $src 1]+[lindex $src 3])/2]
    set x2 [expr $x1 + $y1/4]
    set y2 0

    ModuleInspector:doAnimateSenddirect $insp $x1 $y1 $x2 $y2 $msgptr $mode
}


#
# Called from C++ code. $mode="beg"/"thru"/"end".
#
proc ModuleInspector:animateSenddirectDescent {insp msgptr parentmodptr modptr mode} {
    global config tkenv
    if {$config(concurrent-anim)} {
        # if concurrent-anim is ON, we just store the params here, and will execute inside performAnimations.
        animRememberMsg $msgptr
        lappend tkenv(animjobs) [list senddirect_descent $insp $msgptr $parentmodptr $modptr $mode]
        return
    }

    set c $insp.c
    set dest [ModuleInspector:getSubmodCoords $c $modptr]

    set x2 [expr ([lindex $dest 0]+[lindex $dest 2])/2]
    set y2 [expr ([lindex $dest 1]+[lindex $dest 3])/2]
    set x1 [expr $x2 - $y2/4]
    set y1 0

    ModuleInspector:doAnimateSenddirect $insp $x1 $y1 $x2 $y2 $msgptr $mode
}

#
# Remember properties of the message so we can perform the "send" animation
# even if the message object has been deleted by then
#
proc animRememberMsg {msgptr} {
    global anim_msg

    if {![info exists anim_msg($msgptr:name)]} {
        set anim_msg($msgptr:name) [opp_getobjectfullname $msgptr]
        set anim_msg($msgptr:type) [opp_getobjectshorttypename $msgptr]
        set anim_msg($msgptr:kind) [opp_getobjectfield $msgptr kind]
        set anim_msg($msgptr:disp) [opp_getobjectfield $msgptr displayString]
    }
}

proc animDisableClose {insp} {
    if [opp_inspector_istoplevel $insp] {
        global savedCloseHandlers
        set savedCloseHandlers($insp) [wm protocol $insp WM_DELETE_WINDOW]
        wm protocol $insp WM_DELETE_WINDOW [list opp_markinspectorfordeletion $insp]
    }
}

proc animRestoreClose {insp} {
    if [opp_inspector_istoplevel $insp] {
        global savedCloseHandlers
        wm protocol $insp WM_DELETE_WINDOW $savedCloseHandlers($insp)
    }
}

#
# Called from C++ code.
#
proc ModuleInspector:animateSenddirectDelivery {insp msgptr modptr} {
    # Note: delivery is called *before* processing the event, so it must be
    # animated immediately, regardless of $config(concurrent-anim).

    set c $insp.c
    set src  [ModuleInspector:getSubmodCoords $c $modptr]

    # flash the message a few times before removing it
    # WM_DELETE_WINDOW stuff: if user wants to close window (during "update"), postpone it until updateInspectors()
    animDisableClose $insp
    for {set i 0} {$i<3} {incr i} {
       $c itemconfig $msgptr -state hidden
       update
       animFlashingDelay $insp 0.05
       $c itemconfig $msgptr -state normal
       update
       animFlashingDelay $insp 0.05
    }
    animRestoreClose $insp

    $c delete $msgptr
}

proc animFlashingDelay {insp d} {
    global clicksPerSec
    if {![opp_simulationisstopping] && ![opp_inspmarkedfordeletion $insp]} {
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
proc ModuleInspector:doAnimateSenddirect {insp x1 y1 x2 y2 msgptr mode} {
    set c $insp.c

    if [opp_getsimoption senddirect_arrows] {
        $c create pline $x1 $y1 $x2 $y2 -tags {senddirect} -endarrow 1 -stroke blue -strokedasharray {1 1}
        ModuleInspector:doAnimate $insp $x1 $y1 $x2 $y2 $msgptr "thru"
        #$c delete $arrow -- this will come in _cleanup
    } else {
        ModuleInspector:doAnimate $insp $x1 $y1 $x2 $y2 $msgptr "thru"
    }
    if {$mode!="beg"} {
       $c delete $msgptr
    }
}


#
# Ultimate helper function which in fact performs the animation.
#
proc ModuleInspector:doAnimate {insp x1 y1 x2 y2 msgptr {mode thru}} {
    global clicksPerSec
    set c $insp.c

    # remove "phantom messages" if any
    $c delete $msgptr

    # msg will travel at constant speed: $steps proportional to length
    set len [expr sqrt(($x2-$x1)*($x2-$x1)+($y2-$y1)*($y2-$y1))]
    set steps [expr int($len/2)]

    # max 100 steps (otherwise animation takes forever!)
    if {$steps>100} {set steps 100}
    if {[string equal [tk windowingsystem] aqua]}  {
        # we have max 60 frames/sec due to "update" being vsync'd, so we do fewer $steps to be able to keep up
        set initialspeed [opp_getsimoption animation_speed]
        set steps [expr $steps/2.0/$initialspeed]
    }
    if {$steps==0} {set steps 1}

    if {$mode=="beg"} {
        set endpos [ModuleInspector:getMessageEndPos $x1 $y1 $x2 $y2]
        setvars {x2 y2} $endpos
    }
    if {$mode=="end"} {
        set endpos [ModuleInspector:getMessageEndPos $x1 $y1 $x2 $y2]
        setvars {x1 y1} $endpos
        set steps 6
    }

    ModuleInspector:drawMessage $c $msgptr $x1 $y1

    set dx [expr ($x2-$x1)/double($steps)]
    set dy [expr ($y2-$y1)/double($steps)]

    # WM_DELETE_WINDOW stuff: if user wants to close window (during "update"), postpone it until updateInspectors()
    animDisableClose $insp
    for {set i 0} {$i<$steps} {incr i} {
       set tbeg [clock clicks]
       update
       $c move $msgptr $dx $dy
       if {![opp_simulationisstopping] && ![opp_inspmarkedfordeletion $insp]} {
           set sp [opp_getsimoption animation_speed]
           if {$sp>3} {set $sp 3}
           if {$sp<0} {set $sp 0}
           # delay has the form of f(x) = c/(x-a)+b, where f(3)=0, f(1)=~1, f(0)=~6
           set d [expr 2/($sp+0.3)-0.6]
           set clicks [expr $d*$clicksPerSec*0.04]
           while {[expr abs([clock clicks]-$tbeg)] < $clicks} {}
       }
    }
    animRestoreClose $insp
}

#
# This function is invoked from the module inspector C++ code.
# Removes all dashed arrows at the end of a senddirect animation.
#
proc ModuleInspector:animateSenddirectCleanup {insp} {
    global config tkenv
    if {$config(concurrent-anim)} {
        # if concurrent-anim is ON, we just store the params here, and will execute inside performAnimations.
        lappend tkenv(animjobs) [list senddirect_cleanup $insp]
        return
    }

    set c $insp.c
    $c delete senddirect
}


# ModuleInspector:animateMethodcallAscent --
#
# This function is invoked from the module inspector C++ code.
#
proc ModuleInspector:animateMethodcallAscent {insp parentmodptr modptr methodlabel} {
    set c $insp.c
    set src  [ModuleInspector:getSubmodCoords $c $modptr]

    set x1 [expr ([lindex $src 0]+[lindex $src 2])/2]
    set y1 [expr ([lindex $src 1]+[lindex $src 3])/2]
    set x2 [expr $x1 + $y1/4]
    set y2 0
    ModuleInspector:doDrawMethodcall $insp $x1 $y1 $x2 $y2 $methodlabel
}

# ModuleInspector:animateMethodcallDescent --
#
# This function is invoked from the module inspector C++ code.
#
proc ModuleInspector:animateMethodcallDescent {insp parentmodptr modptr methodlabel} {
    set c $insp.c
    set dest [ModuleInspector:getSubmodCoords $c $modptr]

    set x2 [expr ([lindex $dest 0]+[lindex $dest 2])/2]
    set y2 [expr ([lindex $dest 1]+[lindex $dest 3])/2]
    set x1 [expr $x2 - $y2/4]
    set y1 0
    ModuleInspector:doDrawMethodcall $insp $x1 $y1 $x2 $y2 $methodlabel
}

# ModuleInspector:animateMethodcallHoriz --
#
# This function is invoked from the module inspector C++ code.
#
proc ModuleInspector:animateMethodcallHoriz {insp fromptr toptr methodlabel} {
    set c $insp.c
    set src  [ModuleInspector:getSubmodCoords $c $fromptr]
    set dest [ModuleInspector:getSubmodCoords $c $toptr]

    set x1 [expr ([lindex $src 0]+[lindex $src 2])/2]
    set y1 [expr ([lindex $src 1]+[lindex $src 3])/2]
    set x2 [expr ([lindex $dest 0]+[lindex $dest 2])/2]
    set y2 [expr ([lindex $dest 1]+[lindex $dest 3])/2]
    ModuleInspector:doDrawMethodcall $insp $x1 $y1 $x2 $y2 $methodlabel
}

# ModuleInspector:doDrawMethodcall --
#
# Helper.
#
proc ModuleInspector:doDrawMethodcall {insp x1 y1 x2 y2 methodlabel} {
    global tkpFont

    set c $insp.c
    set arrow [$c create pline $x1 $y1 $x2 $y2 -tags {methodcall} -strokedasharray {2 2} -endarrow 1 -stroke red]

    set x [expr ($x1+$x2)/2]
    set y [expr ($y1+$y2)/2]
    set txtid  [$c create ptext $x $y -tags {methodcall} -text " $methodlabel " -textanchor c {*}$tkpFont(CanvasFont)]
    set color #F0F0F0
    #set color #F0F0D0
    #catch {set color [$c itemcget mod -fill]}
    set rectid [$c create prect [$c bbox $txtid] -tags {methodcall} -stroke "" -fill $color]
    $c lower $rectid $txtid

    # flash arrow a bit
    # WM_DELETE_WINDOW stuff: if user wants to close window (during "update"), postpone it until updateInspectors()
    animDisableClose $insp
    for {set i 0} {$i<2} {incr i} {
       $c itemconfig $arrow -state hidden
       update
       animFlashingDelay $insp 0.3
       $c itemconfig $arrow -state normal
       update
       animFlashingDelay $insp 0.3
    }
    animRestoreClose $insp
}

# ModuleInspector:animateMethodcallWait --
#
# This function is invoked from the module inspector C++ code.
#
proc ModuleInspector:animateMethodcallWait {} {
    update idletasks
    set d [opp_getsimoption methodcalls_delay]
    after $d
}

# ModuleInspector:animateMethodcallCleanup --
#
# This function is invoked from the module inspector C++ code.
#
proc ModuleInspector:animateMethodcallCleanup {insp} {
    set c $insp.c
    $c delete methodcall
}

proc determineClocksPerSec {} {
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
proc performAnimations {} {
    global config tkenv anim_msg
    if {$config(concurrent-anim)} {
        doConcurrentAnimations $tkenv(animjobs)
        set tkenv(animjobs) {}
        array unset anim_msg
    }
}


