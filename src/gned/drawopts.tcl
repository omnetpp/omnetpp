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
#  Copyright (C) 1992,99 Andras Varga
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

    set c $canvas($ned($key,canvasnum),canvas)

    # create dialog with OK and Cancel buttons
    createOkCancelDialog .modopts "Module Drawing Options"

    label-entry .modopts.f.thick "Line thickness:" $ned($key,linethickness)
    label-colorchooser .modopts.f.fill "Fill Color:" fill $ned($key,fill-color)
    label-colorchooser .modopts.f.outl "Outline color:" outl $ned($key,outline-color)
    pack .modopts.f.thick -expand 1 -fill both  -side top
    pack .modopts.f.fill -expand 1 -fill both  -side top
    pack .modopts.f.outl -expand 1 -fill both  -side top

    focus .modopts.f.thick.e

    # exec the dialog, extract its contents if OK was pressed
    if {[execOkCancelDialog .modopts] == 1} {
        # line
        set ned($key,linethickness)  [.modopts.f.thick.e get]

        # fill color
        if {$gned(fill)=="default"} {
           set ned($key,fill-color) ""
        } else {
           set ned($key,fill-color) [.modopts.f.fill.f.b cget -bg]
        }

        # outline color
        if {$gned(outl)=="default"} {
           set ned($key,outline-color) ""
        } else {
           set ned($key,outline-color) [.modopts.f.outl.f.b cget -bg]
        }

        # redraw
        redrawItem $key
    }
    destroy .modopts
}

