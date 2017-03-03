#=================================================================
#  WATCHINSPECTOR.TCL - part of
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


proc createWatchInspector {insp geom} {
    createInspectorToplevel $insp $geom
    createWatchViewer $insp
}

proc createEmbeddedWatchInspector {insp} {
    createWatchViewer $insp
}

proc createWatchViewer {insp} {
    set f $insp.main

    ttk::frame $f
    pack $f -expand 0 -fill both -side top

    label-entry $f.name ""
    $f.name.l config -width 20
    focus $f.name.e
    pack $f.name -fill x -side top

    inspector:bindSideButtons $insp
}


