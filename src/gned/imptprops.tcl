#==========================================================================
#  IMPTPROPS.TCL -
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


proc editImportProps {key} {
    global gned ned canvas

    tk_messageBox -icon warning -type ok -title GNED -message "Sorry! This dialog hasn't been implemented yet."
    return

    # create dialog with OK and Cancel buttons
    createOkCancelDialog .impprops "Import Properties"

    # add notebook pages
    notebook .impprops.f.nb
    pack .impprops.f.nb -expand 1 -fill both
    notebook_addpage .impprops.f.nb imports "Imports"

    label-entry .impprops.f.nb.imports.name  "Name:"
    label-entry .impprops.f.nb.imports.i    "Imports:"

    pack .impprops.f.nb.imports.name  -expand 0 -fill x -side top
    pack .impprops.f.nb.imports.i     -expand 0 -fill x -side top

    .impprops.f.nb.imports.name.e  insert 0 $ned($key,name)
    .impprops.f.nb.imports.name.e  configure -state disabled
    .impprops.f.nb.imports.i.e   insert 0 $ned($key,name)

    focus .impprops.f.nb.imports.name.e

    # exec the dialog, extract its contents if OK was pressed, then delete dialog
    if {[execOkCancelDialog .impprops] == 1} {
       set ned($key,name)     [.impprops.f.nb.imports.i.e   get]

    }
    destroy .impprops
    updateTreeManager
}


