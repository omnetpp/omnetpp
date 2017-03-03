#=================================================================
#  WIDGETS.TCL - part of
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


proc packToolbutton {w args} {
    eval toolbutton $w $args
    pack $w -anchor c -side left -padx 1 -pady 2 -ipadx 1 -ipady 1
}

proc rpackToolbutton {w args} {
    eval toolbutton $w $args
    pack $w -anchor c -side right -padx 1 -pady 2 -ipadx 1 -ipady 1
}

proc focusOrRoot {} {
    set f [focus]
    if {$f==""} {set f "."}
    return $f
}

proc label-entry {w label {text {}}} {
    # utility function: create a frame with a label+entry
    ttk::frame $w
    ttk::label $w.l -anchor w -width 16 -text $label
    ttk::entry $w.e
    pack $w.l -anchor center -expand 0 -fill x -padx 2 -pady 2 -side left
    pack $w.e -anchor center -expand 1 -fill x -padx 2 -pady 2 -side right
    $w.e insert 0 $text
}

proc label-entry-help {w label helptext {text {}}} {
    # utility function: create a frame with a label+entry
    global help_tips
    ttk::frame $w
    ttk::label $w.l -anchor w -text $label
    ttk::entry $w.e
    #label $w.h -anchor w -text "(Help)" -fg "#0000a0"
    set help_tips($w.l) $helptext
    set help_tips($w.e) $helptext
    #grid $w.l $w.e $w.h -sticky news
    grid $w.l $w.e -sticky news
    grid columnconfigure $w 1 -weight 1
    #bind $w.h <Button-1> [list helplabel:showhelp $helptext %X %Y]
    $w.e insert 0 $text
}

