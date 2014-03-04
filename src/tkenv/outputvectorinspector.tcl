#=================================================================
#  OUTPUTVECTORINSPECTOR.TCL - part of
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


proc createOutputVectorInspector {name geom} {
    global icons

    # create histogram inspector
    set w $name
    createInspectorToplevel $w $geom

    # make the window respond to resize events
    bind $w <Configure> "opp_refreshinspector $w"

    frame $w.main
    frame $w.bot
    pack $w.bot -anchor center -expand 0 -fill x -side bottom
    pack $w.main -anchor center -expand 1 -fill both -side top

    canvas $w.main.canvas -borderwidth 2 -relief raised -background wheat1
    pack $w.main.canvas -anchor center -expand 1 -fill both -side top

    label $w.bot.info -width 50 -relief groove
    ttk_button $w.bot.view -text {Options...} -command "OutputVectorInspector:options $w"
    pack $w.bot.view -anchor center -expand 0 -fill none -side right
    pack $w.bot.info -anchor center -expand 1 -fill x -side left

    $w.main.canvas bind all <Any-Enter> {OutputVectorInspector:mouse %W %x %y 1}
    $w.main.canvas bind all <Any-Leave> {OutputVectorInspector:mouse %W %x %y 0}

    # we need to let the window display, otherwise the canvas size
    # (needed by the first draw) returned by [winfo width/height ...]
    # will be 1
    update idletasks
}

proc OutputVectorInspector:options {win} {
    set w .ov-options
    catch {destroy $w}
    global tmp

    toplevel $w -class Toplevel
    wm iconname $w Dialog
    wm focusmodel $w passive
    wm overrideredirect $w 0
    wm resizable $w 1 1
    wm deiconify $w
    wm title $w {Plotting Options}
    wm protocol $w WM_DELETE_WINDOW { }

    labelframe $w.main -text {Options}
    checkbutton $w.main.auto -text {Autoscale time and value axes} -variable tmp(autoscale)
    label $w.main.lbl -text {Manual axis settings:}
    label-entry $w.main.time {Time scale (sec/px):}
    label-entry $w.main.ymin {Ymin:}
    label-entry $w.main.ymax {Ymax:}
    label-combo $w.main.combo {Style:} {dots pins bars sample-hold lines}
    label $w.main.pad -text { }

    frame $w.buttons
    ttk_button $w.buttons.okbutton -width 10 -text {OK}
    ttk_button $w.buttons.applybutton -width 10 -text {Apply}
    ttk_button $w.buttons.cancelbutton -width 10 -text {Cancel}

    pack $w.main -anchor center -expand 1 -fill both -side top -padx 5 -pady 5
    pack $w.main.auto -anchor w -expand 0 -fill none -padx 3m -side top
    pack $w.main.lbl -anchor w -expand 0 -fill none -padx 3m -side top
    pack $w.main.time -anchor center -expand 1 -fill x -padx 5m -side top
    pack $w.main.ymin -anchor center -expand 1 -fill x -padx 5m -side top
    pack $w.main.ymax -anchor center -expand 1 -fill x -padx 5m -side top
    pack $w.main.combo -anchor center -expand 1 -fill x -padx 3m -side top
    pack $w.main.pad -anchor center -expand 1 -fill x -padx 3m -side top

    pack $w.buttons  -anchor center -expand 0 -fill x -side bottom -padx 5 -pady 5
    pack $w.buttons.okbutton  -anchor n -expand 0 -side right
    pack $w.buttons.applybutton -anchor n -expand 0 -side right
    pack $w.buttons.cancelbutton  -anchor n -expand 0 -side right

    center $w

    # 2. Create bindings.
    global opp

    $w.buttons.okbutton configure -command "OutputVectorInspector:apply $w $win; set opp(button) 1"
    $w.buttons.applybutton configure -command "OutputVectorInspector:apply $w $win"
    $w.buttons.cancelbutton configure -command "set opp(button) 1"

    bind $w <Return> "OutputVectorInspector:apply $w $win; set opp(button) 1"
    bind $w <Escape> "set opp(button) 0"

    # 3. set initial values
    set settings [opp_inspectorcommand $win config]
    set tmp(autoscale) [lindex $settings 0]
    $w.main.time.e insert 0 [lindex $settings 1]
    $w.main.ymin.e insert 0 [lindex $settings 2]
    $w.main.ymax.e insert 0 [lindex $settings 3]
    $w.main.combo.e set [lindex $settings 4]

    # 4. Set a grab and claim the focus too.
    set oldFocus [focus]
    set oldGrab [grab current $w]
    if {$oldGrab != ""} {
        set grabStatus [grab status $oldGrab]
    }
    grab $w
    focus $w.main.time.e

    # 5. Wait for the user to respond, then restore the focus and
    # return the index of the selected button.  Restore the focus
    # before deleting the window, since otherwise the window manager
    # may take the focus away so we can't redirect it.  Finally,
    # restore any grab that was in effect.

    tkwait variable opp(button)

    catch {focus $oldFocus}
    destroy $w
    if {$oldGrab != ""} {
        if {$grabStatus == "global"} {
            grab -global $oldGrab
        } else {
            grab $oldGrab
        }
    }
}

proc OutputVectorInspector:apply {w win} {
    global tmp
    opp_inspectorcommand $win config \
                     $tmp(autoscale) \
                     [$w.main.time.e get] \
                     [$w.main.ymin.e get] \
                     [$w.main.ymax.e get] \
                     [$w.main.combo.e get]
    opp_refreshinspector $win
}

proc OutputVectorInspector:mouse {w x y on} {
    global opp
    # mouse enters/leaves a data point's drawing in a outvector window
    # the index of the point is in the drawing's tag: "value12"
    set tags [$w gettags current]
    if [regexp {.*value([0-9]+).*} $tags match value] {
       set win [winfo toplevel $w]
       set no_outline [catch {$w itemconfig current -outline}]
       if {$on == 1} {
           # mouse enters a bar/dot/line/etc: paint it black
           if {$no_outline} {
              set opp(oldfill)    [lindex [$w itemconfig current -fill] 4]
              $w itemconfig current -fill black
           } else {
              set opp(oldfill)    [lindex [$w itemconfig current -fill] 4]
              set opp(oldoutline) [lindex [$w itemconfig current -outline] 4]
              $w itemconfig current -fill black -outline black
           }
           $win.bot.info config -text [opp_inspectorcommand $win value $value]
       } else {
           # mouse leaves the bars/dots/lines: restore original color
           if {$no_outline} {
              $w itemconfig current -fill $opp(oldfill)
           } else {
              $w itemconfig current -fill $opp(oldfill) -outline $opp(oldoutline)
           }
           $win.bot.info config -text [opp_inspectorcommand $win value]
       }
    }
}


