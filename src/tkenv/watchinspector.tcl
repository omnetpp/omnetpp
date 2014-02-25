#=================================================================
#  WATCHINSPECTOR.TCL - part of
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


proc createWatchInspector {name geom} {
    global fonts

    set w $name
    createInspectorToplevel $w $geom

    frame $w.main
    pack $w.main -expand 0 -fill both -side top

    createWatchViewer $w.main
}

proc createWatchViewer {w} {
    label-entry $w.name ""
    $w.name.l config -width 20
    focus $w.name.e
    pack $w.name -fill x -side top
}


