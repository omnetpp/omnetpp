#==========================================================================
#  WIDGETS.TCL -
#            part of the Tcl/Tk windowing environment of
#                            OMNeT++
#==========================================================================

#----------------------------------------------------------------#
#  Copyright (C) 1992-2001 Andras Varga
#  Technical University of Budapest, Dept. of Telecommunications,
#  Stoczek u.2, H-1111 Budapest, Hungary.
#
#  This file is distributed WITHOUT ANY WARRANTY. See the file
#  `license' for details on this and other legal matters.
#----------------------------------------------------------------#


# wsize --
#
# Utility to set a widget's size to exactly width x height pixels.
# Usage example:
#    button .b1 -text "OK"
#    pack [wsize .b1 40 40] -side top -expand 1 -fill both
#
proc wsize {w width height} {
  set f ${w}_f
  frame $f -width $width -height $height
  place $w -in $f -x 0 -y 0 -width $width -height $height
  raise $w
  return $f
}


#===================================================================
#    PROCEDURES FOR CREATING NEW 'WIDGET TYPES'
#===================================================================

proc iconbutton {w args} {
    global fonts

    if {$args=="-separator"} {
        # space between two buttons
        canvas $w -height 1 -width 4
    } {
        # button
        eval button $w -bd 1 $args
    }
    return $w
}

