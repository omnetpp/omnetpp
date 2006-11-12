#=================================================================
#  STATINSP.TCL - part of
#
#                     OMNeT++/OMNEST
#            Discrete System Simulation in C++
#
#=================================================================

#----------------------------------------------------------------#
#  Copyright (C) 1992-2005 Andras Varga
#
#  This file is distributed WITHOUT ANY WARRANTY. See the file
#  `license' for details on this and other legal matters.
#----------------------------------------------------------------#

#
# Histogram, output vector and statistic inspectors
#

proc histogramwindow_mouse {w x y on} {
    #set obj [$w find closest $x $y]
    set tags [$w gettags current]
    if [regexp {.*cell([0-9]+).*} $tags match cell] {
       set win [winfo toplevel $w]
       if {$on == 1} {
           $w itemconfig current -fill gray -outline gray
           $win.bot.info config -text [opp_inspectorcommand $win cell $cell]
       } else {
           $w itemconfig current -fill black -outline black
           $win.bot.info config -text [opp_inspectorcommand $win cell]
       }
    }
}

proc create_histogramwindow {name geom} {
    global icons help_tips

    # create histogram inspector
    set w $name
    create_inspector_toplevel $w $geom

    # make the window respond to resize events
    bind $w <Configure> "opp_updateinspector $w"

    pack_iconbutton $w.toolbar.obj -image $icons(asobject) -command "inspect_this $w {As Object}"
    set help_tips($w.toolbar.obj) {Inspect as object}

    frame $w.main
    frame $w.bot
    pack $w.bot  -expand 0 -fill x -side bottom
    pack $w.main -expand 1 -fill both -side top

    canvas $w.main.canvas -borderwidth 2 -relief raised -background lightblue
    pack $w.main.canvas -anchor center -expand 1 -fill both -side top

    label $w.bot.info -relief groove -width 50
    pack $w.bot.info -anchor center -expand 1 -fill x -side left

    $w.main.canvas bind all <Any-Enter> {histogramwindow_mouse %W %x %y 1}
    $w.main.canvas bind all <Any-Leave> {histogramwindow_mouse %W %x %y 0}

    # we need to let the window display, otherwise the canvas size
    # (needed by the first draw) returned by [winfo width/height ...]
    # will be 1
    update idletasks
}

proc outvectorwindow_opt_update {w win} {
    global tmp
    opp_inspectorcommand $win config \
                     $tmp(autoscale) \
                     [$w.time.e get] \
                     [$w.ymin.e get] \
                     [$w.ymax.e get] \
                     [$w.combo.e cget -value]
    opp_updateinspector $win
}

