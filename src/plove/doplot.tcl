#=================================================================
#  DOPLOT.TCL - part of
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

set elemcounter 0

proc getVectorsToPlot {} {
    global g

    # listbox empty?
    if {[$g(listbox2) index end]==0} {
       return {}
    }

    # if no selection, select current item
    set sel [$g(listbox2) curselection]
    if {$sel == ""} {
         set sel [$g(listbox2) index active]
         $g(listbox2) selection set $sel
    }

    # collect ids of selected vectors
    set idlist {}
    foreach i $sel {
        set line [$g(listbox2) get $i]
        set id [lindex $line end]
        lappend idlist $id
    }
    return $idlist
}


proc checkmemory {arraylist} {
    array set arraybuilder $arraylist
    set numpoints 0
    foreach id [array names arraybuilder] {
        incr numpoints [opp_arraybuilder $arraybuilder($id) length]
    }
    if [catch {opp_checkmemory $numpoints}] {
        error "Not enough memory, plot contains too many points ($numpoints). \
              You can decrease the number of points by selecting fewer vectors to plot, \
              limit the time period plotted (\"crop\" filter), or aggregate points by \
              using e.g. the \"winavg\" filter."
    }
}


proc createVectorPlot {{idlist {}}} {
    global config vec elemcounter

    if {$idlist==""} {
        set idlist [getVectorsToPlot]
        if {$idlist == ""} return
    }

    if [catch {
        # hourglass...
        busyCursor "Processing..."

        set net [opp_createnetwork]

        # collect distinct file names and create vector file readers
        foreach id $idlist {
            set vecfilenodes($vec($id,fname)) ""
        }
        foreach f [array names vecfilenodes] {
            set vecfilenodes($f) [opp_createnode $net vectorfilereader -filename $f]
        }

        # create network
        foreach id $idlist {
            # create filter
            if {$vec($id,filter)==""} {
                # create and connect arraybuilder
                set arraybuilder($id) [opp_createnode $net arraybuilder]
                opp_connect $vecfilenodes($vec($id,fname)) $vec($id,vecid) $arraybuilder($id) in
            } else {
                # create and connect filter, then arraybuilder
                set filtername $vec($id,filter)
                set filter [opp_createnode $net $filtername]
                opp_connect $vecfilenodes($vec($id,fname)) $vec($id,vecid) $filter in

                set arraybuilder($id) [opp_createnode $net arraybuilder]
                opp_connect $filter out $arraybuilder($id) in
            }
        }

        # execute
        opp_executenetwork $net

        # check if we'll have enough memory (BLT has a tendency to just
        # abort() when fails to malloc memory).
        checkmemory [array get arraybuilder]

        # create graph
        busyCursor "Building graph..."
        update idletasks

        set graph [createBltGraph graph]

        # get vectors and build
        set i 0
        foreach id $idlist {
            set xvecname "vx$elemcounter"
            set yvecname "vy$elemcounter"
            incr elemcounter

            opp_arraybuilder $arraybuilder($id) getvectors $xvecname $yvecname
            set color [getChartColor $i]
            set symbol [getChartSymbol $i]
            $graph element create line$id -x $xvecname -y $yvecname -color $color -label $vec($id,title)
            $graph element config line$id -symbol $symbol -pixels 3
            incr i
        }

        idleCursor

    } errmsg] {
        idleCursor
        tk_messageBox -icon warning -type ok -title "Error" -message "Error: $errmsg"
    }
}


proc createVectorScatterPlot {idlist xid chartname} {
    global config vec elemcounter

    if {[llength $idlist]<2} {
        error "needs at least two vectors for scatter plot"
    }

    if [catch {
        # hourglass...
        busyCursor "Processing..."

        set net [opp_createnetwork]

        # collect distinct file names and create vector file readers
        foreach id $idlist {
            set vecfilenodes($vec($id,fname)) ""
        }
        foreach f [array names vecfilenodes] {
            set vecfilenodes($f) [opp_createnode $net vectorfilereader -filename $f]
        }

        # create network
        set xyplotnode [opp_createnode $net xyplot]
        set i 0
        foreach id $idlist {
            # create filter
            if {$id==$xid} {
                set port "x"
            } else {
                set port "y$i"
            }
            if {$vec($id,filter)==""} {
                opp_connect $vecfilenodes($vec($id,fname)) $vec($id,vecid) $xyplotnode $port
            } else {
                # create and connect filter, then arraybuilder
                set filtername $vec($id,filter)
                set filter [opp_createnode $net $filtername]
                opp_connect $vecfilenodes($vec($id,fname)) $vec($id,vecid) $filter in
                opp_connect $filter out $xyplotnode $port
            }
            if {$id!=$xid} {
                set arraybuilder($id) [opp_createnode $net arraybuilder]
                opp_connect $xyplotnode "out$i" $arraybuilder($id) in
                incr i
            }
        }

        # execute
        opp_executenetwork $net

        # check if we'll have enough memory (BLT has a tendency to just
        # abort() when fails to malloc memory).
        checkmemory [array get arraybuilder]

        # create graph
        busyCursor "Building graph..."
        update idletasks

        set graph [createBltGraph graph $chartname]

        $graph axis configure x -title $vec($xid,title)

        set i 0
        foreach id $idlist {
            if {$id!=$xid} {
                set xvecname "vx$elemcounter"
                set yvecname "vy$elemcounter"
                incr elemcounter

                opp_arraybuilder $arraybuilder($id) getvectors $xvecname $yvecname
                set color [getChartColor $i]
                set symbol [getChartSymbol $i]
                $graph element create line$id -x $xvecname -y $yvecname -color $color -label $vec($id,title)
                $graph element config line$id -symbol $symbol -pixels 5 -linewidth 0
                incr i
            }
        }

        idleCursor

    } errmsg] {
        idleCursor
        tk_messageBox -icon warning -type ok -title "Error" -message "Error: $errmsg"
    }
}

