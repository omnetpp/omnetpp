#=================================================================
#  MENUPROC.TCL - part of
#
#                     OMNeT++/OMNEST
#            Discrete System Simulation in C++
#
#=================================================================

#----------------------------------------------------------------#
#  Copyright (C) 2004-2005 Andras Varga
#
#  This file is distributed WITHOUT ANY WARRANTY. See the file
#  `license' for details on this and other legal matters.
#----------------------------------------------------------------#



proc plotVectorScatterPlot {} {

    set idlist [getVectorsToPlot]

    set res [createVectorScatterPlotDialog $idlist]
    if {$res==""} return
    set chartname [lindex $res 0]
    set xid [lindex $res 1]

    createVectorScatterPlot $idlist $xid $chartname
}

