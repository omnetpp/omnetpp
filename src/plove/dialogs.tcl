#=================================================================
#  DIALOGS.TCL - part of
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
# Options for a scatter plot. $idlist contains ids that represent all unique
# (module,scalarname) pairs -- this is used for X axis selection.
#
proc createVectorScatterPlotDialog {idlist} {
    global tmp vec

    if {$idlist==""} {
        set idlist [getVectorsToPlot]
        if {$idlist == ""} return
    }

    if {[llength $idlist]==1} {
        tk_messageBox -icon info -type ok -title "Scatter Plot" \
            -message "You need at least two vectors for scatter (x-y) plots: one for the x coordinate plus one or more for y."
        return
    }

    # create dialog with OK and Cancel buttons
    set w .dlg
    createOkCancelDialog $w "Create scatter plot"

    label $w.f.l -justify left -text "A scatter (X-Y) plot matches values from two vectors by timestamp,\
                                     and uses them as (x,y) coordinates.\nValues which have different timestamps\
                                     will not form data points."
    pack $w.f.l -expand 0 -fill x -side top

    # add entry fields and focus on first one
    labelframe $w.f.f2 -relief groove -border 2 -text "Setup"
    label $w.f.f2.l -justify left -text "Select variable for X axis:"
    combo $w.f.f2.xseries {}
    $w.f.f2.xseries.entry config -textvariable tmp(xseries)

    pack $w.f.f2.l -anchor w -expand 0 -fill none -side top
    pack $w.f.f2.xseries -anchor w -expand 0 -fill x -side top
    pack $w.f.f2 -expand 0 -fill x -side top

    labelframe $w.f.f1 -relief groove -border 2 -text "General"
    label-entry $w.f.f1.chartname "Chart name:"
    pack $w.f.f1 -expand 0 -fill x -side top
    pack $w.f.f1.chartname -expand 0 -fill x -side top

    # fill xseries combo
    set list {}
    foreach id $idlist {
        lappend list $vec($id,title)
    }
    comboconfig $w.f.f2.xseries $list

    # exec the dialog, extract its contents if OK was pressed, then delete dialog
    set ret {}
    if {[execOkCancelDialog $w] == 1} {
        set xid ""
        foreach id $idlist {
            if {$tmp(xseries)==$vec($id,title)} {
                set xid $id
            }
        }
        if {$xid==""} {
            tk_messageBox -icon info -type ok -title "Scatter Plot" \
                -message "x variable selection invalid."
            return ""

        }
        set chartname [$w.f.f1.chartname.e get]
        set ret [list $chartname $xid]
    }
    destroy $w
    return $ret
}
