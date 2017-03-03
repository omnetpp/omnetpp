#=================================================================
#  DIALOGUTILS.TCL - part of
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


# createOkCancelDialog --
#
# creates dialog with OK and Cancel buttons
# user's widgets can go into frame $w.f
#
proc createOkCancelDialog {w title {restoregeometry 0}} {
    global config
    catch {destroy $w}
    toplevel $w -class Toplevel
    wm transient $w [winfo toplevel [winfo parent $w]]
    wm title $w $title
    wm iconname $w Dialog
    wm focusmodel $w passive
    wm overrideredirect $w 0
    wm resizable $w 1 1
    wm deiconify $w
    wm protocol $w WM_DELETE_WINDOW { }

    if {$restoregeometry} {
        restoreDialogGeometry $w
    } else {
        centerDialog $w
    }

    # $w.r is a workaround: the dialog must contain exactly one ttk::frame,
    # otherwise toplevel's non-themed background will be visible through the gaps
    ttk::frame $w.r
    pack $w.r -expand 1 -fill both

    set padx 5
    set pady 5
    if {[string equal [tk windowingsystem] aqua]}  {
        set padx 15
        set pady 10
    }

    ttk::frame $w.f
    ttk::frame $w.buttons
    pack $w.buttons -in $w.r -expand 0 -fill x -padx $padx -pady $pady -side bottom
    pack $w.f -in $w.r  -expand 1 -fill both -padx $padx -pady $pady -side top

    ttk::button $w.buttons.okbutton  -text "OK" -width 10 -default active
    ttk::button $w.buttons.cancelbutton  -text "Cancel" -width 10
    pack $w.buttons.cancelbutton -side right -padx 2
    pack $w.buttons.okbutton -side right -padx 2
}


# execOkCancelDialog --
#
# Executes the dialog.
# Optional validating proc may check if fields are correctly
# filled in -- it should return 1 if dialog contents is valid,
# 0 if there are invalid fields and OK button should not be
# accepted.
#
# Returns 1 if Ok was pressed, 0 if Cancel was pressed.
#
proc execOkCancelDialog {w {validating_proc {}}} {

    global opp

    $w.buttons.okbutton configure -command "set opp($w) 1"
    catch {$w.buttons.cancelbutton configure -command "set opp($w) 0"}

    bind $w <Return> "if {\[winfo class \[focus\]\]!=\"Text\"} {set opp($w) 1}"
    bind $w <Escape> "set opp($w) 0"

    wm protocol $w WM_DELETE_WINDOW "set opp($w) 0"

    # next line mysteriously solves "lost focus" problem of popup dialogs...
    after 1 "wm deiconify $w"

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

    if {$validating_proc==""} {
        tkwait variable opp($w)
    } else {
        tkwait variable opp($w)
        while {$opp($w)==1 && ![eval $validating_proc $w]} {
            tkwait variable opp($w)
        }
    }

    saveDialogGeometry $w

    if {$oldGrab != ""} {
        if {$grabStatus == "global"} {
            grab -global $oldGrab
        } else {
            grab $oldGrab
        }
    }
    return $opp($w)
}

# createCloseDialog --
#
# Creates dialog with a Close button.
# User's widgets can go into frame $w.f, and extra buttons can go into frame $w.buttons.
#
proc createCloseDialog {w title {restoregeometry 0}} {
    catch {destroy $w}
    toplevel $w -class Toplevel
    wm transient $w [winfo toplevel [winfo parent $w]]
    wm title $w $title
    wm iconname $w Dialog
    wm focusmodel $w passive
    wm overrideredirect $w 0
    wm resizable $w 1 1
    wm deiconify $w
    wm protocol $w WM_DELETE_WINDOW { }

    if {$restoregeometry} {
        restoreDialogGeometry $w
    } else {
        centerDialog $w
    }

    # $w.r is a workaround: the dialog must contain exactly one ttk::frame,
    # otherwise toplevel's non-themed background will be visible through the gaps
    ttk::frame $w.r
    pack $w.r -expand 1 -fill both

    set padx 5
    set pady 5
    if {[string equal [tk windowingsystem] aqua]}  {
        set padx 15
        set pady 10
    }

    ttk::frame $w.f
    ttk::frame $w.buttons
    pack $w.buttons -in $w.r -expand 0 -fill x -padx $padx -pady $pady -side bottom
    pack $w.f -in $w.r  -expand 1 -fill both -padx $padx -pady $pady -side top

    ttk::button $w.buttons.closebutton  -text "Close" -width 10 -default active
    pack $w.buttons.closebutton  -anchor n -side right -padx 2
}


