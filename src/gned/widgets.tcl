#==========================================================================
#  WIDGETS.TCL -
#            part of the Tcl/Tk windowing environment of
#                            OMNeT++
#==========================================================================

#----------------------------------------------------------------#
#  Copyright (C) 1992,99 Andras Varga
#  Technical University of Budapest, Dept. of Telecommunications,
#  Stoczek u.2, H-1111 Budapest, Hungary.
#
#  This file is distributed WITHOUT ANY WARRANTY. See the file
#  `terms' for details on this and other legal matters.
#----------------------------------------------------------------#

#------
# Parts of this file were created using Stewart Allen's Visual Tcl (vtcl)
#------

#===================================================================
#    PROCEDURES FOR CREATING NEW 'WIDGET TYPES'
#===================================================================

proc iconbutton {w args} {
    global fonts

    eval button $w -fg red3 -activeforeground red3 \
                   -relief raised -font $fonts(icon) \
                   $args
    return $w
}

proc combo {w list {cmd {}}} {
    # implements a combo box widget (which is missing from Tk)
    # using a menubutton and a menu

    global fonts

    menubutton $w -menu $w.m -font $fonts(bold) -relief raised -width 14 -indicatoron 1
    menu $w.m -tearoff 0
    foreach i $list {
        $w.m add command -label $i -command "$w configure -text \{$i\}; $cmd"
    }
    catch {$w config -text [lindex $list 0]}
    return $w
}

proc comboconfig {w list {cmd {}}} {
    # reconfigures a combo box widget

    $w.m delete 0 end
    foreach i $list {
       $w.m add command -label $i -command "$w configure -text \{$i\}; $cmd"
    }
    if {[lsearch $list [$w cget -text]] == -1} {
        $w.m invoke 0
    }
    return $w
}

proc label-entry {w label {text {}}} {
    # utility function: create a frame with a label+entry
    frame $w
    label $w.l -anchor w -width 16 -text $label
    entry $w.e -highlightthickness 0
    pack $w.l -anchor center -expand 0 -fill none -padx 2 -pady 2 -side left
    pack $w.e -anchor center -expand 1 -fill x -padx 2 -pady 2 -side right
    $w.e insert 0 $text
}

proc label-sunkenlabel {w label {text {}}} {
    # utility function: create a frame with a label+"readonly entry"
    frame $w
    label $w.l -anchor w -width 16 -text $label
    label $w.e -relief groove -justify left
    pack $w.l -anchor center -expand 0 -fill none -padx 2 -pady 2 -side left
    pack $w.e -anchor center -expand 1 -fill x -padx 2 -pady 2 -side right
    $w.e config -text $text
}

proc label-combo {w label list {text {}} {cmd {}}} {
    # utility function: create a frame with a label+combo
    frame $w
    label $w.l -anchor w -width 16 -text $label
    combo $w.e $list $cmd
    pack $w.l -anchor center -expand 0 -fill none -padx 2 -pady 2 -side left
    pack $w.e -anchor center -expand 1 -fill x -padx 2 -pady 2 -side right
    if {$text != ""} {
         $w.e config -text $text
    } else {
         $w.e config -text [lindex $list 0]
    }
}

proc label-text {w label height {text {}}} {
    # utility function: create a frame with a label+text
    frame $w
    label $w.l -anchor w -width 16 -text $label
    text $w.t -highlightthickness 0 -height $height -width 40
    pack $w.l -anchor n -expand 0 -fill none -padx 2 -pady 2 -side left
    pack $w.t -anchor center -expand 1 -fill both -padx 2 -pady 2 -side right
    $w.t insert 1.0 $text
}

proc label-scale {w label} {
    # utility function: create a frame with a label+scale
    frame $w
    label $w.l -anchor w -width 16 -text $label
    scale $w.e -orient horizontal
    pack $w.l -anchor center -expand 0 -fill none -padx 2 -pady 2 -side left
    pack $w.e -anchor center -expand 1 -fill x -padx 2 -pady 2 -side right
}

proc commentlabel {w text} {
    # utility function: create a frame with a message widget
    frame $w
    message $w.e -justify left -text $text -aspect 1000
    pack $w.e -anchor center -expand 0 -fill x -padx 2 -pady 2 -side left
}


proc notebook {w {side top}} {
    #  utility function: create an empty notebook widget
    global nb
    set nb($w) ""

    frame $w
    frame $w.tabs
    pack $w.tabs -side $side -fill x
}

