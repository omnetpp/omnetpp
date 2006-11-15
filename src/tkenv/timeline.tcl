#=================================================================
#  TIMELINE.TCL - part of
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

set tkenv(timeline-minexp) -1
set tkenv(timeline-maxexp) +1

proc redraw_timeline {} {
    global fonts tkenv config widgets

    # spare work if we're not displayed
    if {$config(display-timeline)==0} {return}

    set c $widgets(timeline)

    # sort the FES and adjust display range
    set minexp $tkenv(timeline-minexp)
    set maxexp $tkenv(timeline-maxexp)

    set fesrange [opp_sortfesandgetrange]
    set fesmin [lindex $fesrange 0]
    set fesmax [lindex $fesrange 1]
        if [expr $fesmin!=0 && $fesmax!=0] {
        set fesminexp [expr int(floor(log10($fesmin)))]
        set fesmaxexp [expr int(ceil(log10($fesmax)))]
        if {$fesminexp < $minexp && $fesminexp > -10} {set minexp $fesminexp}
        if {$fesmaxexp > $maxexp && $fesmaxexp < 10} {set maxexp $fesmaxexp}
    }
    set tkenv(timeline-minexp) $minexp
    set tkenv(timeline-maxexp) $maxexp

    # start drawing
    $c delete all

    # draw axis
    set w [winfo width $c]
    incr w -10
    $c create line 20 29 $w 29 -arrow last -fill black -width 1
    $c create text [expr $w+4] 30 -anchor ne -text "sec"

    # draw ticks
    set dx [expr $w/($maxexp-$minexp+1)]
    set x0 [expr int($dx/2)+15]
    set x $x0
    for {set i $minexp} {$i<=$maxexp} {incr i} {
        $c create line $x 26 $x 33 -fill black -width 1
        if {$i>=4} {
            set txt "1e$i"
        } elseif {$i>=0} {
           set txt "1[string repeat 0 $i]"
        } elseif {$i>=-3} {
           set txt "0.[string repeat 0 [expr -$i-1]]1"
        } else {
            set txt "1e$i"
        }
        $c create text $x 30 -anchor n -text "+$txt" -fill "#808080" -font $fonts(msgname)

        # minor ticks at 2, 4, 6, 8
        foreach tick {0.301 0.602 0.778 0.903} {
            set minorx [expr $x+int($tick*$dx)]
            $c create line $minorx 29 $minorx 32 -fill black -width 1
        }
        incr x $dx
    }

    # draw events
    set dtmin [expr 1e$minexp]
    set minlabelx -1000
    set minlabelx2 -1000
    set labelssuppressed 0
    set msgs [opp_fesmsgs $config(timeline-maxnumevents) \
                          $config(timeline-wantselfmsgs) \
                          $config(timeline-wantnonselfmsgs) \
                          $config(timeline-msgnamepattern) \
                          $config(timeline-msgclassnamepattern)]

    foreach msgptr $msgs {
        # calculate position
        set dt [opp_msgarrtimefromnow $msgptr]
        if {$dt < $dtmin} {
            set anchor "sw"
            set x 10
        } else {
            set anchor "s"
            set x [expr int($x0+(log10($dt)-$minexp)*$dx)]
        }

        # display ball
        if [opp_getsimoption animation_msgcolors] {
           set msgkind [opp_getobjectfield $msgptr kind]
           set color [lindex {red green blue white yellow cyan magenta black} [expr $msgkind % 8]]
        } else {
            set color red
        }
        set ball [$c create oval -2 -3 2 4 -fill $color -outline $color -tags "dx tooltip msg $msgptr"]
        $c move $ball $x 29

        # print msg name, if it's not too close to previous label
        # label for only those msgs past this label's right edge will be displayed
        set msglabel [opp_getobjectfullname $msgptr]
        if {$msglabel!=""} {
            set estlabelx [expr $x-3*[string length $msglabel]]
            if {$estlabelx>=$minlabelx} {
                set labelid [$c create text $x 27 -text $msglabel -anchor $anchor -font $fonts(msgname) -tags "dx tooltip msgname $msgptr"]
                set minlabelx [lindex [$c bbox $labelid] 2]
                set labelssuppressed 0
            } elseif {$estlabelx>=$minlabelx2} {
                set labelid [$c create text $x 17 -text $msglabel -anchor $anchor -font $fonts(msgname) -tags "dx tooltip msgname $msgptr"]
                $c create line $x 24 $x 14 -fill "#808080" -width 1 -tags "h"
                set minlabelx2 [lindex [$c bbox $labelid] 2]
                set labelssuppressed 0
            } else {
                incr labelssuppressed
                if {$labelssuppressed==1} {
                    $c insert $labelid end ",..."
                }
            }
        }
    }
    $c lower "h"
}


proc timeline_popup {cx cy x y} {
    global widgets
    set c $widgets(timeline)
    set ids [$c find overlapping $cx $cy $cx $cy]

    # only display popup menu if right-click was on an empty area
    if {$ids=={}} {
        catch {destroy .popup}
        menu .popup -tearoff 0
        .popup add command -label "Timeline options..." -command "options_dialog t"
        .popup post $x $y
    }
}

