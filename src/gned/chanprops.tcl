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

    tk_messageBox -title "GNED" -icon warning -type ok \
                 -message "Dialog implementation not finished yet"
    return

    # create dialog with OK and Cancel buttons
    createOkCancelDialog .chanprops "Channel Properties"
    wm geometry .chanprops "380x260"

    # add notebook pages
    set nb .chanprops.f.nb
    notebook $nb
    pack $nb -expand 1 -fill both
    notebook_addpage $nb general "General"
    notebook_addpage $nb attrs "Channel attributes"
    notebook_showpage $nb attrs

    # create "General" page
    label-entry $nb.general.name "Name:"
    label-text  $nb.general.comment "Doc. comments:" 6
    label-text  $nb.general.rcomment "End-of-line comments:" 2
    pack $nb.general.name -expand 0 -fill x -side top
    pack $nb.general.comment -expand 0 -fill x -side top
    pack $nb.general.rcomment -expand 0 -fill x -side top

    # add "Attributes" page
    label $nb.attrs.l -text  "Attributes:"
    tableEdit $nb.attrs.tbl 10 {
      {Name               name           {entry $e -textvariable $v -width 20 -bd 1}}
      {Value              value          {entry $e -textvariable $v -width 20 -bd 1}}
      {{End-line comment} right-comment  {entry $e -textvariable $v -width 15 -bd 1}}
      {{Doc. comment}     banner-comment {entry $e -textvariable $v -width 15 -bd 1}}
    }
    button $nb.attrs.def -text "Add standard atttributes"
    pack $nb.attrs.l -side top -anchor w
    pack $nb.attrs.tbl -side top
    pack $nb.attrs.def -side top -anchor w -padx 4 -pady 4

    # fill page
    $nb.general.name.e insert 0 $ned($key,name)
    $nb.general.comment.t insert 1.0 $ned($key,banner-comment)
    $nb.general.rcomment.t insert 1.0 $ned($key,right-comment)
    fillTableEditFromNed $nb.attrs.tbl $key
puts "DBG: editChannelProps: implement (Add standard attributes) button"

    # exec the dialog, extract its contents if OK was pressed, then delete dialog
    if {[execOkCancelDialog .chanprops] == 1} {
        set ned($key,name) [$nb.general.name.e get]
        set ned($key,banner-comment) [getCommentFromText $nb.general.comment.t]
        set ned($key,right-comment) [getCommentFromText $nb.general.rcomment.t]
        updateNedFromTableEdit $nb.attrs.tbl $key chanattr name
    }
    destroy .chanprops
}


