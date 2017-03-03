#=================================================================
#  HISTOGRAMINSPECTOR.TCL - part of
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


proc createHistogramInspector {insp geom} {
    createInspectorToplevel $insp $geom
    createHistogramViewer $insp
}

proc createEmbeddedHistogramInspector {insp} {
    createHistogramViewer $insp
}

proc createHistogramViewer {insp} {
    ttk::frame $insp.main
    ttk::frame $insp.bot
    pack $insp.bot  -expand 0 -fill x -side bottom
    pack $insp.main -expand 1 -fill both -side top

    canvas $insp.main.canvas -borderwidth 2 -relief raised -background lightblue
    pack $insp.main.canvas -anchor center -expand 1 -fill both -side top

    ttk::label $insp.bot.info -relief groove -width 50
    pack $insp.bot.info -anchor center -expand 1 -fill x -side left

    $insp.main.canvas bind all <Any-Enter> "HistogramInspector:mouse $insp %x %y 1"
    $insp.main.canvas bind all <Any-Leave> "HistogramInspector:mouse $insp %x %y 0"

    inspector:bindSideButtons $insp

    # make the window respond to resize events
    bind $insp <Configure> "opp_refreshinspector $insp"

    # we need to let the window display, otherwise the canvas size
    # (needed by the first draw) returned by [winfo width/height ...]
    # will be 1
    update idletasks
}

proc HistogramInspector:mouse {insp x y on} {
    set c $insp.main.canvas
    set tags [$c gettags current]
    if [regexp {.*cell([0-9]+).*} $tags match cell] {
       if {$on == 1} {
           $c itemconfig current -fill gray -outline gray
           $insp.bot.info config -text [opp_inspectorcommand $insp cell $cell]
       } else {
           $c itemconfig current -fill black -outline black
           $insp.bot.info config -text [opp_inspectorcommand $insp cell]
       }
    }
}


