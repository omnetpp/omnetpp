#=================================================================
#  LAYOUTERENV.TCL - part of
#
#                     OMNeT++/OMNEST
#            Discrete System Simulation in C++
#
#=================================================================

#----------------------------------------------------------------#
#  Copyright (C) 1992-2017 Andras Varga
#
#  This file is distributed WITHOUT ANY WARRANTY. See the file
#  `license' for details on this and other legal matters.
#----------------------------------------------------------------#


#
# Called from TkenvGraphLayouterEnvironment::debugDraw()
#
proc layouter:debugDrawFinish {c msg} {
    # create label
    set bb [$c bbox bbox]
    $c create text [lindex $bb 0] [lindex $bb 1] -tag bbox -anchor sw -text $msg -font CanvasFont
    set bb [$c bbox bbox]

    # rescale to fit canvas
    set w [expr [lindex $bb 2]-[lindex $bb 0]]
    set h [expr [lindex $bb 3]-[lindex $bb 1]]
    set cw [winfo width $c]
    set ch [winfo height $c]
    set fx [expr $cw/double($w)]
    set fy [expr $ch/double($h)]
    if {$fx>1} {set fx 1}
    if {$fy>1} {set fy 1}
    $c scale all 0 0 $fx $fy

    $c config -scrollregion [$c bbox bbox]
    $c xview moveto 0
    $c yview moveto 0
    update idletasks
}

proc layouter:startGrab {stopbutton} {
    global opp help_tips

    # tooltip
    set opp(grabSavedTooltip) $help_tips($stopbutton)
    set help_tips($stopbutton) "Layouting -- click STOP to abort it"

    # prevent user from closing window (postpone close operation)
    set insp [winfo toplevel $stopbutton]  ;#FIXME NOT QUITE!!!
    set opp(grabOrigCloseHandler) [wm protocol $insp WM_DELETE_WINDOW]
    wm protocol $insp WM_DELETE_WINDOW [list opp_markinspectorfordeletion $insp]

    set opp(oldGrab) [grab current $stopbutton]

    grab $stopbutton
    focus $stopbutton
}

proc layouter:releaseGrab {stopbutton} {
    global opp help_tips

    # restore everything messed up in layouter:startGrab
    set help_tips($stopbutton) $opp(grabSavedTooltip)

    catch {grab release $stopbutton}
    catch {grab release [grab current]}

    set insp [winfo toplevel $stopbutton]   ;#FIXME see above
    wm protocol $insp WM_DELETE_WINDOW $opp(grabOrigCloseHandler)
}


