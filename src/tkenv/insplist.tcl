#==========================================================================
#  INSPLIST.TCL -
#            part of the Tcl/Tk windowing environment of
#                            OMNeT++
#==========================================================================

#----------------------------------------------------------------#
#  Copyright (C) 1992-2001 Andras Varga
#  Technical University of Budapest, Dept. of Telecommunications,
#  Stoczek u.2, H-1111 Budapest, Hungary.
#
#  This file is distributed WITHOUT ANY WARRANTY. See the file
#  `license' for details on this and other legal matters.
#----------------------------------------------------------------#

#
# Inspector list handling
#


# data structure
set inspectorlist() {}
set inspectorkeys {}


#
# THIS PROC IS CALLED FROM C++ CODE, at each inspector display update.
#
proc inspectorupdate_callback {} {
    # experimental code (for Token sample prog):
    # opp_inspectbyname {token.comp[0].sink.local-objects.queuing-time} {cOutVector} {As Graphics}
    inspectorlist_openinspectors
}


#
# try to open all inspectors in list; already open ones are simply ignored
#
proc inspectorlist_openinspectors {} {
    global inspectorlist inspectorkeys

    foreach key $inspectorkeys {
        opp_inspectbyname $inspectorlist($key,name) $inspectorlist($key,classname) \
                          $inspectorlist($key,insptype) $inspectorlist($key,geometry)
    }
}


#
# add an inspector to the list
# (called when an inspector window is opened)
#
proc inspectorlist_add {w} {
    global inspectorlist inspectorkeys

    if {![regexp {\.(ptr.*)-([0-9]+)} $w match object type]} {
        error "window name $w doesn't look like an inspector window"
    }

    set key $w
    lappend inspectorkeys $key
    set inspectorlist($key,name)       [opp_getobjectfullpath $object]
    set inspectorlist($key,classname)  [opp_getobjectclassname $object]
    set inspectorlist($key,insptype)   [opp_inspectortype $type]
    set inspectorlist($key,geometry)   [wm geometry $w]
}

#
# remove an inspector from the list
# (called when an inspector window is closed manually)
#
proc inspectorlist_remove {w} {
    global inspectorlist inspectorkeys

    # FIXME not good!! have to look up by name+classname+insptype!
    set key $w

    set pos [lsearch -exact $inspectorkeys $key]
    set inspectorkeys [lreplace $inspectorkeys $pos $pos]

    unset inspectorlist($key,name)
    unset inspectorlist($key,classname)
    unset inspectorlist($key,insptype)
    unset inspectorlist($key,geometry)
}

#
# store geometry info of an inspector window
# (called when an object gets deleted or before saving inspector list)
#
proc inspectorlist_storegeometry {w} {
    global inspectorlist

    set key $w
    set inspectorlist($key,geometry)   [wm geometry $w]
}

#
# save inspector list to given stream
#
proc inspectorlist_save {fout} {
    global inspectorlist inspectorkeys

    # FIXME not good!!! loop through all *windows*!
    foreach key $inspectorkeys {
        set w $key
        inspectorlist_storegeometry $w
    }

    # save them
    foreach key $inspectorkeys {
        puts $fout "\{$inspectorlist($key,name)\} " nonewline
        puts $fout "\{$inspectorlist($key,classname)\} " nonewline
        puts $fout "\{$inspectorlist($key,insptype)\} " nonewline
        puts $fout "\{$inspectorlist($key,geometry)\}"
    }
}

#
# load inspector list from a given stream; contents add to current inspector list
#
proc inspectorlist_load {fin} {
    global inspectorlist inspectorkeys

    #TBD
}

