#==========================================================================
#  DRAWOPTS.TCL -
#            part of the GNED, the Tcl/Tk graphical topology editor of
#                            OMNeT++
#
#   By Andras Varga
#   Additions: Istvan Pataki, Gabor Vincze ({deity|vinczeg}@sch.bme.hu)
#
#==========================================================================

#----------------------------------------------------------------#
#  Copyright (C) 1992-2001 Andras Varga
#  Technical University of Budapest, Dept. of Telecommunications,
#  Stoczek u.2, H-1111 Budapest, Hungary.
#
#  This file is distributed WITHOUT ANY WARRANTY. See the file
#  `license' for details on this and other legal matters.
#----------------------------------------------------------------#


proc editDrawOptions {key} {
    global ned

    if {$ned($key,type)=="module"} {
       editModuleDrawOptions $key
    } elseif {$ned($key,type)=="submod"} {
       editSubmoduleDrawOptions $key
    } elseif {$ned($key,type)=="conn"} {
       editConnectionDrawOptions $key
    }
}

proc editModuleDrawOptions {key} {
    global canvas ned gned

    set canv_id [canvasIdFromItemKey $key]
    set c $canvas($canv_id,canvas)

    # create dialog with OK and Cancel buttons
    createOkCancelDialog .modopts "Module Appearance"

    label-combo .modopts.f.thick "Line thickness:" {default 1 2 3 4}
    label-colorchooser .modopts.f.fill "Fill Color:" fill $ned($key,disp-fillcolor)
    label-colorchooser .modopts.f.outl "Outline color:" outl $ned($key,disp-outlinecolor)
    pack .modopts.f.thick -expand 1 -fill both  -side top
    pack .modopts.f.fill -expand 1 -fill both  -side top
    pack .modopts.f.outl -expand 1 -fill both  -side top

    set thickness $ned($key,disp-linethickness)
    if {$thickness==""} {set thickness "default"}
    .modopts.f.thick.e configure -value $thickness

    focus .modopts.f.thick.e

    # exec the dialog, extract its contents if OK was pressed
    if {[execOkCancelDialog .modopts] == 1} {

        # line thickness
        set thickness [.modopts.f.thick.e cget -value]
        if {$thickness=="default"} {set thickness ""}
        if {![regexp {^[0-9]*$} $thickness]} {set thickness ""}
        set ned($key,disp-linethickness) $thickness

        # fill color
        if {$gned(fill)=="default"} {
           set ned($key,disp-fillcolor) ""
        } else {
           set ned($key,disp-fillcolor) [.modopts.f.fill.f.radio.r2f.b cget -bg]
        }

        # outline color
        if {$gned(outl)=="default"} {
           set ned($key,disp-outlinecolor) ""
        } else {
           set ned($key,disp-outlinecolor) [.modopts.f.outl.f.radio.r2f.b cget -bg]
        }

        # redraw
        redrawItem $key
    }
    destroy .modopts
}

proc editSubmoduleDrawOptions {key} {
    global canvas ned gned

    set canv_id [canvasIdFromItemKey $key]
    set c $canvas($canv_id,canvas)

    # create dialog with OK and Cancel buttons
    createOkCancelDialog .submopts "Submodule Appearance"

    # add entry fields and focus on first one
    set icon  $ned($key,disp-icon)
    if {$icon == ""} {set gned(radio) rect} else {set gned(radio) icon}

    label .submopts.f.head -text "Choose between icon and colored rectangle:"
    label .submopts.f.sep1 -text "_____________________________"
    radiobutton .submopts.f.r1 -text "Icon" -value icon  -variable gned(radio)
    label-iconchooser .submopts.f.icon "Selected icon:" $ned($key,disp-icon)
    label .submopts.f.sep2 -text "_____________________________"
    radiobutton .submopts.f.r2 -text "Rectangle" -value rect -variable gned(radio)
    label-combo .submopts.f.thick "Line thickness:" {default 1 2 3 4}
    label-colorchooser .submopts.f.fill "Fill Color:" fill $ned($key,disp-fillcolor)
    label-colorchooser .submopts.f.outl "Outline color:" outl $ned($key,disp-outlinecolor)

    pack .submopts.f.head -expand 0 -fill y  -side top -anchor w
    pack .submopts.f.sep1 -expand 1 -fill x
    pack .submopts.f.r1 -expand 0 -fill y  -side top -anchor w
    pack .submopts.f.icon -expand 1 -fill both  -side top
    pack .submopts.f.sep2 -expand 1 -fill x
    pack .submopts.f.r2 -expand 0 -fill y  -side top -anchor w
    pack .submopts.f.thick -expand 1 -fill both  -side top
    pack .submopts.f.fill -expand 1 -fill both  -side top
    pack .submopts.f.outl -expand 1 -fill both  -side top

    set thickness $ned($key,disp-linethickness)
    if {$thickness==""} {set thickness "default"}
    .submopts.f.thick.e configure -value $thickness

    focus .submopts.f.thick.e

    # exec the dialog, extract its contents if OK was pressed
    if {[execOkCancelDialog .submopts] == 1} {

        # line thickness
        set thickness [.submopts.f.thick.e cget -value]
        if {$thickness=="default"} {set thickness ""}
        if {![regexp {^[0-9]*$} $thickness]} {set thickness ""}
        set ned($key,disp-linethickness) $thickness

        # icon
        set centsiz [_getCenterAndSize $c $key]
        if {$gned(radio)=="rect"} {
           set ned($key,disp-icon) ""
        } else {
           set ned($key,disp-icon) [.submopts.f.icon.b cget -image]
        }

        # position and size
        set ned($key,disp-xpos)  [expr int([lindex $centsiz 0])]
        set ned($key,disp-ypos)  [expr int([lindex $centsiz 1])]
        set ned($key,disp-xsize) [expr int([lindex $centsiz 2])]
        set ned($key,disp-ysize) [expr int([lindex $centsiz 3])]

        # fill color
        if {$gned(fill)=="default"} {
           set ned($key,disp-fillcolor) ""
        } else {
           set ned($key,disp-fillcolor) [.submopts.f.fill.f.radio.r2f.b cget -bg]
        }

        # outline color
        if {$gned(outl)=="default"} {
           set ned($key,disp-outlinecolor) ""
        } else {
           set ned($key,disp-outlinecolor) [.submopts.f.outl.f.radio.r2f.b cget -bg]
        }

        # redraw item and connections to/from it
        redrawItem $key

        # redraw connections to/from it
        foreach key [connKeysOfItem $key] {
           _redrawArrow $c $key
        }
    }
    destroy .submopts
}

