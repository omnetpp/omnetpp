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

proc createChartDialog {} {

    global tmp

    # create dialog with OK and Cancel buttons
    set w .dlg
    createOkCancelDialog $w "Create chart"

    # add entry fields and focus on first one
    labelframe $w.f.f2 -relief groove -border 2 -text "Chart type"
    label $w.f.f2.l -text "Select chart type:"
    #radiobutton $w.f.f2.r1 -text "series by file+run# and scalar name (x axis: modules)" -value "copy" -variable tmp(type)
    #radiobutton $w.f.f2.r2 -text "series by module and scalar name (x axis: file+run#)" -value "move" -variable tmp(type)
    radiobutton $w.f.f2.r1 -text "x axis: modules (form series by file+run# and scalar name)" -value "modules" -variable tmp(type)
    radiobutton $w.f.f2.r2 -text "x axis: file+run# (form series by module and scalar name)" -value "runs" -variable tmp(type)
    set tmp(type) "modules"

    pack $w.f.f2.l -anchor w -expand 0 -fill none -side top
    pack $w.f.f2.r1 -anchor w -expand 0 -fill none -side top
    pack $w.f.f2.r2 -anchor w -expand 0 -fill none -side top
    pack $w.f.f2 -expand 0 -fill x -side top

    labelframe $w.f.f1 -relief groove -border 2 -text "General"
    label-entry $w.f.f1.chartname "Chart name:"
    pack $w.f.f1 -expand 0 -fill x -side top
    pack $w.f.f1.chartname -expand 0 -fill x -side top

    # exec the dialog, extract its contents if OK was pressed, then delete dialog
    set ret {}
    if {[execOkCancelDialog $w] == 1} {
        lappend ret $tmp(type)
        lappend ret [$w.f.f1.chartname.e get]
    }
    destroy $w
    return $ret
}


#
# Options for a scatter plot. $idlist contains ids that represent all unique
# (module,scalarname) pairs -- this is used for X axis selection.
#
proc createScatterPlotDialog {idlistforcombo} {
    global tmp

    if {$idlistforcombo=={}} {
        tk_messageBox -icon info -type ok -title Info -message "There's no data to plot!"
        return {}
    }

    # create dialog with OK and Cancel buttons
    set w .dlg
    createOkCancelDialog $w "Create scatter plot"

    label $w.f.l -justify left -text "Scatter (X-Y) plots pair two scalars to get (x,y) coordinates for data points.\nEvery run (file+run#) contributes one point to each graph line.\nExample usage: \"throughput vs offered load\" plot.\n"
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
    foreach id $idlistforcombo {
        lappend list "[opp_getModuleOf $id] - [opp_getNameOf $id]"
    }
    comboconfig $w.f.f2.xseries $list

    # exec the dialog, extract its contents if OK was pressed, then delete dialog
    set ret {}
    if {[execOkCancelDialog $w] == 1} {
        regexp -- {(.*) - (.*)} $tmp(xseries) match modulename scalarname
        set chartname [$w.f.f1.chartname.e get]
        set ret [list $chartname $modulename $scalarname]
    }
    destroy $w
    return $ret
}



proc copyToClipboardDialog {} {

    global tmp

    # create dialog with OK and Cancel buttons
    set w .dlg
    createOkCancelDialog $w "Copy to clipboard"

    # add entry fields and focus on first one
    labelframe $w.f.f2 -relief groove -border 2 -text "Select format"
    radiobutton $w.f.f2.r1 -text "with Name and Value columns" -value "namevalue" -variable tmp(clipboardformat)
    radiobutton $w.f.f2.r2 -text "separate columns for each variable" -value "vars" -variable tmp(clipboardformat)
    set tmp(clipboardformat) "vars"

    label $w.f.l -justify left -text \
        "Hint: explore the PivotTable or DataPilot functionality of your\
         \nspreadsheet program (MS Excel or OpenOffice Calc).\
         \nIt can help you get further insight into your simulation results."

    pack $w.f.f2.r1 -anchor w -expand 0 -fill none -side top
    pack $w.f.f2.r2 -anchor w -expand 0 -fill none -side top
    pack $w.f.f2 -expand 0 -fill x -side top
    pack $w.f.l -expand 0 -fill x -side top

    # exec the dialog, extract its contents if OK was pressed, then delete dialog
    set ret {}
    if {[execOkCancelDialog $w] == 1} {
        lappend ret $tmp(clipboardformat)
    }
    destroy $w
    return $ret
}

