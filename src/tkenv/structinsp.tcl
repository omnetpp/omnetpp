#==========================================================================
#  STRUCTINSP.TCL -
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
# Structure inspector (cStructDescriptor-based inspector)
#

proc create_statisticinspector {name} {
    global icons help_tips

    set w $name
    create_inspector_toplevel $w

    # iconbutton $w.toolbar.graph -image $icons(asgraphics) -command "inspect_this $w {As Graphics}"
    # foreach i {graph} {
    #    pack $w.toolbar.$i -anchor n -side left -padx 0 -pady 2
    # }
    #
    # set help_tips($w.toolbar.graph) {Inspect as histogram graphics}

    frame $w.main
    pack $w.main -anchor center -expand 0 -fill both -side top

    # FIXME: a text is a temporary solution... should be sth like a property sheet
    text $w.main.txt
    pack $w.main.txt -expand 1 -fill both -side top
}

