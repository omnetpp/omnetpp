#==========================================================================
#  MENUPROC.TCL -
#            part of OMNeT++
#==========================================================================

#----------------------------------------------------------------#
#  Copyright (C) 2004 Andras Varga
#
#  This file is distributed WITHOUT ANY WARRANTY. See the file
#  `license' for details on this and other legal matters.
#----------------------------------------------------------------#


set config(scalarfile)  ""


proc fileLoad {} {
    global config

    set fname $config(scalarfile)

    catch {cd [file dirname $fname]}
    set fname [file tail $fname]
    set fname [tk_getOpenFile -defaultextension ".sca" \
               -initialdir [pwd] -initialfile $fname \
               -filetypes {{{Output scalars} {*.sca}} \
                           {{All files} {*}}}]

    if {$fname != ""} {
        loadScalarFile $fname
        set config(scalarfile) $fname
    }
}

proc fileExit {} {
    #exitCleanup
    #saveConfig "~/.scalarsrc"
    exit
}


#
# Copy table contents to clipboard (tab-separated)
#
proc editCopy {} {
    # FIXME doesn't obey current listbox sorting -- manybe should get IDs from listbox?
    # should maybe also obey listbox selection?
    set idlist [getFilteredList]
    set data "Directory\tFile\tRun#\tModule\tName\tValue\n"
    foreach id $idlist {
        append data "[opp_getDirectoryOf $id]\t[opp_getFileNameOf $id]\t[opp_getRunNoOf $id]\t[opp_getModuleOf $id]\t[opp_getNameOf $id]\t[opp_getValueOf $id]\n"
    }
    clipboard clear -displayof .
    clipboard append -displayof . $data
}


#
# Copy table contents to clipboard. User can choose columns and separator
# (comma, semicolon, tab, space), whether to use quotes etc.
#
proc editCopySpecial {} {
    # FIXME should pop up dialog with column selection etc
    editCopy
}

proc chartCreateBarChart {} {
    global chartXAxisLabels

    # ask properties of chart
    set res [createChartDialog]
    if {$res==""} return
    set charttype [lindex $res 0]
    set chartname [lindex $res 1]

    # create empty chart, collect data
    set graph [createBltGraph barchart $chartname]
    set idlist [getFilteredList]

    # fill chart with appropriate series created from the data in $idlist
    if {$charttype=="modules"} {
        createChartWithModulesOnXAxis $graph $idlist
    } elseif {$charttype=="runs"} {
        createChartWithRunsOnXAxis $graph $idlist
    }
}

proc chartCreateScatterPlot {} {

    # collect data (we'll need it for X axis selection combo)
    puts "DBG: getting filtered list"
    set idlist [getFilteredList]

    puts "DBG: getting module-scalar pairs for combo"
    set maxcount 100
    set idlistforcombo [opp_getModuleAndNamePairs $idlist $maxcount]
    if {[llength $idlistforcombo]==$maxcount} {
        tk_messageBox -icon warning -type ok -title "Selection too broad" \
             -message "There are too many, more than $maxcount variables (module+scalar pairs), \
                       which would make as many lines on the plot. \
                       Please narrow the set of variables to be plotted, then try again."
        return
    }

    set res [createScatterPlotDialog $idlistforcombo]
    if {$res==""} return
    set chartname [lindex $res 0]
    set modulename [lindex $res 1]
    set scalarname [lindex $res 2]

    # create empty chart
    set graph [createBltGraph graph $chartname]

    createScatterPlot $graph $idlist $modulename $scalarname
}
