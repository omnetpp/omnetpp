#==========================================================================
#  TREEMGR.TCL -
#            part of the GNED, the Tcl/Tk graphical topology editor of
#                            OMNeT++
#
#   By Andras Varga
#
#==========================================================================

#----------------------------------------------------------------#
#  Copyright (C) 1992,99 Andras Varga
#  Technical University of Budapest, Dept. of Telecommunications,
#  Stoczek u.2, H-1111 Budapest, Hungary.
#
#  This file is distributed WITHOUT ANY WARRANTY. See the file
#  `license' for details on this and other legal matters.
#----------------------------------------------------------------#


# updateTreeManager --
#
# Redraws the manager window (left side of main window).
# This function is a temporary solution, it must be replaced with
# some more sophisticated notification mechanism (like "$key
# deleted/added")
#
proc updateTreeManager {} {
    global ned gned fonts

    # clear canvas
    set c $gned(manager).tree
    $c delete all

    # add items
    set y 5
    set dy 14

    # debug code:
    #foreach i [lsort [array names ned ]] {
    #   $c create text 8 $y -anchor nw -text "$i: $ned($i)"
    #   set y [expr $y+12]
    #}

    foreach nedfilekey $ned(0,childrenkeys) {
        set fname [file tail $ned($nedfilekey,filename)]
        $c create text 8 $y -anchor nw -text $fname -font $fonts(bold)
        set y [expr $y+$dy]

        foreach key $ned($nedfilekey,childrenkeys) {
            if [info exist ned($key,name)] {
                 set txt "$ned($key,type) $ned($key,name)"
            } else {
                 set txt "$ned($key,type)"
            }
            $c create text 20 $y -anchor nw -text $txt -font $fonts(normal)
            set y [expr $y+$dy]
        }
    }

    # adjust scrolling region
    set bbox [$c bbox all]
    if {$bbox==""} {
        $c config -scrollregion "0 0 0 0"
    } else {
        $c config -scrollregion $bbox
    }
}



