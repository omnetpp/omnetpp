#==========================================================================
#  IMPTPROPS.TCL -
#            part of the GNED, the Tcl/Tk graphical topology editor of
#                            OMNeT++
#
#   By Istvan Pataki, Gabor Vincze ({deity|vinczeg}@sch.bme.hu)
#
#==========================================================================

#----------------------------------------------------------------#
#  Copyright (C) 1992-2003 Andras Varga
#
#  This file is distributed WITHOUT ANY WARRANTY. See the file
#  `license' for details on this and other legal matters.
#----------------------------------------------------------------#


proc editImportProps {key} {
    global gned ned canvas

    # create dialog with OK and Cancel buttons
    createOkCancelDialog .impprops "Import Properties"
    wm geometry .impprops "380x260"

    # add notebook pages
    set nb .impprops.f.nb
    notebook $nb
    pack $nb -expand 1 -fill both
    notebook_addpage $nb general "General"
    notebook_addpage $nb imports "Imports"
    notebook_showpage $nb imports

    # create "General" page
    label-text  $nb.general.comment "Doc. comments:" 6
    label-text  $nb.general.rcomment "End-of-line comments:" 2
    pack $nb.general.comment -expand 0 -fill x -side top
    pack $nb.general.rcomment -expand 0 -fill x -side top

    # create "Imports" page
    label $nb.imports.l -text  "Imported files:"
    tableEdit $nb.imports.tbl 10 {
      {Name               name           {entry $e -textvariable $v -width 20 -bd 1}}
      {{End-line comment} right-comment  {entry $e -textvariable $v -width 15 -bd 1}}
      {{Doc. comment}     banner-comment {entry $e -textvariable $v -width 15 -bd 1}}
    }
    pack $nb.imports.l -side top -anchor w
    pack $nb.imports.tbl -side top

    # fill page
    $nb.general.comment.t insert 1.0 $ned($key,banner-comment)
    $nb.general.rcomment.t insert 1.0 $ned($key,right-comment)
    fillTableEditFromNed $nb.imports.tbl $key
    debug "editImportProps: strip/add quotes!"

    # exec the dialog, extract its contents if OK was pressed, then delete dialog
    if {[execOkCancelDialog .impprops] == 1} {
        set ned($key,banner-comment) [getCommentFromText $nb.general.comment.t]
        set ned($key,right-comment) [getCommentFromText $nb.general.rcomment.t]
        updateNedFromTableEdit $nb.imports.tbl $key import name
    }
    destroy .impprops
    updateTreeManager
}


