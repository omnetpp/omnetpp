#==========================================================================
#  NETWPROPS.TCL -
#            part of the GNED, the Tcl/Tk graphical topology editor of
#                            OMNeT++
#
#   By Istvan Pataki, Gabor Vincze ({deity|vinczeg}@sch.bme.hu)
#
#==========================================================================

#----------------------------------------------------------------#
#  Copyright (C) 1992-2001 Andras Varga
#  Technical University of Budapest, Dept. of Telecommunications,
#  Stoczek u.2, H-1111 Budapest, Hungary.
#
#  This file is distributed WITHOUT ANY WARRANTY. See the file
#  `license' for details on this and other legal matters.
#----------------------------------------------------------------#


proc editNetworkProps {key} {
    global gned ned

    #
    # Set up dialog.
    #

    # create dialog with OK and Cancel buttons
    createOkCancelDialog .netprops "Network Properties"
    wm geometry .netprops "480x320"

    set nb .netprops.f.nb

    # add notebook pages
    notebook $nb
    pack $nb -expand 1 -fill both
    notebook_addpage $nb general "General"
    notebook_addpage $nb pars "Parameters"
    notebook_addpage $nb on "Machines"

    # create "General" page
    label-entry $nb.general.name "Name:"
    label-entry $nb.general.type "Type:"
    label-entry $nb.general.like "Like:"
    label-text  $nb.general.comment "Comments:" 4

    pack $nb.general.name  -expand 0 -fill x -side top
    pack $nb.general.type  -expand 0 -fill x -side top
    pack $nb.general.like  -expand 0 -fill x -side top
    pack $nb.general.comment -expand 0 -fill x -side top

    # fill "General" page with values
    $nb.general.name.e insert 0 $ned($key,name)
    $nb.general.type.e insert 0 $ned($key,type-name)
    $nb.general.like.e insert 0 $ned($key,like-name)
    $nb.general.comment.t insert 1.0 $ned($key,banner-comment)

    # create "Parameters" page
    createSectionsComboAndTables $nb.pars $key substparams parameters  {
      {Name    name    {entry $e -textvariable $v -width 20 -bd 1}}
      {Value   value   {entry $e -textvariable $v -width 20 -bd 1}}
      {{End-line comment} right-comment {entry $e -textvariable $v -width 15 -bd 1}}
      {{Doc. comment} banner-comment {entry $e -textvariable $v -width 15 -bd 1}}
    }
    button $nb.pars.xcheck -text "Consult module declaration"
    pack $nb.pars.xcheck -side top -anchor w -padx 4 -pady 4

    # create "Machines" page
    createSectionsComboAndTables $nb.on $key substmachines on  {
      {{On machine} value  {entry $e -textvariable $v -width 16 -bd 1}}
      {{End-line comment} right-comment {entry $e -textvariable $v -width 24 -bd 1}}
      {{Doc. comment} banner-comment {entry $e -textvariable $v -width 30 -bd 1}}
    }
    button $nb.on.xcheck -text "Consult module declaration"
    pack $nb.on.xcheck -side top -anchor w -padx 4 -pady 4

    focus $nb.general.name.e


    #
    # Execute dialogs and extract results.
    #

    # set initial focus
    focus $nb.general.name.e

    # exec the dialog, extract its contents if OK was pressed, then delete dialog
    if {[execOkCancelDialog .netprops] == 1} {

        # process "General" page.
        set ned($key,name) [$nb.general.name.e get]
        set ned($key,type-name) [$nb.general.type.e get]
        set ned($key,like-name) [$nb.general.like.e get]
        set ned($key,banner-comment) [getCommentFromText $nb.general.comment.t]

        # process "Parameters" page.
        updateNedFromSectionTables $nb.pars $key substparams substparam name

        # process "Machines" page.
        updateNedFromSectionTables $nb.on $key substmachines substmachine value

        # destroy dialog here
        destroy .netprops

        # check if module type specified already exists...
        if {$ned($key,like-name)!=""} {
            set typename $ned($key,like-name)
        } else {
            # FIXME: check it's non-empty
            set typename $ned($key,type-name)
        }
        if {[itemKeyFromName $typename module]=="" && [itemKeyFromName $typename simple]==""} {
            if {"yes"==[tk_messageBox -type yesno -title GNED -icon question \
                -message "Module type '$typename' is unknown to GNED. Create it in this file?"]
            } {
                tk_messageBox -type ok -title GNED -icon error -message "Not implemented yet :-)"
            }
        }
    } else {
        destroy .netprops
    }
}



