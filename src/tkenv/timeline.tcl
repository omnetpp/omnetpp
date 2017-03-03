#=================================================================
#  TIMELINE.TCL - part of
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

set tkenv(timeline-minexp) -1
set tkenv(timeline-maxexp) +1

proc redrawTimeline {} {
    global tkenv config widgets

    # spare work if we're not displayed
    if {$config(display-timeline)==0} {return}

    set c $widgets(timeline)

    # figure out vertical layout (see also timeline:fontChanged)
    set fontheight [font metrics TimelineFont -linespace]
    set row1y       [expr 2]
    set row2y       [expr $row1y+$fontheight]
    set axisy       [expr $row2y+$fontheight+3]
    set labely      [expr $axisy+3]
    set minorticky1 [expr $axisy-2]
    set minorticky2 [expr $axisy+3]
    set majorticky1 [expr $axisy-3]
    set majorticky2 [expr $axisy+4]
    set arrowy1     [expr $row2y-2]
    set arrowy2     [expr $axisy-4]

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
    $c create line 20 $axisy $w $axisy -arrow last -fill black -width 1
    $c create text [expr $w+4] $labely -anchor ne -text "sec" -font TimelineFont

    # draw ticks
    set dx [expr $w/($maxexp-$minexp+1)]
    set x0 [expr int($dx/2)+15]
    set x $x0
    for {set i $minexp} {$i<=$maxexp} {incr i} {
        $c create line $x $majorticky1 $x $majorticky2 -fill black -width 1
        if {$i>=4} {
            set txt "1e$i"
        } elseif {$i>=0} {
           set txt "1[string repeat 0 $i]"
        } elseif {$i>=-3} {
           set txt "0.[string repeat 0 [expr -$i-1]]1"
        } else {
            set txt "1e$i"
        }
        $c create text $x $labely -anchor n -text "+$txt" -fill "#808080" -font TimelineFont

        # minor ticks at 2, 4, 6, 8
        foreach tick {0.301 0.602 0.778 0.903} {
            set minorx [expr $x+int($tick*$dx)]
            $c create line $minorx $minorticky1 $minorx $minorticky2 -fill black -width 1
        }
        incr x $dx
    }

    # draw events
    set dtmin [expr 1e$minexp]
    set minlabelx -1000
    set minlabelx2 -1000
    set labelssuppressed 0
    set events [opp_fesevents $config(timeline-maxnumevents) \
                              $config(timeline-wantevents) \
                              $config(timeline-wantselfmsgs) \
                              $config(timeline-wantnonselfmsgs) \
                              $config(timeline-wantsilentmsgs)]

    foreach eventptr $events {
        # calculate position
        set dt [opp_eventarrtimefromnow $eventptr]
        if {$dt < $dtmin} {
            set anchor "nw"
            set x 10
        } else {
            set anchor "n"
            set x [expr int($x0+(log10($dt)-$minexp)*$dx)]
        }

        # display ball
        if [opp_instanceof $eventptr cMessage] {
            if [opp_getsimoption animation_msgcolors] {
               set msgkind [opp_getobjectfield $eventptr kind]
               set color [lindex {red green blue white yellow cyan magenta black} [expr $msgkind % 8]]
            } else {
                set color red
            }
            set ball [$c create oval -2 -3 2 4 -fill $color -outline $color -tags "dx tooltip msg $eventptr"]
        } else {
            set ball [$c create oval -2 -3 2 4 -fill "" -outline red -tags "dx tooltip msg $eventptr"]
        }
        $c move $ball $x $axisy

        # print msg name, if it's not too close to previous label
        # label for only those msgs past this label's right edge will be displayed
        set eventlabel [opp_getobjectfullname $eventptr]
        if {$eventlabel!=""} {
            set estlabelwidth [font measure TimelineFont $eventlabel]
            set estlabelx [expr $x-$estlabelwidth/2]
            if {$estlabelx>=$minlabelx} {
                set labelid [$c create text $x $row2y -text $eventlabel -anchor $anchor -font TimelineFont -tags "dx tooltip msgname $eventptr"]
                set minlabelx [lindex [$c bbox $labelid] 2]
                set labelssuppressed 0
            } elseif {$estlabelx>=$minlabelx2} {
                set labelid [$c create text $x $row1y -text $eventlabel -anchor $anchor -font TimelineFont -tags "dx tooltip msgname $eventptr"]
                $c create line $x $arrowy1 $x $arrowy2 -fill "#ccc" -width 1 -tags "h"
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

proc timeline:fontChanged {} {
   global widgets

   set h [font metrics TimelineFont -linespace]
   set h [expr 3*$h+6]
   $widgets(timeline) config -height $h
}

proc timeline:popup {cx cy x y} {
    global widgets
    set c $widgets(timeline)
    set ids [$c find overlapping $cx $cy $cx $cy]

    # only display popup menu if right-click was on an empty area
    if {$ids=={}} {
        catch {destroy .popup}
        menu .popup -tearoff 0
        .popup add command -label "Timeline Settings..." -command "preferencesDialog . t"
        tk_popup .popup $x $y
    }
}

proc timeline:click c {
   set item [$c find withtag current]
   set tags [$c gettags $item]

   set ptr ""
   if {[lsearch $tags "ptr*"] != -1} {
      regexp "ptr.*" $tags ptr
   }

   if {$ptr!=""} {
      mainWindow:selectionChanged "" $ptr
   }
}

proc timeline:dblClick c {
   set item [$c find withtag current]
   set tags [$c gettags $item]

   set ptr ""
   if {[lsearch $tags "ptr*"] != -1} {
      regexp "ptr.*" $tags ptr
   }

   if {$ptr!=""} {
      opp_inspect $ptr
   }
}

proc timeline:rightClick {c X Y x y} {
   set ptrs [ModuleInspector:getPtrsUnderMouse $c $x $y]
   if {$ptrs != {}} {
      set popup [createInspectorContextMenu "" $ptrs]
      tk_popup $popup $X $Y
   }
}

