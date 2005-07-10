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
    saveConfig "~/.scalarsrc"
    exit
}

proc editCopy {} {
    set fmt [copyToClipboardDialog]
    if {$fmt=="vars"} {
        editCopy2
    } else {
        editCopy1
    }
}

#
# Copy table contents to clipboard (tab-separated), with "Name" and "Value" columns
#
proc editCopy1 {} {
    # FIXME doesn't obey current listbox sorting -- maybe should get IDs from listbox?
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
# Copy table contents to clipboard (tab-separated), every scalar variable on its own column
#
proc editCopy2 {} {
    set idlist [getFilteredList]

    set matrix [opp_prepareCopyToClipboard $idlist]
    set header "Directory\tFile\tRun#\tModule\t"

    # get table heading
    set row [lindex $matrix 0]
    for {set pos 0} {$pos<[llength $row]} {incr pos} {
        # find a row which has something at position $pos
        set id -1
        foreach row $matrix {
            if {[lindex $row $pos]!=-1} {
                set id [lindex $row $pos]
                break
            }
        }
        if {$id!=-1} {
            append header "[opp_getNameOf $id]\t"
        } else {
            append header "?\t"
        }
    }

    set data "$header\n"

    # get table data
    foreach row $matrix {
        set id [lindex $row 0]

        set fileRunModule ""
        set values ""
        foreach id $row {
            if {$id==-1} {
                append values "\t"
            } else {
                append values "[opp_getValueOf $id]\t"
                if {$fileRunModule==""} {
                    set fileRunModule "[opp_getDirectoryOf $id]\t[opp_getFileNameOf $id]\t[opp_getRunNoOf $id]\t[opp_getModuleOf $id]"
                }
            }
        }
        append data "$fileRunModule\t$values\n"
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

    # apply defaults to graph and lines
    bltGraph_applyDefaults $graph
}

proc chartCreateScatterPlot {} {

    # collect data (we'll need it for X axis selection combo)
    debug "getting filtered list"
    set idlist [getFilteredList]

    debug "getting module-scalar pairs for combo"
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

    # do the plot
    createScatterPlot $graph $idlist $modulename $scalarname

    # apply defaults to graph and lines
    bltGraph_applyDefaults $graph
}