# executeCloseDialog --
#
# Executes the dialog.
#
proc executeCloseDialog w {
    global opp

    $w.buttons.closebutton configure -command "set opp($w) 1"

    #bind $w <Return> "if {\[winfo class \[focus\]\]!=\"Text\"} {set opp($w) 1}"
    bind $w <Escape> "set opp($w) 0"

    wm protocol $w WM_DELETE_WINDOW "set opp($w) 0"

    # next line mysteriously solves "lost focus" problem of popup dialogs...
    after 1 "wm deiconify $w"

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

    saveDialogGeometry $w

    if {$oldGrab != ""} {
        if {$grabStatus == "global"} {
            grab -global $oldGrab
        } else {
            grab $oldGrab
        }
    }
    return $opp($w)
}


proc saveDialogGeometry {w} {
    global config session

    # parse geometry string
    set geom [wm geometry $w]
    if {![regexp {^([0-9]+)x([0-9]+)\+(-?[0-9]+)\+(-?[0-9]+)$} $geom dummy width height x y]} {
        error "unexpected window geometry string $geom"
    }

    # size: preserve it across sessions because we need it for centering the dialog on the screen
    # position: remember for this session, but forget for new sessions to prevent accidents
    regsub {^.*\.} $w "" w0
    set session($w0:pos) [list $x $y]
    set config($w0:size) [list $width $height]
}

proc centerDialog {w} {
    # use the stored size for computing the position
    global config
    regsub {^.*\.} $w "" w0
    if {![info exists config($w0:size)]} {error "config($w0:size) missing, should be set to dialog's expected size"}
    setvars {width height} $config($w0:size)
    setvars {x y} [getCenteredDialogPos $width $height [winfo parent $w]]
    wm geometry $w "+$x+$y"
}

proc restoreDialogGeometry {w} {
    global config session
    regsub {^.*\.} $w "" w0
    if {![info exists config($w0:size)]} {error "config($w0:size) missing, should be set to dialog's expected size"}
    if {[info exists session($w0:pos)]} {
        setvars {width height} $config($w0:size)
        setvars {x y} $session($w0:pos)
        set geom "${width}x${height}+$x+$y"
        wm geometry $w $geom
    } else {
        centerDialog $w
    }
}

proc getCenteredDialogPos {width height parentwin} {
    # compute x and y
    if {$parentwin==""} {
        # Note: this solution has a problem with multi-monitor support, dialogs
        # end up being centered in the space which is the *union* of all screens
        # (often resulting in the dialog being split between two monitors).
        set x [expr { ( [winfo vrootwidth  .] - $width  ) / 2 }]
        set y [expr { ( [winfo vrootheight .] - $height ) / 2 }]
    } else {
        # Preferred solution: center above the parent window
        set centerx [expr { [winfo x $parentwin] + [winfo width $parentwin]/2 }]
        set centery [expr { [winfo y $parentwin] + [winfo height $parentwin]/2 }]
        set x [expr { $centerx - $width / 2 }]
        set y [expr { $centery - $height / 2 }]
    }

    return [list $x $y]
}

#
# Focuses the given widget.
#
# Sounds simple, right? Well, on the Aqua version of Tk, the focus command
# apparently gets ignored if the containing dialog is not already focused.
# In that case, we'll keep trying in the background until we succeed.
#
proc setInitialDialogFocus w {
    focus $w

    if {[string equal [tk windowingsystem] aqua]}  {
        set f [focus]
        if {$f != $w} {
            after 10 [list setInitialDialogFocusIfStillExists $w]   ;# retry after 10ms
        }
    }
}

proc setInitialDialogFocusIfStillExists w {
    if [winfo exist $w] {
        setInitialDialogFocus $w
    }
}

#
# Focuses the given widget. Only exists because of Aqua.
#
proc waitForFocus w {
    focus $w

    if {[string equal [tk windowingsystem] aqua]}  {
        while {[focus] != $w} {
            focus $w
            update
        }
    }
}

#
# Brings the window to front, and gives it focus
#
proc showWindow {w} {
    global tcl_platform
    if {$tcl_platform(platform) != "windows"} {
        # looks like some X servers ignore the "raise" command unless we
        # kick them by "wm withdraw" plus "wm deiconify"...
        wm withdraw $w
        wm deiconify $w
    }
    raise $w
    focus $w
}

