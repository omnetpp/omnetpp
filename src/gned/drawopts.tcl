#=================================================================
#  DRAWOPTS.TCL - part of
#
#                     OMNeT++/OMNEST
#            Discrete System Simulation in C++
#
#  Parts by: Istvan Pataki, Gabor Vincze ({deity|vinczeg}@sch.bme.hu)
#
#=================================================================

#----------------------------------------------------------------#
#  Copyright (C) 1992-2005 Andras Varga
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
    label-colorchooser .modopts.f.fill "Fill Color:" $ned($key,disp-fillcolor)
    label-colorchooser .modopts.f.outl "Outline color:" $ned($key,disp-outlinecolor)
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

        # fill color, outline color
        set ned($key,disp-fillcolor) [.modopts.f.fill.f.e get]
        if {![isValidColor $ned($key,disp-fillcolor)]} {set ned($key,disp-fillcolor) ""}
        set ned($key,disp-outlinecolor) [.modopts.f.outl.f.e get]
        if {![isValidColor $ned($key,disp-outlinecolor)]} {set ned($key,disp-outlinecolor) ""}

        # redraw
        redrawItemOnAnyCanvas $key
        markNedFileOfItemDirty $key
        updateTreeManager

    }
    destroy .modopts
}

proc editSubmoduleDrawOptions {key} {
    global canvas ned gned config

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
    label-colorchooser .submopts.f.icolor "Colorize icon:" $ned($key,disp-iconcolor)
    label-combo .submopts.f.icolorpc "Colorization (%):" {default 10 20 30 40 50 60 70 80 90 100}
    label .submopts.f.sep2 -text "_____________________________"
    radiobutton .submopts.f.r2 -text "Rectangle" -value rect -variable gned(radio)
    label-combo .submopts.f.thick "Line thickness:" {default 1 2 3 4}
    label-colorchooser .submopts.f.fill "Fill Color:" $ned($key,disp-fillcolor)
    label-colorchooser .submopts.f.outl "Outline color:" $ned($key,disp-outlinecolor)

    pack .submopts.f.head -expand 0 -fill y  -side top -anchor w
    pack .submopts.f.sep1 -expand 1 -fill x
    pack .submopts.f.r1 -expand 0 -fill y  -side top -anchor w
    pack .submopts.f.icon -expand 1 -fill both  -side top
    pack .submopts.f.icolor -expand 1 -fill both  -side top
    pack .submopts.f.icolorpc -expand 1 -fill both  -side top
    pack .submopts.f.sep2 -expand 1 -fill x
    pack .submopts.f.r2 -expand 0 -fill y  -side top -anchor w
    pack .submopts.f.thick -expand 1 -fill both  -side top
    pack .submopts.f.fill -expand 1 -fill both  -side top
    pack .submopts.f.outl -expand 1 -fill both  -side top

    set thickness $ned($key,disp-linethickness)
    if {$thickness==""} {set thickness "default"}
    .submopts.f.thick.e configure -value $thickness

    set iconcolorpc $ned($key,disp-iconcolorpc)
    if {$iconcolorpc==""} {set iconcolorpc "default"}
    .submopts.f.icolorpc.e configure -value $iconcolorpc

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
           set ned($key,disp-icon) [.submopts.f.icon.e get]
        }

        # position and size
        set ned($key,disp-xpos)  [expr int([lindex $centsiz 0])]
        set ned($key,disp-ypos)  [expr int([lindex $centsiz 1])]
        set ned($key,disp-xsize) [expr int([lindex $centsiz 2])]
        set ned($key,disp-ysize) [expr int([lindex $centsiz 3])]

        # icon color, fill color, outline color
        set ned($key,disp-iconcolor) [.submopts.f.icolor.f.e get]
        if {![isValidColor $ned($key,disp-iconcolor)]} {set ned($key,disp-iconcolor) ""}
        set ned($key,disp-fillcolor) [.submopts.f.fill.f.e get]
        if {![isValidColor $ned($key,disp-fillcolor)]} {set ned($key,disp-fillcolor) ""}
        set ned($key,disp-outlinecolor) [.submopts.f.outl.f.e get]
        if {![isValidColor $ned($key,disp-outlinecolor)]} {set ned($key,disp-outlinecolor) ""}

        # icon color percentage
        set iconcolorpc [.submopts.f.icolorpc.e cget -value]
        if {$iconcolorpc=="default"} {set iconcolorpc ""}
        if {![regexp {^[0-9]*$} $iconcolorpc]} {set iconcolorpc ""}
        set ned($key,disp-iconcolorpc) $iconcolorpc

        # redraw item and connections to/from it
        redrawItemOnAnyCanvas $key

        # redraw connections to/from it
        foreach key [connKeysOfItem $key] {
           _redrawArrow $c $key
        }
        markNedFileOfItemDirty $key
        updateTreeManager
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
    label-colorchooser .connopts.f.fill "Line color:" $ned($key,disp-fillcolor)
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
           set ned($key,disp-drawmode)  "a"
        } else {
           if {[lsearch -exact {manual north east south west} $dm]==-1} {set dm manual}
           set ned($key,disp-drawmode)  [string index $dm 0]
           debug "drawmode=$ned($key,disp-drawmode)"
        }

        # fill color
        set ned($key,disp-fillcolor) [.connopts.f.fill.f.e get]
        if {![isValidColor $ned($key,disp-fillcolor)]} {set ned($key,disp-fillcolor) ""}

        redrawItemOnAnyCanvas $key
        markNedFileOfItemDirty $key
        updateTreeManager
    }
    destroy .connopts
}

#-------------------------------------------------------------