proc label-entry-chooser {w label text chooserproc} {
    # utility function: create a frame with a label+entry+button
    # the button is expected to call a dialog where the user can select
    # a value for the entry
    ttk::frame $w
    ttk::label $w.l -anchor w -width 16 -text $label
    ttk::entry $w.e
    ttk::button $w.c -text " ... " -command [list chooser:choose $w.e $chooserproc]
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

proc label-spinbox {w label {value {}}} {
    # utility function: create a frame with a label+spinbox
    ttk::frame $w
    ttk::label $w.l -anchor w -width 16 -text $label
    ttk::spinbox $w.e
    pack $w.l -anchor center -expand 0 -fill x -padx 2 -pady 2 -side left
    pack $w.e -anchor center -expand 1 -fill x -padx 2 -pady 2 -side right
    $w.e set $value
}

proc label-sunkenlabel {w label {text {}}} {
    # utility function: create a frame with a label+"readonly entry"
    ttk::frame $w
    ttk::label $w.l -anchor w -width 16 -text $label
    ttk::label $w.e -relief groove -justify left -anchor w
    pack $w.l -anchor center -expand 0 -fill none -padx 2 -pady 2 -side left
    pack $w.e -anchor center -expand 1 -fill x -padx 2 -pady 2 -side right
    $w.e config -text $text
}

proc label-combo {w label list {text {}} {cmd {}}} {
    # utility function: create a frame with a label+combo
    ttk::frame $w
    ttk::label $w.l -anchor w -width 16 -text $label
    ttk::combobox $w.e -values $list
    pack $w.l -anchor center -expand 0 -fill none -padx 2 -pady 2 -side left
    pack $w.e -anchor center -expand 1 -fill x -padx 2 -pady 2 -side right

    if {$text != ""} {
        $w.e set $text
    } else {
        $w.e set [lindex $list 0]
    }
    if {$cmd != ""} {
        bind $w.e <<ComboboxSelected>> $cmd
    }
}

proc label-combo2 {w label list {text {}} {cmd {}}} {
    # start with empty combo box
    label-combo $w $label $list $text $cmd
    $w.e delete 0 end
}

proc label-fontcombo {w label fontname} {
    # utility function: create a frame with a label+combo for font selection
    ttk::frame $w
    ttk::label $w.l -anchor w -width 16 -text $label
    ttk::combobox $w.e
    ttk::combobox $w.s -width 4 -justify right

    grid $w.l $w.e $w.s -sticky news -padx 2 -pady 2
    grid columnconfigure $w 1 -weight 1

    $w.e configure -values [fontcombo:getfontfamilies]
    $w.s configure -values {4 5 6 7 8 9 10 11 12 13 14 15 16 18 20 22 24 26 28 32 36 40 44 48}

    array set attrs [font actual $fontname]
    $w.e set $attrs(-family)
    $w.s set $attrs(-size)
}

proc fontcombo:getfontfamilies {} {
    set list {}
    foreach family [lsort [font families]] {
        if {[regexp {^[A-Za-z]} $family] && [lsearch -exact $list $family]==-1} {
            lappend list $family
        }
    }
    return $list
}

proc fontcombo:updatefont {w fontname} {
    set family [$w.e get]
    set size [$w.s get]
    font configure $fontname -family $family -size $size  ;# leave weight unchanged
}

proc label-text {w label height {text {}}} {
    # utility function: create a frame with a label+text
    ttk::frame $w
    ttk::label $w.l -anchor w -width 16 -text $label
    text $w.t -height $height -width 40
    catch {$w.t config -undo true}
    pack $w.l -anchor n -expand 0 -fill none -padx 2 -pady 2 -side left
    pack $w.t -anchor center -expand 1 -fill both -padx 2 -pady 2 -side right
    $w.t insert 1.0 $text
}

proc label-scale {w label} {
    # utility function: create a frame with a label+scale
    ttk::frame $w
    ttk::label $w.l -anchor w -width 30 -text $label
    ttk::scale $w.e -orient horizontal
    pack $w.l -anchor center -expand 0 -fill none -padx 2 -pady 0 -side left
    pack $w.e -anchor center -expand 1 -fill x -padx 2 -pady 0 -side right
}

proc label-button {w label {text {}}} {
    # utility function: create a frame with label+button
    ttk::frame $w
    ttk::label $w.l -anchor w -width 16 -text $label
    ttk::button $w.e
    pack $w.l -anchor center -expand 0 -fill none -padx 2 -pady 2 -side left
    pack $w.e -anchor center -expand 1 -fill x -padx 2 -pady 2 -side right
    $w.e config -text $text
}

proc label-check {w label first var} {
    # utility function: create a frame with a label+radiobutton for choose
    ttk::frame $w
    ttk::label $w.l -anchor w -width 16 -text $label
    ttk::frame $w.f
    ttk::checkbutton $w.f.r1 -text $first -variable $var

    pack $w.l -anchor w -expand 0 -fill none -side left
    pack $w.f -anchor w -expand 0 -side left -fill x
    pack $w.f.r1 -anchor w -expand 0 -side left
}

# label-colorchooser --
#
# utility function: create a frame with a label and a color chooser button
#
proc label-colorchooser {w label {color ""}} {
    global gned

    ttk::frame $w
    ttk::label $w.l -anchor w -width 16 -text $label
    ttk::frame $w.f
    pack $w.l -anchor nw -expand 0 -fill none -side left -padx 2 -pady 2
    pack $w.f -anchor n -expand 1 -fill x -side left

    ttk::entry $w.f.e
    pack $w.f.e -anchor center -expand 1 -fill x -side left -padx 2 -pady 2

    ttk::button $w.f.b -relief groove -command [list colorChooser:setColor $w.f.b $w.f.e [winfo toplevel $w]] -width 6
    pack $w.f.b -anchor c -expand 0 -fill none -side left -padx 2 -pady 2

    $w.f.e insert 0 $color
    set dispcolor [resolveDispStrColor $color ""]
    if {$dispcolor!=""} {
       $w.f.b config -background $dispcolor -activebackground $dispcolor
    }
}

proc colorChooser:setColor {b e pwin} {
    global gned

    set color [tk_chooseColor -parent $pwin]
    if {$color!=""} {
        $b config -background $color -activebackground $color
        $e delete 0 end
        $e insert 0 $color
        $e selection range 0 end
    }
}

proc commentlabel {w text {preferredwidth 60}} {
    # Note: make sure to specify -fill x when packing!
    ttk::label $w -text $text -anchor nw -justify left -width $preferredwidth
    bind $w <Configure> {%W configure -wraplength [expr {%w-4}] }
}

proc labelwithhelp {w text helptext} {
    global help_tips
    ttk::frame $w
    ttk::label $w.l -justify left -text $text
    #label $w.h -justify left -text "(Help)" -fg "#0000a0"
    set help_tips($w.l) $helptext
    #pack $w.l $w.h -expand 0 -side left -anchor center -fill none -padx 2 -pady 2
    pack $w.l -expand 0 -side left -anchor center -fill none -padx 2 -pady 2
    #bind $w.h <Button-1> [list helplabel:showhelp $helptext %X %Y]
}

# do not use - DOES NOT work on Linux (focus problems...)
proc helplabel:showhelp {text x y} {
    global help_tips
    catch {destroy .helpwin}
    toplevel .helpwin -relief flat
    wm overrideredirect .helpwin true
    wm positionfrom .helpwin program
    wm geometry .helpwin "+[expr $x-200]+[expr $y+5]"
    wm transient .helpwin [winfo toplevel $w]
    ttk::label .helpwin.tip -text $text -wraplength $help_tips(width) \
                            -background $help_tips(color) -border 1 -relief solid \
                            -font TkTooltipFont -justify left -takefocus 1
    pack .helpwin.tip
    bind .helpwin.tip <Return> "catch { destroy .helpwin }"
    bind .helpwin.tip <Escape> "catch { destroy .helpwin }"
    bind .helpwin.tip <FocusOut> "catch { destroy .helpwin }"
    bind .helpwin.tip <Button-1> "catch { destroy .helpwin }"
    waitForFocus .helpwin.tip
}

proc ttkTreeview:deleteAll {tree} {
    $tree delete [$tree children {}]
}

proc panedwindow:getSashPosition {w} {
    set pos [$w sash coord 0]
    if {[$w cget -orient]=="horizontal"} {
        return [lindex $pos 0]
    } else {
        return [lindex $pos 1]
    }
}

proc panedwindow:setSashPosition {w pos} {
    global sash
    # This essentially does {$w sash place 0 $pos}, but well:
    # 1. "sash place" does nothing if child hasn't been laid out yet, so we may need "after idle"
    # 2. "sash place" allows 0px too, which makes the sash completely unnoticeable to the user
    # 3. allows pos=="" for convenience (it is a no-op)
    if {$pos == ""} {return}
    if {$pos < 5} {set pos 5}
    set sash($w:retrycount) 0
    panedwindow:trySetSashPosition $w $pos
}

proc panedwindow:trySetSashPosition {w pos} {
    panedwindow:doSetSashPosition $w $pos
    if {[panedwindow:getSashPosition $w] != $pos} {
        global sash
        incr sash($w:retrycount)
        if {$sash($w:retrycount) < 50} {
            # note: "after idle" doesn't work on Linux, only on Windows
            after 100 [list panedwindow:trySetSashPosition $w $pos]
        }
    }
}

proc panedwindow:doSetSashPosition {w pos} {
    if {[$w cget -orient]=="horizontal"} {
        $w sash place 0 $pos 0
    } else {
        $w sash place 0 0 $pos
    }
}

#===================================================================
#    UTILITY PROCEDURES
#===================================================================

#
# Assigns variables from a list. Example: setvars {a b c} {1 2 3}
#
proc setvars {vars vals} {
    if {[llength $vars] != [llength $vals]} {error "number of vars and length of values list don't match"}
    uplevel [list foreach $vars $vals {}]
}

proc lcontains {list item} {
    set i [lsearch -exact $list $item]
    return [expr $i != -1]
}

proc uniq {list} {
    set result {}
    foreach elem $list {
        if {[lsearch -exact $result $elem] == -1 } {
            lappend result $elem
        }
    }
    return $result
}

#
# Dictionary compare
#
proc strdictcmp {s1 s2} {
    if {$s1 eq $s2} {
        return 0
    } else {
        # there's no dictionary comparison in Tcl, only dictionary sort...
        set ordered [lsort -dictionary [list $s1 $s2]]
        if {[lindex $ordered 0] eq $s1} {
            return -1
        } else {
            return 1
        }
    }
}

# wsize --
#
# Utility to set a widget's size to exactly width x height pixels.
# Usage example:
#    button .b1 -text "OK"
#    pack [wsize .b1 40 40] -side top -expand 1 -fill both
#
proc wsize {w width height} {
    set f ${w}_f
    ttk::frame $f -width $width -height $height
    place $w -in $f -x 0 -y 0 -width $width -height $height
    raise $w
    return $f
}


#
# Copies the given string to the clipboard
#
proc setClipboard {str} {
    clipboard clear
    clipboard append -- $str
}

proc busy {{msg {}}} {
    if {$msg != ""} {
        #$statusbar.mode config -text $msg
        # note: next 2 lines are useless (on Windows at least)
        . config -cursor watch
        update idletasks
    } else {
        #$statusbar.mode config -text "Ready"
        . config -cursor ""
        update idletasks
    }
}

proc bindRec {w event handler} {
    if {[winfo class $w]!="Toplevel"} { # Tk bug: handler is invoked twice: once by specific widget, and also by the toplevel
        bind $w $event $handler
    }
    foreach c [winfo children $w] {
        bindRec $c $event $handler
    }
}

#
# Make a text widget generate a <<CursorMove>> event when the user
# moves the cursor via keyboard/mouse
#
proc addCursorMoveEvent {txt} {
    # note: "after idle" is mandatory, otherwise listeners will see the OLD cursor position
    bind $txt <Key> +[list after idle "addCursorMoveEvent:fire $txt"]
    bind $txt <ButtonRelease-1> +[list after idle "addCursorMoveEvent:fire $txt"]
}

proc addCursorMoveEvent:fire {w} {
    if [winfo exists $w] {
        event generate $w <<CursorMove>>
    }
}

proc addScrollbars {w {ingrid ""}} {
    set f [winfo parent $w]
    if {$ingrid!=""} {
        if {[llength [winfo children $ingrid]] > 0} {error "target frame $ingrid should be empty"}
    } else {
        set ingrid $f
        if {[llength [winfo children $f]] > 1} {error "$w should be the only child within its parent, see [winfo children $f]"}
    }

    ttk::scrollbar $f.vsb -command "$w yview"
    ttk::scrollbar $f.hsb -orient horiz -command "$w xview"
    $w configure -xscrollcommand "$f.hsb set" -yscrollcommand "$f.vsb set"

    grid $w     -in $ingrid -row 0 -column 0 -sticky nsew
    grid $f.vsb -in $ingrid -row 0 -column 1 -sticky ns
    grid $f.hsb -in $ingrid -row 1 -column 0 -sticky ew

    grid columnconfigure $ingrid 0 -weight 1
    grid rowconfigure $ingrid 0 -weight 1

    ::autoscroll::autoscroll $f.vsb
    ::autoscroll::autoscroll $f.hsb
}

proc configureScrollbars {w vsb hsb} {
    $vsb configure -command "$w yview"
    $hsb configure -command "$w xview"
    $w configure -xscrollcommand "$hsb set" -yscrollcommand "$vsb set"
    ::autoscroll::autoscroll $vsb
    ::autoscroll::autoscroll $hsb
}

#
# Make a text widget generate a <<CursorRest>> event whenever the cursor
# is idle for 500ms after being moved by the user (via keyboard/mouse)
#
proc addCursorRestEvent {txt} {
    bind $txt <Key> "+addCursorRestEvent:onChange %W"
    bind $txt <ButtonRelease-1> "+addCursorRestEvent:onChange %W"
}

proc addCursorRestEvent:onChange {w} {
    global opp
    catch {after cancel $opp(cursorAfterID)}
    set opp(cursorAfterID) [after 100 "addCursorRestEvent:fire $w"]
}

proc addCursorRestEvent:fire {w} {
    if [winfo exists $w] {
        event generate $w <<CursorRest>>
    }
}

proc highlightcurrentline {txt} {
    bind $txt <Key> +[list after idle "highlightcurrentline:onchange $txt"]
    bind $txt <Button> +[list after idle "highlightcurrentline:onchange $txt"]
    bind $txt <<Selection>> +[list $txt tag remove linehighlight 1.0 end]
}

proc highlightcurrentline:onchange {txt} {
    $txt tag remove linehighlight 1.0 end
    if {[$txt tag ranges sel]==""} {
        $txt tag add linehighlight "insert linestart" "insert +1 line linestart"
    }
}

proc makereadonly {txt} {
    global Control B2
    bind $txt <$B2> "break" ;# disable paste by middle button
    bind $txt <Key> "+makereadonly:keypress %W %K" ;# CAUTION! This will mask global hotkeys!
    bind $txt <$Control-c> "tk_textCopy %W"
    bind $txt <$Control-Insert> "tk_textCopy %W"
}

proc makereadonly:keypress {w k} {
    if {$k ni {Up Down Left Right Home End Prior Next}} {
        return -code break
    }
}

#
# Sets up Ctrl+Wheel to generate <<ZoomIn>> and <<ZoomOut>> events.
# PROBLEMS WITH WHEEL-ZOOM: redraw is too slow, sluggish ==> poor user experience
# TODO: pass mouse position and amount of scroll to listeners (in global vars, there's no other way)
#
#proc setupZoomEvents {} {
#    global Control
#
#    if {[string equal [tk windowingsystem] win32]} {
#        # Windows: <MouseWheel> exists, %W is the FOCUSED widget not the one under mouse; %D is usually +-120, with some mouse +-28; or their multiples
#        bind all <$Control-MouseWheel> {
#            set w [winfo containing %X %Y]
#            if {%D > 0} {set e <<ZoomIn>>} else {set e <<ZoomOut>>}
#            event generate $w $e
#        }
#    } elseif {[string equal [tk windowingsystem] aqua]} {
#        # OS X: <MouseWheel> exists, %W is widget under mouse; %D is +-1 (or multiple)
#        bind all <$Control-MouseWheel> {
#            if {%D > 0} {set e <<ZoomIn>>} else {set e <<ZoomOut>>}
#            event generate %W $e
#        }
#    } elseif {[string equal [tk windowingsystem] x11]} {
#        # Linux: no <MouseWheel>, bind buttons 4 and 5 instead; %W is widget under mouse
#        bind all <$Control-Button-4> {event generate %W <<ZoomIn>>]}
#        bind all <$Control-Button-5> {event generate %W <<ZoomOut>>]}
#    }
#}

#
# Creates mouse wheel bindings for the given widget or widget class.
# Note: wheel events are only delivered to the widget IF IT HAS FOCUS!
#
# Code copied almost verbatim from lib/tk8.4/text.tcl.
#
proc bindMouseWheel {w} {
    # The MouseWheel will typically only fire on Windows.  However,
    # someone could use the "event generate" command to produce one
    # on other platforms.

    if {[string equal [tk windowingsystem] "classic"]
        || [string equal [tk windowingsystem] "aqua"]} {
        bind $w <MouseWheel> {
            if [canScrollY %W] {
                %W yview scroll [expr {- (%D)}] units
            }
        }
        bind $w <Option-MouseWheel> {
            if [canScrollY %W] {
                %W yview scroll [expr {-10 * (%D)}] units
            }
        }
    } else {
        bind $w <MouseWheel> {
            if [canScrollY %W] {
                %W yview scroll [expr {- (%D / 120) * 4}] units
            }
        }
    }

    if {[string equal "x11" [tk windowingsystem]]} {
        # Support for mousewheels on Linux/Unix commonly comes through mapping
        # the wheel to the extended buttons.  If you have a mousewheel, find
        # Linux configuration info at:
        #       http://www.inria.fr/koala/colas/mouse-wheel-scroll/
        bind $w <4> {
            if {!$tk_strictMotif && [canScrollY %W]} {
                %W yview scroll -5 units
            }
        }
        bind $w <5> {
            if {!$tk_strictMotif && [canScrollY %W]} {
                %W yview scroll 5 units
            }
        }
    }
}

proc canScrollY {w} {
    if {[winfo class $w]!="Canvas"} {
        return 1
    } else {
        set bbox [$w bbox all]
        if {$bbox == {}} {return 0}
        set contentHeight [expr [lindex $bbox 3] - [lindex $bbox 1]]
        set widgetHeight [winfo height $w]
        return [expr $contentHeight > $widgetHeight]
    }
}

proc getTextExtent {fontFamily pointSize weight slant text} {
    catch {destroy .c}

    tkp::canvas .c
    set id [.c create ptext 0 0 -text $text -fontfamily $fontFamily -fontsize $pointSize -fontweight $weight -fontslant $slant]
    set bbox [.c bbox $id]
    destroy .c

    catch {font delete tmp}
    font create tmp -family $fontFamily -size $pointSize -weight $weight
    set ascent [font metrics tmp -ascent]
    font delete tmp

    return [list {*}$bbox $ascent]
}
