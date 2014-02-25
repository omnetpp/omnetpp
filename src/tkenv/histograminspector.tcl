#=================================================================
#  HISTOGRAMINSPECTOR.TCL - part of
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


proc createHistogramWindow {name geom} {
    global icons help_tips

    # create histogram inspector
    set w $name
    createInspectorToplevel $w $geom

    # make the window respond to resize events
    bind $w <Configure> "opp_updateinspector $w"

    packIconButton $w.toolbar.obj -image $icons(asobject) -command "inspectThis $w {As Object}"
    set help_tips($w.toolbar.obj) {Inspect as object}

    frame $w.main
    frame $w.bot
    pack $w.bot  -expand 0 -fill x -side bottom
    pack $w.main -expand 1 -fill both -side top

    canvas $w.main.canvas -borderwidth 2 -relief raised -background lightblue
    pack $w.main.canvas -anchor center -expand 1 -fill both -side top

    label $w.bot.info -relief groove -width 50
    pack $w.bot.info -anchor center -expand 1 -fill x -side left

    $w.main.canvas bind all <Any-Enter> {histogramWindow:mouse %W %x %y 1}
    $w.main.canvas bind all <Any-Leave> {histogramWindow:mouse %W %x %y 0}

    # we need to let the window display, otherwise the canvas size
    # (needed by the first draw) returned by [winfo width/height ...]
    # will be 1
    update idletasks
}

proc histogramWindow:mouse {w x y on} {
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