proc outvectorwindow_options {win} {
    set w .ov-options
    catch {destroy $w}
    global tmp

    ###################
    # CREATING WIDGETS
    ###################
    toplevel $w -class Toplevel
    wm iconname $w Dialog
    wm focusmodel $w passive
    wm overrideredirect $w 0
    wm resizable $w 1 1
    wm deiconify $w
    wm title $w {Options...}
    wm protocol $w WM_DELETE_WINDOW { }
    #bind $w <Visibility> "raise $w"  ;# Keep modal window on top

    label $w.msg -justify left -text {cOutVector window options:}
    checkbutton $w.auto -text {Autoscale} -variable tmp(autoscale)
    label-entry $w.time {Time factor (sec/pixel):}
    label-entry $w.ymin {Ymin:}
    label-entry $w.ymax {Ymax:}
    label-combo $w.combo {Draw mode:} {dots pins bars sample-hold lines}
    frame $w.buttons
    button $w.buttons.okbutton -width 10 -text {OK}
    button $w.buttons.applybutton -width 10 -text {Apply}
    button $w.buttons.cancelbutton -width 10 -text {Cancel}

    pack $w.msg  -anchor w -expand 0 -fill none -padx 3m -pady 3m -side top
    pack $w.auto -anchor w -expand 0 -fill none -padx 3m -pady 3m -side top
    pack $w.time -anchor center -expand 1 -fill x -padx 3m -pady 1m -side top
    pack $w.ymin -anchor center -expand 1 -fill x -padx 3m -pady 1m -side top
    pack $w.ymax -anchor center -expand 1 -fill x -padx 3m -pady 1m -side top
    pack $w.combo -anchor center -expand 1 -fill x -padx 3m -pady 1m -side top
    pack $w.buttons  -anchor center -expand 0 -fill x -side bottom -padx 5 -pady 5
    pack $w.buttons.okbutton  -anchor n -expand 0 -side right
    pack $w.buttons.applybutton -anchor n -expand 0 -side right
    pack $w.buttons.cancelbutton  -anchor n -expand 0 -side right

    center $w

    # 2. Create bindings.
    global opp

    $w.buttons.okbutton configure -command "outvectorwindow_opt_update $w $win; set opp(button) 1"
    $w.buttons.applybutton configure -command "outvectorwindow_opt_update $w $win"
    $w.buttons.cancelbutton configure -command "set opp(button) 1"

    bind $w <Return> "outvectorwindow_opt_update $w $win; set opp(button) 1"
    bind $w <Escape> "set opp(button) 0"

    # 3. set initial values
    set settings [opp_inspectorcommand $win config]
    set tmp(autoscale) [lindex $settings 0]
    $w.time.e insert 0 [lindex $settings 1]
    $w.ymin.e insert 0 [lindex $settings 2]
    $w.ymax.e insert 0 [lindex $settings 3]
    $w.combo.e configure -value [lindex $settings 4]

    # 4. Set a grab and claim the focus too.
    set oldFocus [focus]
    set oldGrab [grab current $w]
    if {$oldGrab != ""} {
        set grabStatus [grab status $oldGrab]
    }
    grab $w
    focus $w.time.e

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

proc outvectorwindow_mouse {w x y on} {
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

proc create_outvectorwindow {name geom} {
    global icons

    # create histogram inspector
    set w $name
    create_inspector_toplevel $w $geom

    # make the window respond to resize events
    bind $w <Configure> "opp_updateinspector $w"

    pack_iconbutton $w.toolbar.obj -image $icons(asobject) -command "inspect_this $w {As Object}"
    set help_tips($w.toolbar.obj) {Inspect as object}

    frame $w.main
    frame $w.bot
    pack $w.bot -anchor center -expand 0 -fill x -side bottom
    pack $w.main -anchor center -expand 1 -fill both -side top

    canvas $w.main.canvas -borderwidth 2 -relief raised -background wheat1
    pack $w.main.canvas -anchor center -expand 1 -fill both -side top

    label $w.bot.info -width 50 -relief groove
    button $w.bot.view -text {Options...} -command "outvectorwindow_options $w"
    pack $w.bot.view -anchor center -expand 0 -fill none -side right
    pack $w.bot.info -anchor center -expand 1 -fill x -side left

    $w.main.canvas bind all <Any-Enter> {outvectorwindow_mouse %W %x %y 1}
    $w.main.canvas bind all <Any-Leave> {outvectorwindow_mouse %W %x %y 0}

    # we need to let the window display, otherwise the canvas size
    # (needed by the first draw) returned by [winfo width/height ...]
    # will be 1
    update idletasks
}

#proc create_statisticinspector {name geom} {
#    global icons help_tips
#
#    set w $name
#    create_inspector_toplevel $w $geom
#
#    set nb [inspector_createnotebook $w]
#
#    inspector_createfields2page $w
#
#    notebook_addpage $nb info  {Info}
#
#    pack_iconbutton $w.toolbar.graph -image $icons(asgraphics) -command "inspect_this $w {As Graphics}"
#    set help_tips($w.toolbar.graph) {Inspect as histogram graphics}
#
#    label-sunkenlabel $nb.info.count Count:
#    label-sunkenlabel $nb.info.mean Mean:
#    label-sunkenlabel $nb.info.stddev Std.dev:
#    label-sunkenlabel $nb.info.min Min:
#    label-sunkenlabel $nb.info.max Max:
#
#    pack $nb.info.count -fill x -side top
#    pack $nb.info.mean -fill x -side top
#    pack $nb.info.stddev -fill x -side top
#    pack $nb.info.min -fill x -side top
#    pack $nb.info.max -fill x -side top
#}


