#=================================================================
#  IMPTPROPS.TCL - part of
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


proc editImportProps {key} {
    global gned ned canvas

    # create dialog with OK and Cancel buttons
    set w .impprops
    createOkCancelDialog $w "Import Properties"
    wm geometry $w "380x260"

    # add notebook pages
    set nb $w.f.nb
    notebook $nb
    pack $nb -expand 1 -fill both
    notebook_addpage $nb general "General"
    notebook_addpage $nb imports "Imports"

    # create "General" page
    label-text  $nb.general.comment "Doc. comments:" 6
    label-text  $nb.general.rcomment "End-of-line comments:" 2
    pack $nb.general.comment -expand 0 -fill x -side top
    pack $nb.general.rcomment -expand 0 -fill x -side top

    # create "Imports" page
    label $nb.imports.l -text  "Imported files:"
    tableEdit $nb.imports.tbl 15 {
      {{Name (without .ned)} name        {entry $e -textvariable $v -width 20 -bd 1}}
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

    notebook_showpage $nb imports

    # exec the dialog, extract its contents if OK was pressed, then delete dialog
    if {[execOkCancelDialog $w ImportProps:validate] == 1} {
        set ned($key,banner-comment) [getCommentFromText $nb.general.comment.t]
        set ned($key,right-comment) [getCommentFromText $nb.general.rcomment.t]
        updateNedFromTableEdit $nb.imports.tbl $key import name

        markNedFileOfItemDirty $key
        updateTreeManager
    }
    destroy $w
}


# ImportProps:validate --
#
# Validation proc, for use with execOKCancelDialog.
#
proc ImportProps:validate {w} {
    set nb $w.f.nb
    if [catch {
        # FIXME validate tables
    } message] {
        tk_messageBox -parent $w -icon error -type ok -title "Error" -message $message
        return 0
    }
    return 1
}

