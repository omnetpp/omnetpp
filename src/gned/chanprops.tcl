#==========================================================================
#  CHANPROPS.TCL -
#            part of the GNED, the Tcl/Tk graphical topology editor of
#                            OMNeT++
#
#   By Istvan Pataki, Gabor Vincze ({deity|vinczeg}@sch.bme.hu)
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


proc editChannelProps {key} {
    global gned ned canvas

    tk_messageBox -icon warning -type ok -title GNED -message "Sorry! This dialog hasn't been implemented yet."
    return

    # create dialog with OK and Cancel buttons
    createOkCancelDialog .chanprops "Network Properties"

    # add notebook pages
    notebook .chanprops.f.nb
    pack .chanprops.f.nb -expand 1 -fill both
    notebook_addpage .chanprops.f.nb general "General"

    label-entry .chanprops.f.nb.general.name     "Name:"
    label-entry .chanprops.f.nb.general.delay    "Delay:"
    label-entry .chanprops.f.nb.general.error    "Error:"
    label-entry .chanprops.f.nb.general.datarate "Datarate:"
    label-text  .chanprops.f.nb.general.comment  "Comments:" 3

    pack .chanprops.f.nb.general.name     -expand 0 -fill x -side top
    pack .chanprops.f.nb.general.delay    -expand 0 -fill x -side top
    pack .chanprops.f.nb.general.error    -expand 0 -fill x -side top
    pack .chanprops.f.nb.general.datarate -expand 0 -fill x -side top
    pack .chanprops.f.nb.general.comment  -expand 0 -fill x -side top

    .chanprops.f.nb.general.name.e     insert 0 $ned($key,name)
    .chanprops.f.nb.general.delay.e    insert 0 $ned($key,delay)
    .chanprops.f.nb.general.error.e    insert 0 $ned($key,error)
    .chanprops.f.nb.general.datarate.e insert 0 $ned($key,datarate)
    .chanprops.f.nb.general.comment.t  insert 1.0 $ned($key,comment)

    focus .chanprops.f.nb.general.name.e

    # exec the dialog, extract its contents if OK was pressed, then delete dialog
    if {[execOkCancelDialog .chanprops] == 1} {
       set ned($key,name)       [.chanprops.f.nb.general.name.e get]
       set ned($key,delay)      [.chanprops.f.nb.general.delay.e get]
       set ned($key,error)      [.chanprops.f.nb.general.error.e  get]
       set ned($key,datarate)   [.chanprops.f.nb.general.datarate.e get]
       set ned($key,comment)    [.chanprops.f.nb.general.comment.t get 1.0 end]
    }
    destroy .chanprops
}


