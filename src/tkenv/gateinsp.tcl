#=================================================================
#  GATEINSP.TCL - part of
#
#                     OMNeT++/OMNEST
#            Discrete System Simulation in C++
#
#=================================================================

#----------------------------------------------------------------#
#  Copyright (C) 1992-2008 Andras Varga
#
#  This file is distributed WITHOUT ANY WARRANTY. See the file
#  `license' for details on this and other legal matters.
#----------------------------------------------------------------#


#-----------------------------------------------------------------
#  gate inspector
#-----------------------------------------------------------------
#
#proc createGateInspector {name geom} {
#    global fonts icons help_tips
#
#    set w $name
#    createInspectorToplevel $w $geom
#
#    set nb [inspector:createNotebook $w]
#
#    notebook:addPage $nb info {General}
#
#    inspector:createFields2Page $w
#
#    label-entry $nb.info.name {Name:}
#    label-sunkenlabel $nb.info.id {Id:}
#    label-button $nb.info.from {From:}
#    label-button $nb.info.to {To:}
#    label-entry $nb.info.dispstr {Display string:}
#    label-entry $nb.info.delay {Delay:}
#    label-entry $nb.info.error {Error:}
#    label-entry $nb.info.datarate {Data rate:}
#    label-sunkenlabel $nb.info.trfinish {Tx finishes:}
#
#    pack $nb.info.name -fill x -side top
#    pack $nb.info.id -fill x -side top
#    pack $nb.info.from -fill x -side top
#    pack $nb.info.to -fill x -side top
#    pack $nb.info.dispstr -fill x -side top
#    pack $nb.info.delay -fill x -side top
#    pack $nb.info.error -fill x -side top
#    pack $nb.info.datarate -fill x -side top
#    pack $nb.info.trfinish -fill x -side top
#}


#-----------------------------------------------------------------
#  Graphical gate/path window stuff
#-----------------------------------------------------------------

proc createGraphicalGateWindow {name geom} {
    global icons help_tips inspectordata
    global B2 B3

    set w $name
    createInspectorToplevel $w $geom

    # create toolbar
    packIconButton $w.toolbar.ascont -image $icons(asobject) -command "inspectThis $w {As Object}"
    packIconButton $w.toolbar.sep1   -separator
    packIconButton $w.toolbar.redraw -image $icons(redraw) -command "opp_inspectorcommand $w redraw"

    set help_tips($w.toolbar.ascont) {Inspect as object}
    set help_tips($w.toolbar.redraw) {Redraw}

    # create canvas
    set c $w.c

    # init some state vars (not all of them fully used, e.g. zoomfactor is only
    # needed by some modinsp2.tcl procs we call from here)
    set inspectordata($c:zoomfactor) 1
    set inspectordata($c:imagesizefactor) 1
    set inspectordata($c:showlabels) 1
    set inspectordata($c:showarrowheads) 1

    frame $w.grid
    scrollbar $w.hsb -orient horiz -command "$c xview"
    scrollbar $w.vsb -command "$c yview"
    canvas $c -background #a0e0a0 -relief raised \
        -xscrollcommand "$w.hsb set" \
        -yscrollcommand "$w.vsb set"
    pack $w.grid -expand yes -fill both -padx 1 -pady 1
    grid rowconfig    $w.grid 0 -weight 1 -minsize 0
    grid columnconfig $w.grid 0 -weight 1 -minsize 0

    grid $c -in $w.grid -row 0 -column 0 -rowspan 1 -columnspan 1 -sticky news
    grid $w.vsb -in $w.grid -row 0 -column 1 -rowspan 1 -columnspan 1 -sticky news
    grid $w.hsb -in $w.grid -row 1 -column 0 -rowspan 1 -columnspan 1 -sticky news

    # mouse bindings
    $c bind mod <Double-1> "graphGateWin:dblClick $c"
    $c bind gate <Double-1> "graphGateWin:dblClick $c {As Object}"
    $c bind conn <Double-1> "graphGateWin:dblClick $c {As Object}"
    $c bind msg <Double-1> "graphGateWin:dblClick $c"
    $c bind msgname <Double-1> "graphGateWin:dblClick $c"

    $c bind mod <$B3> "graphGateWin:rightClick $c %X %Y"
    $c bind gate <$B3> "graphGateWin:rightClick $c %X %Y"
    $c bind conn <$B3> "graphGateWin:rightClick $c %X %Y"
    $c bind msg <$B3> "graphGateWin:rightClick $c %X %Y"
    $c bind msgname <$B3> "graphGateWin:rightClick $c %X %Y"

    opp_inspectorcommand $w redraw
}


proc graphicalModuleWindow:drawModuleGate {c modptr gateptr modname gatename k xsiz dir highlight} {
    global fonts

    # puts "DEBUG: graphicalModuleWindow:drawModuleGate /$c/ /$modptr/ /$gateptr/ "
    #      "/$modname/ /$gatename/ /$k/ /$xsiz/ /$dir/ /$highlight/"

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
    $c create text 72 $y -text $gatename -anchor "$anch\e" -font $fonts(canvas)
    $c create text [expr 88+$dx] $y -text $modname -anchor "$anch\w" -font $fonts(canvas)

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

proc graphGateWin:drawConnection {c srcgateptr destgateptr chanptr chanstr dispstr} {
    global fonts

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

       $c create text [expr $x+3] [expr ($y0+$y1)/2] -text $chanstr -anchor w -font $fonts(canvas)

   } errmsg] {
       tk_messageBox -type ok -title Error -icon error -parent [winfo toplevel [focus]] \
                     -message "Error in display string of a connection: $errmsg"
   }
}

proc graphGateWin:dblClick {c {type (default)}} {

   set item [$c find withtag current]
   set tags [$c gettags $item]

   set ptr ""
   if {[lsearch $tags "ptr*"] != -1} {
      regexp "ptr.*" $tags ptr
   }

   if {$ptr!=""} {
      opp_inspect $ptr $type
   }
}

proc graphGateWin:rightClick {c X Y} {

   set item [$c find withtag current]
   set tags [$c gettags $item]

   set ptr ""
   if {[lsearch $tags "ptr*"] != -1} {
      regexp "ptr.*" $tags ptr
   }
   set ptr [lindex $ptr 0]

   if [opp_isnotnull $ptr] {
      set popup [createInspectorContextMenu $ptr]
      tk_popup $popup $X $Y
   }
}

