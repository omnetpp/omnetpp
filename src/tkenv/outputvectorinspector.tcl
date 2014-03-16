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


proc createOutputVectorInspector {insp geom} {
    createInspectorToplevel $insp $geom
    createOutputVectorViewer $insp
}

proc createEmbeddedOutputVectorInspector {insp geom} {
    createOutputVectorViewer $insp
}

proc createOutputVectorViewer {insp} {
    frame $insp.main
    frame $insp.bot
    pack $insp.bot -anchor center -expand 0 -fill x -side bottom
    pack $insp.main -anchor center -expand 1 -fill both -side top

    canvas $insp.main.canvas -borderwidth 2 -relief raised -background wheat1
    pack $insp.main.canvas -anchor center -expand 1 -fill both -side top

    label $insp.bot.info -width 50 -relief groove
    ttk_button $insp.bot.view -text "Options..." -command "OutputVectorInspector:options $insp"
    pack $insp.bot.view -anchor center -expand 0 -fill none -side right
    pack $insp.bot.info -anchor center -expand 1 -fill x -side left

    $insp.main.canvas bind all <Any-Enter> "OutputVectorInspector:mouse $insp %x %y 1"
    $insp.main.canvas bind all <Any-Leave> "OutputVectorInspector:mouse $insp %x %y 0"

    # make the window respond to resize events
    bind $insp <Configure> "opp_refreshinspector $insp"

    # we need to let the window display, otherwise the canvas size
    # (needed by the first draw) returned by [winfo width/height ...]
    # will be 1
    update idletasks
}

proc OutputVectorInspector:options {insp} {
    set dlg .ov-options
    catch {destroy $dlg}
    global tmp

    toplevel $dlg -class Toplevel
    wm iconname $dlg Dialog
    wm focusmodel $dlg passive
    wm overrideredirect $dlg 0
    wm resizable $dlg 1 1
    wm deiconify $dlg
    wm title $dlg "Plotting Options"
    wm protocol $dlg WM_DELETE_WINDOW { }

    labelframe $dlg.main -text "Options"
    checkbutton $dlg.main.auto -text "Autoscale time and value axes" -variable tmp(autoscale)
    label $dlg.main.lbl -text "Manual axis settings:"
    label-entry $dlg.main.time "Time scale (sec/px):"
    label-entry $dlg.main.ymin "Ymin:"
    label-entry $dlg.main.ymax "Ymax:"
    label-combo $dlg.main.combo "Style:" {dots pins bars sample-hold lines}
    label $dlg.main.pad -text " "

    frame $dlg.buttons
    ttk_button $dlg.buttons.okbutton -width 10 -text "OK"
    ttk_button $dlg.buttons.applybutton -width 10 -text "Apply"
    ttk_button $dlg.buttons.cancelbutton -width 10 -text "Cancel"

    pack $dlg.main -anchor center -expand 1 -fill both -side top -padx 5 -pady 5
    pack $dlg.main.auto -anchor w -expand 0 -fill none -padx 3m -side top
    pack $dlg.main.lbl -anchor w -expand 0 -fill none -padx 3m -side top
    pack $dlg.main.time -anchor center -expand 1 -fill x -padx 5m -side top
    pack $dlg.main.ymin -anchor center -expand 1 -fill x -padx 5m -side top
    pack $dlg.main.ymax -anchor center -expand 1 -fill x -padx 5m -side top
    pack $dlg.main.combo -anchor center -expand 1 -fill x -padx 3m -side top
    pack $dlg.main.pad -anchor center -expand 1 -fill x -padx 3m -side top

    pack $dlg.buttons  -anchor center -expand 0 -fill x -side bottom -padx 5 -pady 5
    pack $dlg.buttons.okbutton  -anchor n -expand 0 -side right
    pack $dlg.buttons.applybutton -anchor n -expand 0 -side right
    pack $dlg.buttons.cancelbutton  -anchor n -expand 0 -side right

    center $dlg

    # 2. Create bindings.
    global opp

    $dlg.buttons.okbutton configure -command "OutputVectorInspector:apply $dlg $insp; set opp(button) 1"
    $dlg.buttons.applybutton configure -command "OutputVectorInspector:apply $dlg $insp"
    $dlg.buttons.cancelbutton configure -command "set opp(button) 1"

    bind $dlg <Return> "OutputVectorInspector:apply $dlg $insp; set opp(button) 1"
    bind $dlg <Escape> "set opp(button) 0"

    # 3. set initial values
    set settings [opp_inspectorcommand $insp config]
    set tmp(autoscale) [lindex $settings 0]
    $dlg.main.time.e insert 0 [lindex $settings 1]
    $dlg.main.ymin.e insert 0 [lindex $settings 2]
    $dlg.main.ymax.e insert 0 [lindex $settings 3]
    $dlg.main.combo.e set [lindex $settings 4]

    # 4. Set a grab and claim the focus too.
    set oldFocus [focus]
    set oldGrab [grab current $dlg]
    if {$oldGrab != ""} {
        set grabStatus [grab status $oldGrab]
    }
    grab $dlg
    focus $dlg.main.time.e

    # 5. Wait for the user to respond, then restore the focus and
    # return the index of the selected button.  Restore the focus
    # before deleting the window, since otherwise the window manager
    # may take the focus away so we can't redirect it.  Finally,
    # restore any grab that was in effect.

    tkwait variable opp(button)

    catch {focus $oldFocus}
    destroy $dlg
    if {$oldGrab != ""} {
        if {$grabStatus == "global"} {
            grab -global $oldGrab
        } else {
            grab $oldGrab
        }
    }
}

proc OutputVectorInspector:apply {dlg insp} {
    global tmp
    opp_inspectorcommand $insp config \
                     $tmp(autoscale) \
                     [$dlg.main.time.e get] \
                     [$dlg.main.ymin.e get] \
                     [$dlg.main.ymax.e get] \
                     [$dlg.main.combo.e get]
    opp_refreshinspector $insp
}

proc OutputVectorInspector:mouse {insp x y on} {
    global opp

    # mouse enters/leaves a data point's drawing in a outvector window
    # the index of the point is in the drawing's tag: "value12"
    set c $insp.main.canvas
    set tags [$c gettags current]
    if [regexp {.*value([0-9]+).*} $tags match value] {
       set no_outline [catch {$c itemconfig current -outline}]
       if {$on == 1} {
           # mouse enters a bar/dot/line/etc: paint it black
           if {$no_outline} {
              set opp(oldfill)    [lindex [$c itemconfig current -fill] 4]
              $c itemconfig current -fill black
           } else {
              set opp(oldfill)    [lindex [$c itemconfig current -fill] 4]
              set opp(oldoutline) [lindex [$c itemconfig current -outline] 4]
              $c itemconfig current -fill black -outline black
           }
           $insp.bot.info config -text [opp_inspectorcommand $insp value $value]
       } else {
           # mouse leaves the bars/dots/lines: restore original color
           if {$no_outline} {
              $c itemconfig current -fill $opp(oldfill)
           } else {
              $c itemconfig current -fill $opp(oldfill) -outline $opp(oldoutline)
           }
           $insp.bot.info config -text [opp_inspectorcommand $insp value]
       }
    }
}