proc editConnectionDrawOptions {key} {
    global canvas ned gned

    set drawmodes()   auto
    set drawmodes(a)  auto
    set drawmodes(m)  manual
    set drawmodes(n)  north
    set drawmodes(s)  south
    set drawmodes(e)  east
    set drawmodes(w)  west

    set canv_id [canvasIdFromItemKey $key]
    set c $canvas($canv_id,canvas)

    # create dialog with OK and Cancel buttons
    createOkCancelDialog .connopts "Connection Appearance"

    # add entry fields and focus on first one
    label-combo .connopts.f.thick "Line thickness:" {default 1 2 3 4}
    label-colorchooser .connopts.f.fill "Line color:" fill $ned($key,disp-fillcolor)
    label-combo .connopts.f.drawmode "Drawing mode:" {auto manual north east south west}
    label-sunkenlabel .connopts.f.src_an "Source anchoring:" "x=$ned($key,disp-src-anchor-x)%   y=$ned($key,disp-src-anchor-y)%"
    label-sunkenlabel .connopts.f.dest_an "Dest. anchoring:" "x=$ned($key,disp-dest-anchor-x)%   y=$ned($key,disp-dest-anchor-y)%"
    pack .connopts.f.thick -expand 0 -fill x -side top
    pack .connopts.f.fill -expand 0 -fill x -side top
    pack .connopts.f.drawmode -expand 0 -fill x -side top
    pack .connopts.f.src_an -expand 0 -fill x -side top
    pack .connopts.f.dest_an -expand 0 -fill x -side top

    set thickness $ned($key,disp-linethickness)
    if {$thickness==""} {set thickness "default"}
    .connopts.f.thick.e configure -value $thickness
    .connopts.f.drawmode.e configure -value $drawmodes($ned($key,disp-drawmode))

    # exec the dialog, extract its contents if OK was pressed, then delete dialog
    if {[execOkCancelDialog .connopts] == 1} {

        # line thickness
        set thickness [.connopts.f.thick.e cget -value]
        if {$thickness=="default"} {set thickness ""}
        if {![regexp {^[0-9]*$} $thickness]} {set thickness ""}
        set ned($key,disp-linethickness) $thickness

        # draw mode: one of m,n,e,w,s. auto is the default
        set dm [.connopts.f.drawmode.e cget -value]
        if {$dm=="auto"} {
           set ned($key,disp-drawmode)  ""
        } else {
           if {[lsearch -exact {auto manual north east south west} $dm]==-1} {set dm auto}
           set ned($key,disp-drawmode)  [string index $dm 0]
           debug "drawmode=$ned($key,disp-drawmode)"
        }

        # fill color
        if {$gned(fill)=="default"} {
           set ned($key,disp-fillcolor) ""
        } else {
           set ned($key,disp-fillcolor) [.connopts.f.fill.f.radio.r2f.b cget -bg]
        }

        redrawItem $key
    }
    destroy .connopts
}

#-------------------------------------------------------------

# label-colorchooser --
#
# utility function: create a frame with a label and a color chooser button
#
proc label-colorchooser {w label var {color ""}} {
    global gned

    frame $w
    label $w.l -anchor w -width 16 -text $label
    frame $w.f
    pack $w.l -anchor nw -expand 0 -fill none -side left -padx 2 -pady 2
    pack $w.f -anchor n -expand 1 -fill x -side left

    frame $w.f.radio
    pack $w.f.radio -anchor n -expand 1 -fill x -side left

    radiobutton $w.f.radio.r1 -text "default" -value default -variable gned($var)
    frame $w.f.radio.r2f
    pack $w.f.radio.r1 -anchor w -expand 0 -side top
    pack $w.f.radio.r2f -anchor w -expand 0 -side top

    radiobutton $w.f.radio.r2f.r2 -text "color:"   -value color   -variable gned($var)
    button $w.f.radio.r2f.b -command [list _setColor $w.f.radio.r2f.b $var [winfo toplevel $w]] -width 6
    pack $w.f.radio.r2f.r2 -anchor n -expand 0 -side left
    pack $w.f.radio.r2f.b -anchor c -expand 0 -fill none -side left

    if {$color!=""} {
       $w.f.radio.r2f.b config -background $color -activebackground $color
       set gned($var) color
    } else {
       set gned($var) default
    }
}