proc combo {w list {cmd {}}} {
    # implements a combo box widget (which is missing from Tk)
    # using a menubutton and a menu

    global fonts

    menubutton $w -menu $w.m -font $fonts(normal) -relief raised -width 14 -indicatoron 1
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

proc label-entry-chooser {w label text chooserproc} {
    # utility function: create a frame with a label+entry+button
    # the button is expected to call a dialog where the user can select
    # a value for the entry
    frame $w
    label $w.l -anchor w -width 16 -text $label
    entry $w.e -highlightthickness 0
    button $w.c -text " ... " -command [list chooser:choose $w.e $chooserproc]
    pack $w.l -anchor center -expand 0 -fill none -padx 2 -pady 2 -side left
    pack [wsize $w.c 20 20] -anchor center -expand 0 -fill none -padx 2 -pady 2 -side right
    pack $w.e -anchor center -expand 1 -fill x -padx 2 -pady 2 -side left
    $w.e insert 0 $text
}

# private proc for label-entry-chooser
proc chooser:choose {w chooserproc} {
    set current [$w get]
    set new [eval $chooserproc \"$current\"]
    if {$new!=""} {
       $w delete 0 end
       $w insert end $new
    }
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

proc label-check {w label first var} {
    # utility function: create a frame with a label+radiobutton for choose
    global gned

    frame $w
    label $w.l -anchor w -width 16 -text $label
    frame $w.f
    checkbutton $w.f.r1 -text $first -variable $var

    pack $w.l -anchor w -expand 0 -fill none -side left
    pack $w.f -anchor w -expand 0 -side left -fill x
    pack $w.f.r1 -anchor w -expand 0 -side left
}

proc vertResizeBar {w wToBeResized} {
    # create widget
    frame $w -width 5 -relief raised -borderwidth 1
    if [catch {$w config -cursor size_we}] {
      if [catch {$w config -cursor sb_h_double_arrow}] {
        catch {$w config -cursor sizing}
      }
    }

    # create bindings
    bind $w <Button-1> "vertResizeBar:buttonDown $w %X"
    bind $w <B1-Motion> "vertResizeBar:buttonMove %X"
    bind $w <ButtonRelease-1> "vertResizeBar:buttonRelease %X $wToBeResized"
    bind $w <Button-2> "catch {destroy .resizeBar}"
    bind $w <Button-3> "catch {destroy .resizeBar}"
}

proc vertResizeBar:buttonDown {w x} {
    global mouse
    set mouse(origx) $x

    catch {destroy .resizeBar}
    toplevel .resizeBar -relief flat -bg #606060
    wm overrideredirect .resizeBar true
    wm positionfrom .resizeBar program
    set geom "[winfo width $w]x[winfo height $w]+[winfo rootx $w]+[winfo rooty $w]"
    wm geometry .resizeBar $geom
}

proc vertResizeBar:buttonMove {x} {
    catch {wm geometry .resizeBar "+$x+[winfo rooty .resizeBar]"}
}

proc vertResizeBar:buttonRelease {x wToBeResized} {
    global mouse
    set dx [expr $x-$mouse(origx)]

    set width [$wToBeResized cget -width]
    set width [expr $width+$dx]
    $wToBeResized config -width $width

    catch {destroy .resizeBar}
}

# tableEdit --
#
# Create a "tableEdit" widget
#
# one $columnlist entry:
#   {title column-name command-to-create-widget-in-cell}
#
#  the command should use two variables:
#    $e - widget must be created with name stored in $e
#    $v - widget must be bound to variable whose name is in $v
#
# Example:
# tableEdit .t 20 {
#   {Name    name    {entry $e -textvariable $v -width 8 -bd 1 -relief sunken}}
#   {Value   value   {entry $e -textvariable $v -width 12 -bd 1 -relief sunken}}
#   {Comment comment {entry $e -textvariable $v -width 20 -bd 1 -relief sunken}}
# }
# pack .t -expand 1 -fill both
#
proc tableEdit {w numlines columnlist} {

    # clean up variables from earlier table instances with same name $w
    global tablePriv
    foreach i [array names tablePriv "$w,*"] {
        unset tablePriv($i)
    }

    # create widgets
    frame $w; # -bg green
    frame $w.tb -height 16
    canvas $w.c -yscrollcommand "$w.vsb set" -height 150 -bd 0
    scrollbar $w.vsb -command "$w.c yview"

    grid rowconfig $w 1 -weight 1 -minsize 0

    grid $w.tb -in $w -row 0 -column 0 -rowspan 1 -columnspan 1 -sticky news
    grid $w.c   -in $w -row 1 -column 0 -rowspan 1 -columnspan 1 -sticky news
    grid $w.vsb -in $w -row 1 -column 1 -rowspan 1 -columnspan 1 -sticky news

    frame $w.c.f -bd 0
    $w.c create window 0 0 -anchor nw -window $w.c.f

    set tb $w.tb
    set f $w.c.f

    for {set li 0} {$li<$numlines} {incr li} {
       set col 0
       foreach entry $columnlist {
           # get fields from entry
           set title   [lindex $entry 0]
           set attr    [lindex $entry 1]
           set wcmd    [lindex $entry 2]

           # add table entry
           set e $f.li$li-$attr
           set v tablePriv($w,$li,$attr)
           eval $wcmd
           grid $e -in $f -row $li -column $col -rowspan 1 -columnspan 1 -sticky news

           # next column
           incr col
       }
    }

    update idletasks

    # create title labels
    set dx 2
    foreach entry $columnlist {
        # get fields from entry
        set title   [lindex $entry 0]
        set attr    [lindex $entry 1]

        set e $f.li0-$attr
        label $tb.$attr -bd 1 -relief raised -text $title

        # add title bar
        set width [expr [winfo width $e]]
        place $tb.$attr -in $tb -x $dx -y 0 -width $width -height [winfo height $tb]
        set dx [expr $dx + $width]
    }

    # adjust canvas width to frame width
    $w.c config -width [winfo width $f]
    $w.c config -scrollregion "0 0 0 [winfo height $f]"

    #focus $w.l0c0

}


# notebook .x bottom
# notebook_addpage .x p1 Egy
# notebook_addpage .x p2 Ketto
# notebook_addpage .x p3 Harom
# pack .x -expand 1 -fill both
# label .x.p1.e -text "One"
# pack .x.p1.e

proc center {w} {
    # utility function: centers a dialog on the screen

    global tcl_platform

    # preliminary placement...
    if {[winfo reqwidth $w]!=0} {
       set pre_x [expr ([winfo screenwidth $w]-[winfo reqwidth $w])/2-[winfo vrootx [winfo parent $w]]]
       set pre_y [expr ([winfo screenheight $w]-[winfo reqheight $w])/2-[winfo vrooty [winfo parent $w]]]
       wm geom $w +$pre_x+$pre_y
    }

    # withdraw the window, then update all the geometry information
    # so we know how big it wants to be, then center the window in the
    # display and de-iconify it.
    if {$tcl_platform(platform) != "windows"} {
        wm withdraw $w
    }
    update idletasks
    set x [expr [winfo screenwidth $w]/2 - [winfo width $w]/2  - [winfo vrootx [winfo parent $w]]]
    set y [expr [winfo screenheight $w]/2 - [winfo height $w]/2  - [winfo vrooty [winfo parent $w]]]
    wm geom $w +$x+$y
    if {$tcl_platform(platform) != "windows"} {
        wm deiconify $w
    }
    focus $w
}

proc createOkCancelDialog {w title} {
    # creates dialog with OK and Cancel buttons
    # user's widgets can go into frame $w.f
    global tk_version tcl_platform

    catch {destroy $w}
    toplevel $w -class Toplevel
    if {$tk_version<8.2 || $tcl_platform(platform)!="windows"} {
        wm transient $w [winfo toplevel [winfo parent $w]]
    }
    wm title $w $title
    wm iconname $w Dialog
    wm focusmodel $w passive
    wm overrideredirect $w 0
    wm resizable $w 1 1
    wm deiconify $w
    wm protocol $w WM_DELETE_WINDOW { }

    # preliminary placement (assumes 350x250 dialog)...
    set pre_x [expr ([winfo screenwidth $w]-350)/2-[winfo vrootx [winfo parent $w]]]
    set pre_y [expr ([winfo screenheight $w]-250)/2-[winfo vrooty [winfo parent $w]]]
    wm geom $w +$pre_x+$pre_y

    frame $w.f
    frame $w.buttons
    button $w.buttons.okbutton  -text {  OK  }
    button $w.buttons.cancelbutton  -text {Cancel}

    pack $w.buttons -expand 0 -fill x -padx 5 -pady 5 -side bottom
    pack $w.f -expand 1 -fill both -padx 5 -pady 5 -side top
    pack $w.buttons.cancelbutton  -anchor n -side right -padx 2
    pack $w.buttons.okbutton  -anchor n -side right -padx 2
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
