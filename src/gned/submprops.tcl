#==========================================================================
#  SUBMPROPS.TCL -
#            part of the GNED, the Tcl/Tk graphical topology editor of
#                            OMNeT++
#
#   By Andras Varga
#   Additions: Istvan Pataki, Gabor Vincze ({deity|vinczeg}@sch.bme.hu)
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


proc editSubmoduleProps {key} {
    global gned ned

    #
    # Set up dialog.
    #

    # create dialog with OK and Cancel buttons
    createOkCancelDialog .submprops "Submodule Properties"

    # add notebook pages
    notebook .submprops.f.nb
    pack .submprops.f.nb -expand 1 -fill both
    notebook_addpage .submprops.f.nb general "General"
    notebook_addpage .submprops.f.nb pars "Parameters"
    notebook_addpage .submprops.f.nb gates "Gate sizes"
    notebook_addpage .submprops.f.nb on "Machines"

    # create "General" page
    label-entry .submprops.f.nb.general.name "Name:"
    label-entry .submprops.f.nb.general.type "Type:"
    label-entry .submprops.f.nb.general.like "Like:"
    label-check .submprops.f.nb.general.link "Type link:"  "Define type if not exist" def
    label-entry .submprops.f.nb.general.vs "Vector size:"
    label-text  .submprops.f.nb.general.comment "Comments:" 4

    pack .submprops.f.nb.general.name  -expand 0 -fill x -side top
    pack .submprops.f.nb.general.type  -expand 0 -fill x -side top
    pack .submprops.f.nb.general.like  -expand 0 -fill x -side top
    pack .submprops.f.nb.general.link -expand 0 -fill x -side top
    pack .submprops.f.nb.general.vs  -expand 0 -fill x -side top
    pack .submprops.f.nb.general.comment -expand 0 -fill x -side top

    # create "Parameters" page
    label .submprops.f.nb.pars.l -text "Parameters:"
    spreadsheet .submprops.f.nb.pars.tbl 20 {
      {Name    name    {entry $e -textvariable $v -width 20 -bd 1 -relief sunken}}
      {Value   value   {entry $e -textvariable $v -width 20 -bd 1 -relief sunken}}
      {{End-line comment} right-comment {entry $e -textvariable $v -width 15 -bd 1 -relief sunken}}
      {{Doc. comment} banner-comment {entry $e -textvariable $v -width 15 -bd 1 -relief sunken}}
    }
    pack .submprops.f.nb.pars.l  -side top -anchor w
    pack .submprops.f.nb.pars.tbl -expand 1 -fill both

    # create "Gate sizes" page
    label .submprops.f.nb.gates.l -text "Gatesizes:"
    spreadsheet .submprops.f.nb.gates.tbl 20 {
      {Gate   name       {entry $e -textvariable $v -width 16 -bd 1 -relief sunken}}
      {Size   vectorsize {entry $e -textvariable $v -width 20 -bd 1 -relief sunken}}
      {{End-line comment} right-comment {entry $e -textvariable $v -width 15 -bd 1 -relief sunken}}
      {{Doc. comment} banner-comment {entry $e -textvariable $v -width 15 -bd 1 -relief sunken}}
    }
    pack .submprops.f.nb.gates.l  -side top -anchor w
    pack .submprops.f.nb.gates.tbl -expand 1 -fill both

    # create "Machines" page
    label .submprops.f.nb.on.l -text "On machines:"
    spreadsheet .submprops.f.nb.on.tbl 20 {
      {{On machine} mach    {entry $e -textvariable $v -width 16 -bd 1 -relief sunken}}
      {{End-line comment} right-comment {entry $e -textvariable $v -width 15 -bd 1 -relief sunken}}
      {{Doc. comment} banner-comment {entry $e -textvariable $v -width 15 -bd 1 -relief sunken}}
    }
    pack .submprops.f.nb.on.l  -side top -anchor w
    pack .submprops.f.nb.on.tbl -expand 1 -fill both

    focus .submprops.f.nb.general.name.e

    # what is this?
    set typename $ned($key,type-name)
    if {$ned($key,like-name)!=""} {
        if {[itemKeyFromName $ned($key,like-name) module]==""} {
            set ned(def) 0
        } else {
            set ned(def) 1
        }
    } else {
       if {$typename!="" && [itemKeyFromName $typename module]==""} {
           set ned(def) 0
       } else {
           set ned(def) 1
       }
    }

    # fill "General" page with values
    .submprops.f.nb.general.name.e insert 0 $ned($key,name)
    .submprops.f.nb.general.type.e insert 0 $ned($key,type-name)
    .submprops.f.nb.general.like.e insert 0 $ned($key,like-name)
    .submprops.f.nb.general.vs.e insert 0 $ned($key,vectorsize)
    .submprops.f.nb.general.comment.t insert 1.0 $ned($key,banner-comment)

    # fill "Paramters" page with values
    fillSpreadsheetFromNed .submprops.f.nb.pars.tbl [getChildrenWithType $key substparams]


    #
    # Execute dialogs and extract results.
    #

    # set initial focus
    focus .submprops.f.nb.general.name.e

    # exec the dialog, extract its contents if OK was pressed, then delete dialog
    if {[execOkCancelDialog .submprops] == 1} {

        # process "General" page.
        set ned($key,name) [.submprops.f.nb.general.name.e get]
        set typename [.submprops.f.nb.general.type.e get]
        set likename [.submprops.f.nb.general.like.e get]
        set ned($key,vectorsize) [.submprops.f.nb.general.vs.e get]
        set ned($key,banner-comment) [getCommentFromText .submprops.f.nb.general.comment.t]

        # process "Parameters" page.
        updateNedFromSpreadsheet .submprops.f.nb.pars.tbl [getChildrenWithType $key substparams] substparam name
        $gned(canvas) itemconfigure $ned($key,label-cid) -text $ned($key,name)

        if {$likename!=""} {
           set ned($key,like-name) $likename
           set ned($key,type-name) $typename
           if {[itemKeyFromName $likename module]==""} {
              if {$ned(def) == 1} {
                  set modk [addItem module]
                  set ned($modk,name) $likename
                  set ned($modk,parentkey) $ned($ned($key,module-ownerkey),parentkey)
                  openModuleOnNewCanvas $modk
              }
           }
        } else {
           set ned($key,like-name) $likename
           set ned($key,type-name) $typename
           if {$typename!="" && [itemKeyFromName $typename module]==""} {
               if {$ned(def) == 1} {
                   set modk [addItem module]
                   set ned($modk,name) $typename
                   set ned($modk,parentkey) $ned($ned($key,module-ownerkey),parentkey)
                   # FIXME: what's this????
                   openModuleOnNewCanvas $modk
               }
           }
        }
    }
    destroy .submprops
}