# label-iconchooser --
#
# utility function: create a frame with a label and an icon chooser button
#
proc label-iconchooser {w label  {image ""}} {
    # utility function: create a frame with a label+entry
    global gned icons

    frame $w
    label $w.l -anchor w -width 16 -text $label
    combo $w.e $gned(icons) [list _setButtonImg $w.e $w.b]
    button $w.b -relief raised -bd 1 -command [list _chooseIcon $image $w.e [winfo toplevel $w]]

    # next line sets button image as well
    $w.e configure -value $image
    grid $w.l $w.e -sticky ew -padx 2 -pady 2
    grid x    $w.b -sticky nw -padx 2 -pady 2
    grid columnconfigure $w 1 -weight 1
}

proc _setButtonImg {combo button} {
    global icons gned bitmaps
    set img [$combo get]
    if {$img == ""} {
        set gned(radio) "rect"
        $button config -text "    - n/a -    "
    } else {
        set gned(radio) "icon"
        if [catch {
            $button config -image $bitmaps($img)
        }] {
            $button config -image $icons(unknown)
        }
    }
}

proc _chooseIcon {oldicon win {pwin {}}} {
     global gned bitmaps config

     # side effect: set gned(radio) "icon"
     set dlg $pwin.iconbox
     createOkCancelDialog $dlg "Icon selection"
     wm geometry $dlg "660x500"

     set prefixes $gned(iconprefixes)
     set sizes {large normal small {very small}}
     label-combo $dlg.f.prefix "Category:" $prefixes $config(iconprefix)
     label-combo $dlg.f.sizes "Size:" $sizes  $config(iconsize)
     combo-onchange $dlg.f.prefix.e [list _chooseIconOnChange $dlg]
     combo-onchange $dlg.f.sizes.e [list _chooseIconOnChange $dlg]
     pack  $dlg.f.prefix -side top -anchor w
     pack  $dlg.f.sizes -side top -anchor w

     frame  $dlg.f.select
     pack   $dlg.f.select -side top -expand 0 -fill x
     label  $dlg.f.select.label -text "Icon selected:"
     label  $dlg.f.select.name -text "$oldicon" -foreground #ff0000
     pack   $dlg.f.select.label  -side left
     pack   $dlg.f.select.name  -side left

     set w $dlg.f.icons
     frame $w  -highlightthickness 1 -highlightbackground #000000
     pack $w -expand 1 -fill both -padx 14 -pady 6

     frame $w.tb -height 16
     canvas $w.c  -height 150 -bd 0 -yscrollcommand "$w.vsb set" -xscrollcommand "$w.hsb set"
     scrollbar $w.vsb -command "$w.c yview"
     scrollbar $w.hsb -command "$w.c xview" -orient horiz

     grid rowconfig $w 0 -weight 1 -minsize 0
     grid columnconfig $w 0 -weight 1 -minsize 0
     grid $w.c   -in $w -row 0 -column 0 -rowspan 1 -columnspan 1 -sticky news
     grid $w.hsb -in $w -row 1 -column 0 -rowspan 1 -columnspan 1 -sticky ews
     grid $w.vsb -in $w -row 0 -column 1 -rowspan 1 -columnspan 1 -sticky nes

     _chooseIconOnChange $dlg

     if {[execOkCancelDialog $dlg] == 1} {
         set icon [$dlg.f.select.name cget -text]
         $win configure -value $icon

         set config(iconprefix) [$dlg.f.prefix.e get]
         set config(iconsize)   [$dlg.f.sizes.e get]
     }
     destroy $dlg
}

proc _chooseIconOnChange {dlg} {

     set prefix [$dlg.f.prefix.e get]
     set size   [$dlg.f.sizes.e get]

     if [string match "l*" $size] {
         set sz "l"
     } elseif [string match "n*" $size] {
         set sz "n"
     } elseif [string match "s*" $size] {
         set sz "s"
     } elseif [string match "v*s*" $size] {
         set sz "vs"
     } else {
         set sz "n"
     }

     _fillCanvasWithIcons $dlg "$prefix*,$sz" 5
}

proc _fillCanvasWithIcons {dlg filter numcols} {
     global gned bitmaps

     set w $dlg.f.icons
     set c $w.c
     set f $w.c.f

     catch {destroy $f}
     frame $f -bd 0
     $c create window 0 0 -anchor nw -window $f

     set li 0
     set col 0
     foreach i [lsort [array names bitmaps $filter]] {
         set img $bitmaps($i)

         set imgName $i
         regsub -- ",n" $imgName "" imgName
         regsub -- "," $imgName "_" imgName

         set e $f.$imgName

         frame $e -relief flat -borderwidth 2

         button $e.b -image $img -command "_iconSelected $dlg.f.select.name $imgName $f"
         bind $e.b <Double-1> "$dlg.buttons.okbutton invoke"
         label $e.l -text $imgName
         pack $e.l -side bottom -anchor s -padx 2
         pack $e.b -side top  -anchor center -expand 1 -padx 2

         grid $e -in $f -row $li -column $col -rowspan 1 -columnspan 1 -sticky news

         # next column
         incr col
         if {$col == $numcols} {
             incr li
             set col 0
         }
     }

     update idletasks

     # adjust canvas width to frame width
     $w.c config -width [winfo width $f]
     $w.c config -scrollregion "0 0 [winfo width $f] [winfo height $f]"
}

proc _iconSelected {select imgname frame} {

     if {[ $select cget -text ] != "" } {
         catch {$frame.[$select cget -text] config -relief flat}
     }
     $frame.$imgname config -relief solid

     $select configure -text $imgname
}

proc isValidColor {color} {
     set res [expr ![catch {button .isValidColorTestButton -bg $color}]]
     catch {destroy .isValidColorTestButton}
     return $res
}