proc editSubmoduleDrawOptions {key} {
    global canvas ned gned

    set c $canvas($ned($key,canvasnum),canvas)

    # create dialog with OK and Cancel buttons
    createOkCancelDialog .submopts "Submodule Drawing Options"

    # add entry fields and focus on first one
    set icon  $ned($key,icon)
    if {$icon == ""} {set gned(radio) rect} else {set gned(radio) icon}


    radiobutton .submopts.f.r1 -text "Icon" -value icon  -variable gned(radio)
    label-iconchooser .submopts.f.icon "Selected icon:" $ned($key,icon)
    radiobutton .submopts.f.r2 -text "Rectangle" -value rect -variable gned(radio)
    label-entry .submopts.f.thick "Line thickness:" $ned($key,linethickness)
    label-colorchooser .submopts.f.fill "Fill Color:" fill $ned($key,fill-color)
    label-colorchooser .submopts.f.outl "Outline color:" outl $ned($key,outline-color)

    pack .submopts.f.r1 -expand 0 -fill y  -side top -anchor w
    pack .submopts.f.icon -expand 1 -fill both  -side top
    pack .submopts.f.r2 -expand 0 -fill y  -side top -anchor w
    pack .submopts.f.thick -expand 1 -fill both  -side top
    pack .submopts.f.fill -expand 1 -fill both  -side top
    pack .submopts.f.outl -expand 1 -fill both  -side top

    focus .submopts.f.thick.e

    # exec the dialog, extract its contents if OK was pressed
    if {[execOkCancelDialog .submopts] == 1} {

        # line thickness
        set ned($key,linethickness)  [.submopts.f.thick.e get]

        # icon
        set centsiz [_getCenterAndSize $c $key]
        if {$gned(radio)=="rect"} {
           set ned($key,icon) ""
        } else {
           set ned($key,icon) [.submopts.f.icon.b cget -image]
        }

        # position and size
        set ned($key,x-pos)  [expr int([lindex $centsiz 0])]
        set ned($key,y-pos)  [expr int([lindex $centsiz 1])]
        set ned($key,x-size) [expr int([lindex $centsiz 2])]
        set ned($key,y-size) [expr int([lindex $centsiz 3])]

        # fill color
        if {$gned(fill)=="default"} {
           set ned($key,fill-color) ""
        } else {
           set ned($key,fill-color) [.submopts.f.fill.f.b cget -bg]
        }

        # outline color
        if {$gned(outl)=="default"} {
           set ned($key,outline-color) ""
        } else {
           set ned($key,outline-color) [.submopts.f.outl.f.b cget -bg]
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

    set c $canvas($ned($key,canvasnum),canvas)

    # create dialog with OK and Cancel buttons
    createOkCancelDialog .connopts "Connection Drawing Options"

    # add entry fields and focus on first one
    label-entry .connopts.f.thick "Line thickness:" $ned($key,linethickness)
    label-colorchooser .connopts.f.fill "Line color:" fill $ned($key,fill-color)
    label-combo .connopts.f.drawmode "Drawing mode:" {auto manual north east south west}
    label-sunkenlabel .connopts.f.src_an "Source anchoring:" "x=$ned($key,an_src_x)% y=$ned($key,an_src_y)%"
    label-sunkenlabel .connopts.f.dest_an "Dest. anchoring:" "x=$ned($key,an_dest_x)% y=$ned($key,an_dest_y)%"
    pack .connopts.f.thick -expand 0 -fill x -side top
    pack .connopts.f.fill -expand 0 -fill x -side top
    pack .connopts.f.drawmode -expand 0 -fill x -side top
    pack .connopts.f.src_an -expand 0 -fill x -side top
    pack .connopts.f.dest_an -expand 0 -fill x -side top

    .connopts.f.drawmode.e configure -text $drawmodes($ned($key,drawmode))

    # exec the dialog, extract its contents if OK was pressed, then delete dialog
    if {[execOkCancelDialog .connopts] == 1} {

        set ned($key,linethickness)  [.connopts.f.thick.e get]

        # draw mode: one of m,n,e,w,s. auto is the default
        set dm [.connopts.f.drawmode.e cget -text]
        if {$dm=="auto"} {
           set ned($key,drawmode)  ""
        } else {
           set ned($key,drawmode)  [string index $dm 0]
puts "dbg: drawmode=$ned($key,drawmode)"
        }

        # fill color
        if {$gned(fill)=="default"} {
           set ned($key,fill-color) ""
        } else {
           set ned($key,fill-color) [.connopts.f.fill.f.b cget -bg]
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
    radiobutton $w.f.r1 -text "default" -value default -variable gned($var)
    radiobutton $w.f.r2 -text "color:"   -value color   -variable gned($var)
    button $w.f.b -command "_setColor $w.f.b $var" -width 4

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
    button $w.b -command [list _chooseIcon $image $w.b]

    if {$image == ""} {
        $w.b config -text "-none-"
    } else {
        $w.b config -image $image
    }
    pack $w.l -anchor n -expand 0 -side left
    pack $w.b -anchor n -expand 0 -side left
}

proc _setColor {butt var} {

    # side effect: set gned(radio) "icon"

    global gned
    set color [tk_chooseColor]
    if {$color!=""} {
        $butt config -background $color -activebackground $color
        set gned($var) color
        set gned(radio) "rect"
    }
}

proc _chooseIcon {oldicon w} {
     global gned

     # side effect: set gned(radio) "icon"

     createOkCancelDialog .iconBox "Icon selection"

     frame  .iconBox.f.select
     pack   .iconBox.f.select -side top -expand 0 -fill x
     label  .iconBox.f.select.felirat -text "Icon selected:"
     label  .iconBox.f.select.name -text "$oldicon" -foreground #ff0000 -font bold
     pack   .iconBox.f.select.felirat  -side left
     pack   .iconBox.f.select.name  -side left

     canvas .iconBox.f.c -xscrollcommand ".iconBox.f.s set"
     scrollbar .iconBox.f.s -command ".iconBox.f.c xview" -orient horizontal
     pack .iconBox.f.s -side bottom -fill x
     pack .iconBox.f.c -side top -expand yes -fill both
     frame .iconBox.f.c.f

     .iconBox.f.c create window 0 0 -anchor nw -window .iconBox.f.c.f

     foreach imgName $gned(icons) {

         set f .iconBox.f.c.f.$imgName
         frame $f -highlightthickness 1 -highlightbackground #000000
         button $f.b -image $imgName -command "_iconSelected .iconBox.f.select.name $imgName .iconBox.f.c.f"
         bind $f.b <Double-1> ".iconBox.buttons.okbutton invoke"
         label $f.l -text $imgName
         pack $f  -side left -fill y
         pack $f.l -side bottom -anchor s -padx 2
         pack $f.b -side top  -anchor center -expand 1 -padx 2
     }

     .iconBox.f.c config -height 70
     update idletasks
     .iconBox.f.c config -scrollregion "0 0  [winfo width .iconBox.f.c.f] 0"

     if {[execOkCancelDialog .iconBox] == 1} {
         set icon [.iconBox.f.select.name cget -text]

         if {$icon != ""} {
             $w configure -image $icon
             set gned(radio) "icon"
         }
     }
     destroy .iconBox
}

proc _iconSelected {select me frame} {

     if {[ $select cget -text ] != "" } {
         $frame.[$select cget -text] configure -background #d9d9d9
         $frame.[$select cget -text].l configure -background #d9d9d9
     }
     $select configure -text $me
     $frame.$me configure -background red
     $frame.$me.l configure -background red
}
