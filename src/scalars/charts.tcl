#=================================================================
#  CHARTS.TCL - part of
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


#
# Callback invoked by BLT to get tick labels
#
proc chartXLabel {w value} {
    global chartXAxisLabels
    if [info exist chartXAxisLabels($w,$value)] {
        return $chartXAxisLabels($w,$value)
    } else {
        return ""
    }
}

proc checkChartLimits {idlists} {
    set numseries [llength $idlists]
    if {$numseries>50} {
        tk_messageBox -icon warning -type ok -title Warning -parent .bltwin \
             -message "Too many series: $numseries, they don't fit on the chart! Dropping some of them to cut the number to 50."
        set idlists [lreplace $idlists 50 end]
    }

    set len [llength [lindex $idlists 0]]
    if {$len>9990} {
        # 10,001 is BLT built-in limit -- grep for MAXTICKS
        tk_messageBox -icon warning -type ok -title Warning -parent .bltwin \
             -message "Too many items on x axis: $len. Dropping some of them to cut the number to ~10,000."

        set oldidlists $idlists
        set idlists {}
        foreach idlist $oldidlists {
            lappend idlists [lreplace $idlist 9990 end]
        }
    }
    return $idlists
}


proc createChartWithModulesOnXAxis {graph idlist} {
    global chartXAxisLabels

    busyCursor "Generating chart..."

    # have to create series by grouping along (run,scalarname) pairs
    debug "grouping and ordering data"
    set idlists [opp_groupByRunAndName $idlist]

    set idlists [checkChartLimits $idlists]

    # now every idlist in $idlists is a series: same (run,scalarname) pairs,
    # aligned by moduleName; may contain holes of id=-1.
    debug "adding series to chart"
    set seriesnum 0
    foreach idlist $idlists {
        set data {}
        foreach id $idlist {
            if {$id==-1} {
                lappend data ""
            } else {
                lappend data [opp_getValueOf $id]
            }
        }

        foreach id $idlist {if {$id!=-1} break}
        if {$id==-1} {
            # no data points for this series, skip it
            continue
        }

        set serieslabel "[opp_getFileNameOf $id]#[opp_getRunNoOf $id] - [opp_getNameOf $id]"
        blt::vector X
        X seq 0 [expr [llength $idlist]-1]
        $graph element create data$seriesnum -x $X(:) -y $data -label $serieslabel -fg [getChartColor $seriesnum]
        blt::vector destroy X
        incr seriesnum
    }

    # get x axis labels
    set idlist [lindex $idlists 0]
    for {set pos 0} {$pos<[llength $idlist]} {incr pos} {
        set id [lindex $idlist $pos]
        if {$id==-1} {
            # find another idlist which has something at position $pos
            foreach idlist $idlists {
                if {[lindex $idlist $pos]!=-1} {
                    set id [lindex $idlist $pos]
                    break
                }
            }
        }
        if {$id!=-1} {
            set chartXAxisLabels($graph,$pos) [opp_getModuleOf $id]
        } else {
            set chartXAxisLabels($graph,$pos) "?"
        }
    }

    $graph axis configure x -rotate 90 -command chartXLabel

    idleCursor
}


proc createChartWithRunsOnXAxis {graph idlist} {
    global chartXAxisLabels

    busyCursor "Generating chart..."

    # have to create series by grouping along (module,scalarname) pairs.
    debug "grouping and ordering data"
    set idlists [opp_groupByModuleAndName $idlist]

    set idlists [checkChartLimits $idlists]

    # now every idlist in $idlists is a series: same (module,scalarname) pairs,
    # aligned by fileNumber+runNumber; may contain holes of id=-1.
    debug "adding series to chart"
    set seriesnum 0
    foreach idlist $idlists {
        set data {}
        foreach id $idlist {
            if {$id==-1} {
                lappend data ""
            } else {
                lappend data [opp_getValueOf $id]
            }
        }

        foreach id $idlist {if {$id!=-1} break}
        if {$id==-1} {
            # no data points for this series, skip it
            continue
        }

        set serieslabel "[opp_getModuleOf $id] - [opp_getNameOf $id]"
        blt::vector X
        X seq 0 [expr [llength $idlist]-1]
        $graph element create data$seriesnum -x $X(:) -y $data -label $serieslabel -fg [getChartColor $seriesnum]
        blt::vector destroy X
        incr seriesnum
    }

    # get x axis labels
    set idlist [lindex $idlists 0]
    for {set pos 0} {$pos<[llength $idlist]} {incr pos} {
        set id [lindex $idlist $pos]
        if {$id==-1} {
            # find another idlist which has something at position $pos
            foreach idlist $idlists {
                if {[lindex $idlist $pos]!=-1} {
                    set id [lindex $idlist $pos]
                    break
                }
            }
        }
        if {$id!=-1} {
            set chartXAxisLabels($graph,$pos) "[opp_getFileNameOf $id] #[opp_getRunNoOf $id]"
        } else {
            set chartXAxisLabels($graph,$pos) "?"
        }
    }
    $graph axis configure x -rotate 90 -command chartXLabel

    idleCursor
}


proc createScatterPlot {graph idlist modulename scalarname} {

    busyCursor "Generating chart..."

    # X series is denoted by ($modulename, $scalarname).
    # Have to group data along (module,scalarname) pairs, and sort
    # everything by X coordinates
    debug "grouping and ordering data"
    set idlists [opp_prepareScatterPlot $idlist $modulename $scalarname]

    # X axis is the first idlist
    set xidlist [lindex $idlists 0]
    set idlists [lreplace $idlists 0 0]

    # convert x
    set xdata {}
    foreach id $xidlist {
        if {$id==-1} {
            lappend xdata ""
        } else {
            lappend xdata [opp_getValueOf $id]
        }
    }
    foreach id $xidlist {if {$id!=-1} break}
    if {$id!=-1} {
        set xlabel "[opp_getModuleOf $id] - [opp_getNameOf $id]"
    } else {
        set xlabel "?"
    }
    $graph axis configure x -title $xlabel

    # convert y and create graph lines
    set seriesnum 0
    foreach idlist $idlists {
        set xdata1 $xdata
        set ydata {}
        foreach id $idlist {
            if {$id==-1} {
                # remove x value as well (adding "" is not good -- it'll show up as 0)
                set pos [llength $ydata]
                set xdata1 [lreplace $xdata1 $pos $pos]
            } else {
                lappend ydata [opp_getValueOf $id]
            }
        }

        foreach id $idlist {if {$id!=-1} break}
        if {$id==-1} {
            # no data points for this series, skip it
            continue
        }

        set serieslabel "[opp_getModuleOf $id] - [opp_getNameOf $id]"
        $graph element create line$seriesnum -x $xdata1 -y $ydata -label $serieslabel \
            -color [getChartColor $seriesnum] -symbol [getChartSymbol $seriesnum] -pixels 5
        incr seriesnum
    }

    idleCursor
}