#
# Returns the bounds of the space of the screen that is commonly available for
# application windows. This is the screen area minus the system menu area,
# taskbar area, etc if they exist. This method tries to err on the safe side,
# i.e. rather report a smaller area as usable.
#
proc wmGetDesktopBounds {w arrayname} {
    global tcl_platform
    upvar $arrayname a

    set a(x) [winfo vrootx $w]

    set topmargin 0
    if {$tcl_platform(platform) != "windows"} {set topmargin 20}  ;# menu bar of OS X, Gnome, etc.
    set rooty [winfo vrooty $w]
    set a(y) [expr $rooty + $topmargin]

    set screenwidth [winfo screenwidth $w]
    set a(width) $screenwidth

    set vmargin 50  ;# lost space at top+bottom (due to system menu, taskbar, etc)
    set screenheight [winfo screenheight $w]
    set a(height) [expr $screenheight-$vmargin]

    set a(x2) [expr $a(x)+$a(width)]
    set a(y2) [expr $a(y)+$a(height)]

    #puts "desktop: $a(width)x$a(height) at ($a(x),$a(y))"
}

#
# Returns the typical sizes of window decoration (title bar, resize borders, etc.)
#
proc wmGetDecorationSize {arrayname} {
    upvar $arrayname a
    set a(left) 10
    set a(top) 25
    set a(right) 10
    set a(bottom) 10
    set a(width) [expr $a(left)+$a(right)]
    set a(height) [expr $a(top)+$a(bottom)]
}

# moveToScreen --
#
# utility function: move a toplevel window so that it is fully on the screen
#
proc moveToScreen {w} {
    global tcl_platform

    # Note:
    # - winfo width/height returns the size WITHOUT window manager decorations (title bar, handle border)
    # - winfo x/y returns the position WITHOUT window manager decorations
    # - wm geometry:
    #   - gets/sets the size WITHOUT window manager decorations (title bar, handle border);
    #   - BUT position is WITH decorations, i.e. top-left coordinate of the window's title bar
    # That is, "winfo x/y" and "wm geometry" are NOT compatible!
    # This seems to apply on all platforms. (Tested on Win7, KDE, OS X.)
    #

    update idletasks  ;# needed to get "winfo" and "geom" information updated

    set geom [wm geom $w]
    regexp -- {.*\+(-?[0-9]+)\+(-?[0-9]+)} $geom dummy x y
    set oldx $x
    set oldy $y
    set width [winfo width $w]
    set height [winfo height $w]
    wmGetDesktopBounds $w "desktop"
    wmGetDecorationSize "border"

    # note: x,y are top-left of the wm title bar, unlike width/height which are size of the window contents

    if {$width+$border(width) >= $desktop(width) || $x < $desktop(x)} {
        set x $desktop(x)
    } elseif {$x+$width+$border(width) > $desktop(x2)} {
        set x [expr $desktop(x2)-$border(width)-$width]
    }

    if {$height+$border(height) >= $desktop(height) || $y < $desktop(y)} {
        set y $desktop(y)
    } elseif {$y+$height+$border(height) > $desktop(y2)} {
        set y [expr $desktop(y2)-$border(height)-$height]
    }

    if {$x!=$oldx || $y!=$oldy} {
        # move the window
        if {$tcl_platform(platform) != "windows"} {wm withdraw $w}
        update idletasks  ;# needed
        wm geom $w +$x+$y
        if {$tcl_platform(platform) != "windows"} {wm deiconify $w}
    }
}

#
# Make a window transient and look like a "tool" window (if supported by the
# platform). Used for inspector windows.
#
proc makeTransient {w {geom ""}} {
    # Drawbacks of "transient": on Windows and OS X, inspector windows cannot be
    # minimized or maximized; on OS X, inspectors move together with the main window
    # (but this is not necessarily bad)
    # Note: "wm attribute $w -topmost 1" is no substitute for transient -- it keeps
    # the window above ALL OTHER apps' windows as well
    wm transient $w .

    # Platform-specific additional configuration
    if {[string equal [tk windowingsystem] win32]} {
        # On Windows, toolwindow=1 makes the caption bar small
        wm attribute $w -toolwindow 1
    } elseif {[string equal [tk windowingsystem] x11]} {
        # "positionfrom" is for KDE 4.x, where minimize+restore would lose window position otherwise
        wm attribute $w -type normal
        wm positionfrom $w user
    }
}

proc placeWindowNearMouse {w} {
    # Position the window explicitly. This is needed because most platforms place the
    # window inconveniently, e.g. Windows and KDE (kwin) places them at (0,0); OS X also
    # near (0,0). "positionfrom" doesn't help. So place window explicitly, near the current
    # mouse position. An additional random jitter is useful when multiple transient windows
    # are created at the same time.
    set x [expr [winfo pointerx .] - 40 + int(rand()*20)]
    set y [expr [winfo pointery .] + 30 + int(rand()*20)]
    wm geometry $w +$x+$y
}



