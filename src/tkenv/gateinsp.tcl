#==========================================================================
#  GATEINSP.TCL -
#            part of the Tcl/Tk windowing environment of
#                            OMNeT++
#==========================================================================

#----------------------------------------------------------------#
#  Copyright (C) 1992,99 Andras Varga
#  Technical University of Budapest, Dept. of Telecommunications,
#  Stoczek u.2, H-1111 Budapest, Hungary.
#
#  This file is distributed WITHOUT ANY WARRANTY. See the file
#  `license' for details on this and other legal matters.
#----------------------------------------------------------------#


#-----------------------------------------------------------------
#  gate inspector
#-----------------------------------------------------------------

proc create_gateinspector {name} {
    global fonts

    set w $name
    create_inspector_toplevel $w

    iconbutton $w.toolbar.mod -text {Module}
    pack $w.toolbar.mod -anchor n -side left

    frame $w.main
    pack $w.main -anchor center -expand 0 -fill both -side top

    label-sunkenlabel $w.main.name {ID and name:}
    label-sunkenlabel $w.main.mod  {Module:}
    label-sunkenlabel $w.main.delay {Delay:}
    label-sunkenlabel $w.main.error {Error:}
    label-sunkenlabel $w.main.datarate {Data rate:}
    label-sunkenlabel $w.main.trfinish {Tx finishes:}
    button $w.main.from -font $fonts(normal) -width 30
    button $w.main.to -font $fonts(normal) -width 30

    pack $w.main.name -expand 1 -fill x -side top
    pack $w.main.mod -expand 1 -fill x -side top
    pack $w.main.delay -expand 1 -fill x -side top
    pack $w.main.error -expand 1 -fill x -side top
    pack $w.main.datarate -expand 1 -fill x -side top
    pack $w.main.trfinish -expand 1 -fill x -side top
    pack $w.main.from -expand 1 -fill x -side top
    pack $w.main.to -expand 1 -fill x -side top
}


#-----------------------------------------------------------------
#  Graphical gate/path window stuff
#-----------------------------------------------------------------

proc create_graphicalgatewindow {name} {
    set w $name
    create_inspector_toplevel $w

    # create toolbar
    iconbutton $w.toolbar.ascont -text "As Object" \
           -command "inspect_this $w {As Object}"
    iconbutton $w.toolbar.module -text {Module}
    iconbutton $w.toolbar.redraw -text {Redraw} \
           -command "graphgatewin_redraw $w"
    foreach i {ascont module redraw} {
       pack $w.toolbar.$i -anchor n -side left
    }

    # create canvas
    set c $w.c

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
    $c bind mod <Double-1> "graphgatewin_dblclick $c"
    $c bind gate <Double-1> "graphgatewin_dblclick $c {As Object}"
    $c bind conn <Double-1> "graphgatewin_dblclick $c {As Object}"
    $c bind msg <Double-1> "graphgatewin_dblclick $c"
    $c bind msgname <Double-1> "graphgatewin_dblclick $c"

    $c bind mod <3> "graphgatewin_rightclick $c %X %Y"
    $c bind gate <3> "graphgatewin_rightclick $c %X %Y"
    $c bind conn <3> "graphgatewin_rightclick $c %X %Y"
    $c bind msg <3> "graphgatewin_rightclick $c %X %Y"
    $c bind msgname <3> "graphgatewin_rightclick $c %X %Y"

    graphgatewin_redraw $w
}


proc draw_module_gate {c modptr gateptr modname gatename k xsiz dir highlight} {

    # puts "DEBUG: draw_module_gate /$c/ /$modptr/ /$gateptr/ "
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
                     -tags "mod $modptr"]
    set mod2 [$c create line -$dx $dy -$dx $dy -$dx 0 $dx 0 $dx $dy $dx $dy \
                     -smooth 0 -width 2 \
                     -fill black \
                     -tags "mod $modptr"]
    set gate [$c create rect -6 -6 6 6 -fill $color \
                             -width 2 -tags "gate $gateptr" ]

    if {$dir=="O"} {set anch "s"} else {set anch "n"}
    $c create text 72 $y -text $gatename -anchor "$anch\e"
    $c create text [expr 88+$dx] $y -text $modname -anchor "$anch\w"

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

proc draw_conn {c srcgateptr destgateptr chanstr dispstr} {

    split_dispstr $dispstr tags

    if {![info exists tags(s)]} {set tags(s) {auto}}
    if {![info exists tags(d)]} {set tags(d) {auto}}

    set srcrect  [$c coords $srcgateptr]
    set destrect [$c coords $destgateptr]
    set y0 [lindex $srcrect 3]
    set y1 [lindex $destrect 1]
    set x  [expr ([lindex $destrect 0]+ [lindex $destrect 2])/2]

    if [catch {
       if {![info exists tags(o)]} {set tags(o) {}}
       set fill [lindex $tags(o) 0]
       if {$fill == ""} {set fill black}
       set width [lindex $tags(o) 1]
       if {$width == ""} {set width 2}

       $c create line $x $y0 $x $y1 -arrow last \
           -fill $fill -width $width -tags "conn $srcgateptr"

       $c create text [expr $x+3] [expr ($y0+$y1)/2] -text $chanstr -anchor w

   } errmsg] {
       tk_messageBox -type ok -title Error -icon error \
                     -message "Error in display string of a connection: $errmsg"
   }
}

proc graphgatewin_redraw w {

    $w.c delete all

    global hack_y
    set hack_y 0

    opp_inspectorcommand $w redraw
}

proc graphgatewin_dblclick {c {type (default)}} {

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

proc graphgatewin_rightclick {c X Y} {

   set item [$c find withtag current]
   set tags [$c gettags $item]

   set ptr ""
   if {[lsearch $tags "ptr*"] != -1} {
      regexp "ptr.*" $tags ptr
   }
   set ptr [lindex $ptr 0]

   if {$ptr!=""} {
      popup_insp_menu $ptr $X $Y
   }
}
