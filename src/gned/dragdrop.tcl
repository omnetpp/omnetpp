#==========================================================================
#  DRAGDROP.TCL -
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


proc dragAndDropStart {absx absy} {

}


proc dragAndDropMotion {absx absy} {

}


proc dragAndDropFinish {typekey absx absy} {
    global gned canvas ned

    # was it dropped to the canvas?
    set canv_id $gned(canvas_id)
    set w [winfo containing $absx $absy]

    if {$w!=$canvas($canv_id,canvas)} {
       tk_messageBox -message "Can only drop to the canvas..." -type ok -icon error
       return
    }

    # determine coords on the canvas
    set x [expr $absx - [winfo rootx $w]]
    set y [expr $absy - [winfo rooty $w]]

    # create module
    set modkey $canvas($gned(canvas_id),module-key)
    set submodskey [getChildrenWithType $modkey submods]
    if {[llength $submodskey]==0} {
        set submodskey [addItem submods $modkey]
    } elseif {[llength $submodskey]>1} {
        error "Internal error: more than one 'submods'"
    }
    set key [addItem submod $submodskey]

    set x1 [expr $x-30]
    set y1 [expr $y-20]
    set x2 [expr $x+30]
    set y2 [expr $y+20]

    set ned($key,x-pos)  [expr int(($x1+$x2)/2)]
    set ned($key,y-pos)  [expr int(($y1+$y2)/2)]
    set ned($key,x-size) [expr int($x2-$x1)]
    set ned($key,y-size) [expr int($y2-$y1)]

    drawItem $key

    markCanvasDirty
}