proc notebook_addpage {w name label} {
    #  utility function: add page to notebook widget
    set tab $w.tabs.$name
    set page $w.$name

    frame $page -border 2 -relief raised
    button $tab -text $label -command "notebook_showpage $w $name" -relief flat
    pack $tab -anchor n -expand 0 -fill none -side left

    global nb
    if {$nb($w)==""} {notebook_showpage $w $name}
}

proc notebook_showpage {w name} {
    #  notebook internal function
    global nb

    if {$nb($w)==$name} return

    pack $w.$name -expand 1 -fill both
    $w.tabs.$name config -relief raised

    if {$nb($w)!=""} {
       pack forget $w.$nb($w)
       $w.tabs.$nb($w) config -relief flat
    }
    set nb($w) $name
}

proc label-check {w label first var } {
    # utility function: create a frame with a label+radiobutton for choose
    global gned

    frame $w
    label $w.l -anchor w -width 16 -text $label
    frame $w.f
    checkbutton $w.f.r1 -text $first -variable ned($var)

    pack $w.l -anchor w -expand 0 -fill none -side left
    pack $w.f -anchor w -expand 0 -side left -fill x
    pack $w.f.r1 -anchor w -expand 0 -side left
}

# notebook .x bottom
# notebook_addpage .x p1 Egy
# notebook_addpage .x p2 Ketto
# notebook_addpage .x p3 Harom
# pack .x -expand 1 -fill both
# label .x.p1.e -text "One"
# pack .x.p1.e

proc center {name} {
    # utility function: centers a dialog on the screen

    # withdraw the window, then update all the geometry information
    # so we know how big it wants to be, then center the window in the
    # display and de-iconify it.

    global tcl_platform

    set w $name
    if {$tcl_platform(platform) != "windows"} {
        wm withdraw $w
    }
    update idletasks
    set x [expr [winfo screenwidth $w]/2 - [winfo reqwidth $w]/2  - [winfo vrootx [winfo parent $w]]]
    set y [expr [winfo screenheight $w]/2 - [winfo reqheight $w]/2  - [winfo vrooty [winfo parent $w]]]
    wm geom $w +$x+$y
    if {$tcl_platform(platform) != "windows"} {
        wm deiconify $w
    }
    focus $w
}

proc createOkCancelDialog {w title} {
    # creates dialog with OK and Cancel buttons
    # user's widgets can go into frame $w.f

    catch {destroy $w}

    toplevel $w -class Toplevel
    wm title $w $title
    wm iconname $w Dialog
    wm focusmodel $w passive
    wm overrideredirect $w 0
    wm resizable $w 1 1
    wm deiconify $w
    wm protocol $w WM_DELETE_WINDOW { }
    #bind $w <Visibility> "raise $w"  ;# Keep modal window on top

    frame $w.f
    frame $w.buttons
    button $w.buttons.okbutton  -text {  OK  }
    button $w.buttons.cancelbutton  -text {Cancel}

    pack $w.buttons -expand 0 -fill x -padx 5 -pady 5 -side bottom
    pack $w.f -expand 1 -fill both -padx 5 -pady 5 -side top
    pack $w.buttons.cancelbutton  -anchor n -side right
    pack $w.buttons.okbutton  -anchor n -side right

}

proc execOkCancelDialog w {
    # execs the dialog
    # returns 1 if Ok was pressed, 0 if Cancel was pressed

    global opp

    $w.buttons.okbutton configure -command "set opp($w) 1"
    $w.buttons.cancelbutton configure -command "set opp($w) 0"

    bind $w <Return> "if {\[winfo class \[focus\]\]!=\"Text\"} {set opp($w) 1}"
    bind $w <Escape> "set opp($w) 0"

    wm protocol $w WM_DELETE_WINDOW "set opp($w) 0"

    center $w

    set oldGrab [grab current $w]
    if {$oldGrab != ""} {
        set grabStatus [grab status $oldGrab]
    }
    grab $w

    # Wait for the user to respond, then restore the focus and
    # return the index of the selected button.  Restore the focus
    # before deleting the window, since otherwise the window manager
    # may take the focus away so we can't redirect it.  Finally,
    # restore any grab that was in effect.

    tkwait variable opp($w)

    if {$oldGrab != ""} {
        if {$grabStatus == "global"} {
            grab -global $oldGrab
        } else {
            grab $oldGrab
        }
    }
    return $opp($w)
}
