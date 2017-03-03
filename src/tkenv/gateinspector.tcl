#=================================================================
#  GATEINSPECTOR.TCL - part of
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


proc createGateInspector {insp geom} {
    global icons help_tips

    createInspectorToplevel $insp $geom

    # create toolbar
    packToolbutton $insp.toolbar.sep1   -separator
    packToolbutton $insp.toolbar.redraw -image $icons(redraw) -command "opp_inspectorcommand $insp redraw"
    set help_tips($insp.toolbar.redraw) "Redraw"

    createGateViewer $insp
}

proc createEmbeddedGateInspector {insp} {
    createGateViewer $insp
}

proc createGateViewer {insp} {
    global inspectordata
    global B2 B3

    set c $insp.c

    # init some state vars (not all of them fully used, e.g. zoomfactor is only
    # needed by some modinsp2.tcl procs we call from here)
    set inspectordata($c:zoomfactor) 1
    set inspectordata($c:imagesizefactor) 1
    set inspectordata($c:showlabels) 1
    set inspectordata($c:showarrowheads) 1

    # create canvas
    ttk::frame $insp.grid
    pack $insp.grid -expand yes -fill both -padx 1 -pady 1
    canvas $c -background #a0e0a0 -relief raised
    addScrollbars $c $insp.grid

    # mouse bindings
    $c bind mod <Double-1> "GateInspector:dblClick $insp"
    $c bind gate <Double-1> "GateInspector:dblClick $insp {As Object}"
    $c bind conn <Double-1> "GateInspector:dblClick $insp {As Object}"
    $c bind msg <Double-1> "GateInspector:dblClick $insp"
    $c bind msgname <Double-1> "GateInspector:dblClick $insp"

    $c bind mod <$B3> "GateInspector:rightClick $insp %X %Y"
    $c bind gate <$B3> "GateInspector:rightClick $insp %X %Y"
    $c bind conn <$B3> "GateInspector:rightClick $insp %X %Y"
    $c bind msg <$B3> "GateInspector:rightClick $insp %X %Y"
    $c bind msgname <$B3> "GateInspector:rightClick $insp %X %Y"

    inspector:bindSideButtons $insp
}

proc GateInspector:drawModuleGate {c modptr gateptr modname gatename k xsiz dir highlight} {
    set y [expr 40+40*$k]

    if {$xsiz<-1} {set xsiz -1.5}
    if {$xsiz>3} {set xsiz 3}
    set dx [expr ($xsiz+2) * 20]
    # set dy [expr ($xsiz+2) * 10]
    set dy 20

    if {$highlight} {
        set color yellow
    } else {
        set color white
    }

    if {$dir=="O"} {set dy [expr -$dy]}
    set mod1 [$c create polygon -$dx $dy -$dx $dy -$dx 0 $dx 0 $dx $dy $dx $dy \
                     -smooth 0 -width 2 \
                     -fill #c0c0ff \
                     -tags "tooltip mod $modptr"]
    set mod2 [$c create line -$dx $dy -$dx $dy -$dx 0 $dx 0 $dx $dy $dx $dy \
                     -smooth 0 -width 2 \
                     -fill black \
                     -tags "tooltip mod $modptr"]
    set gate [$c create rect -6 -6 6 6 -fill $color \
                             -width 2 -tags "tooltip gate $gateptr" ]

    if {$dir=="O"} {set anch "s"} else {set anch "n"}
    $c create text 72 $y -text $gatename -anchor "$anch\e" -font CanvasFont
    $c create text [expr 88+$dx] $y -text $modname -anchor "$anch\w" -font CanvasFont

    $c move $mod1 80 $y
    $c move $mod2 80 $y
    $c move $gate 80 $y

    # config canvas size and scrolling
    set bbox [$c bbox all]
    set cwidth [expr [lindex $bbox 2]-[lindex $bbox 0]+20]
    set cheight [expr [lindex $bbox 3]-[lindex $bbox 1]+20]
    $c config -width [expr $cwidth<500 ? $cwidth : 500]
    $c config -height [expr $cheight<400 ? $cheight : 400]
    $c config -scrollregion [list 0 0 $cwidth $cheight]

}

proc GateInspector:drawConnection {c srcgateptr destgateptr chanptr chanstr dispstr} {
    opp_displaystring $dispstr parse tags $chanptr 0

    if {![info exists tags(s)]} {set tags(s) {auto}}
    if {![info exists tags(d)]} {set tags(d) {auto}}

    set srcrect  [$c coords $srcgateptr]
    set destrect [$c coords $destgateptr]
    set y0 [lindex $srcrect 3]
    set y1 [lindex $destrect 1]
    set x  [expr ([lindex $destrect 0]+ [lindex $destrect 2])/2]

    if [catch {
       if {![info exists tags(ls)]} {set tags(ls) {}}
       set fill [lindex $tags(ls) 0]
       if {$fill == ""} {set fill black}
       set width [lindex $tags(ls) 1]
       if {$width == ""} {set width 2}

       $c create line $x $y0 $x $y1 -arrow last \
           -fill $fill -width $width -tags "tooltip conn $srcgateptr"

       $c create text [expr $x+3] [expr ($y0+$y1)/2] -text $chanstr -anchor w -font CanvasFont

   } errmsg] {
       tk_messageBox -type ok -title "Error" -icon error -parent [winfo toplevel [focusOrRoot]] \
                     -message "Error in display string of a connection: $errmsg"
   }
}

proc GateInspector:dblClick {insp {type ""}} {
   set c $insp.c
   set item [$c find withtag current]
   set tags [$c gettags $item]

   set ptr ""
   if {[lsearch $tags "ptr*"] != -1} {
      regexp "ptr.*" $tags ptr
   }

   if {$ptr!=""} {
      inspector:dblClick $insp $ptr
   }
}

proc GateInspector:rightClick {insp X Y} {
   set c $insp.c
   set item [$c find withtag current]
   set tags [$c gettags $item]

   set ptr ""
   if {[lsearch $tags "ptr*"] != -1} {
      regexp "ptr.*" $tags ptr
   }
   set ptr [lindex $ptr 0]

   if [opp_isnotnull $ptr] {
      set popup [createInspectorContextMenu $insp $ptr]
      tk_popup $popup $X $Y
   }
}