proc label-colorchooser-old {w label var {color ""}} {
    global gned

    frame $w
    label $w.l -anchor w -width 16 -text $label
    frame $w.f
    radiobutton $w.f.r1 -text "default" -value default -variable gned($var)
    radiobutton $w.f.r2 -text "color:"   -value color   -variable gned($var)
    button $w.f.b -command [list _setColor $w.f.b $var [winfo toplevel $w]] -width 4

    pack $w.l -anchor nw -expand 0 -fill none -side left
    pack $w.f -anchor n -expand 1 -fill y -side left
    pack $w.f.r1 -anchor n -expand 0 -side left
    pack $w.f.r2 -anchor n -expand 0 -side left
    pack $w.f.b -anchor c -expand 0 -fill none -side left

    if {$color!=""} {
       $w.f.b config -background $color -activebackground $color
       set gned($var) color
    } else {
       set gned($var) default
    }
}

# label-iconchooser --
#
# utility function: create a frame with a label and an icon chooser button
#
proc label-iconchooser {w label  {image ""}} {
    # utility function: create a frame with a label+entry
    global gned

    frame $w
    label $w.l -anchor w -width 16 -text $label
    button $w.b -command [list _chooseIcon $image $w.b [winfo toplevel $w]]

    if {$image == ""} {
        $w.b config -text "-none-"
    } else {
        $w.b config -image $image
    }
    pack $w.l -anchor n -expand 0 -side left
    pack $w.b -anchor n -expand 0 -side left
}

proc _setColor {butt var pwin} {

    # side effect: set gned(radio) "icon"

    global gned
    set color [tk_chooseColor -parent $pwin]
    if {$color!=""} {
        $butt config -background $color -activebackground $color
        set gned($var) color
        set gned(radio) "rect"
    }
}

proc _chooseIcon {oldicon win {pwin {}}} {
     global gned

     # side effect: set gned(radio) "icon"
     set dlg $pwin.iconbox
     createOkCancelDialog $dlg "Icon selection"

     frame  $dlg.f.select
     pack   $dlg.f.select -side top -expand 0 -fill x
     label  $dlg.f.select.label -text "Icon selected:"
     label  $dlg.f.select.name -text "$oldicon" -foreground #ff0000 -font bold
     pack   $dlg.f.select.label  -side left
     pack   $dlg.f.select.name  -side left

     set w $dlg.f.icons
     frame $w  -highlightthickness 1 -highlightbackground #000000
     pack $w -expand 1 -fill both -padx 14 -pady 6

     frame $w.tb -height 16
     canvas $w.c -yscrollcommand "$w.vsb set" -height 150 -bd 0
     scrollbar $w.vsb -command "$w.c yview"

     grid rowconfig $w 0 -weight 1 -minsize 0
     grid $w.c   -in $w -row 0 -column 0 -rowspan 1 -columnspan 1 -sticky news
     grid $w.vsb -in $w -row 0 -column 1 -rowspan 1 -columnspan 1 -sticky news

     frame $w.c.f -bd 0
     $w.c create window 0 0 -anchor nw -window $w.c.f

     set tb $w.tb
     set f $w.c.f

     set li 0
     set col 0
     foreach imgName $gned(icons) {
         set e $f.$imgName

         frame $e -relief flat -borderwidth 2

         button $e.b -image $imgName -command "_iconSelected $dlg.f.select.name $imgName $f"
         bind $e.b <Double-1> "$dlg.buttons.okbutton invoke"
         label $e.l -text $imgName
         pack $e.l -side bottom -anchor s -padx 2
         pack $e.b -side top  -anchor center -expand 1 -padx 2

         grid $e -in $f -row $li -column $col -rowspan 1 -columnspan 1 -sticky news

         # next column
         incr col
         if {$col == 6} {
             incr li
             set col 0
         }
     }

     update idletasks

     # adjust canvas width to frame width
     $w.c config -width [winfo width $f]
     $w.c config -scrollregion "0 0 0 [winfo height $f]"

     wm resizable $dlg 0 1

     if {[execOkCancelDialog $dlg] == 1} {
         set icon [$dlg.f.select.name cget -text]

         if {$icon != ""} {
             $win configure -image $icon
             set gned(radio) "icon"
         }
     }
     destroy $dlg
}

proc _iconSelected {select imgname frame} {

     if {[ $select cget -text ] != "" } {
         $frame.[$select cget -text] config -relief flat
     }
     $frame.$imgname config -relief solid

     $select configure -text $imgname

}

