#=================================================================
#  NETWPROPS.TCL - part of
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


proc editNetworkProps {key} {
    global gned ned tmp

    #
    # Set up dialog.
    #

    # create dialog with OK and Cancel buttons
    set w .netprops
    createOkCancelDialog $w "Network Properties"
    wm geometry $w "490x320"

    set nb $w.f.nb

    # add notebook pages
    notebook $nb
    pack $nb -expand 1 -fill both
    notebook_addpage $nb general "General"
    notebook_addpage $nb pars "Parameters"
    #notebook_addpage $nb on "Machines"

    # create "General" page
    label-entry $nb.general.name "Name:"
    #radiobutton $nb.general.r1 -text "Type is fixed:" -value 0  -variable tmp(uselike) -command "NetworkProps:notUseLike $w"
    label-combo2 $nb.general.type "Type:" [getModuleNameList]
    #radiobutton $nb.general.r2 -text "Type is passed in a parameter:" -value 1  -variable tmp(uselike) -command "NetworkProps:useLike $w"
    #label-combo2 $nb.general.likepar "  Parameter name:" [getNameList $ned($ned($key,parentkey),parentkey) "params"]; #FIXME BAD!!!! REMOVE THE WHOLE "LIKE" THING FROM NETWORKS!
    #label-combo2 $nb.general.likemod "  Prototype module:" [getModuleNameList]
    label-text  $nb.general.comment "Comments:" 4

    pack $nb.general.name  -expand 0 -fill x -side top
    #pack $nb.general.r1  -expand 0 -fill none -side top -anchor w
    pack $nb.general.type  -expand 0 -fill x -side top
    #pack $nb.general.r2  -expand 0 -fill none -side top -anchor w
    #pack $nb.general.likepar  -expand 0 -fill x -side top
    #pack $nb.general.likemod  -expand 0 -fill x -side top
    pack $nb.general.comment -expand 0 -fill x -side top

    # fill "General" page with values
    $nb.general.name.e insert 0 $ned($key,name)
    $nb.general.type.e insert 0 $ned($key,type-name)
    #if {$ned($key,like-name)==""} {
    #    set tmp(uselike) 0
    #    $nb.general.type.e insert 0 $ned($key,type-name)
    #    NetworkProps:notUseLike $w
    #} else {
    #    set tmp(uselike) 1
    #    $nb.general.likepar.e insert 0 $ned($key,type-name)
    #    $nb.general.likemod.e insert 0 $ned($key,like-name)
    #    NetworkProps:useLike $w
    #}
    $nb.general.comment.t insert 1.0 $ned($key,banner-comment)

    # create "Parameters" page
    createSectionsComboAndTables $nb.pars $key substparams parameters  {
      {Name    name    {entry $e -textvariable $v -width 20 -bd 1}}
      {Value   value   {entry $e -textvariable $v -width 20 -bd 1}}
      {{End-line comment} right-comment {entry $e -textvariable $v -width 15 -bd 1}}
      {{Doc. comment} banner-comment {entry $e -textvariable $v -width 15 -bd 1}}
    }
    button $nb.pars.xcheck -text "Consult module declaration" -command "showModuleDecl $nb.general.type.e"
    pack $nb.pars.xcheck -side top -anchor w -padx 4 -pady 4

    # create "Machines" page
    #createSectionsComboAndTables $nb.on $key substmachines on  {
    #  {{On machine} value  {entry $e -textvariable $v -width 16 -bd 1}}
    #  {{End-line comment} right-comment {entry $e -textvariable $v -width 24 -bd 1}}
    #  {{Doc. comment} banner-comment {entry $e -textvariable $v -width 30 -bd 1}}
    #}
    #button $nb.on.xcheck -text "Consult module declaration"
    #pack $nb.on.xcheck -side top -anchor w -padx 4 -pady 4

    notebook_showpage $nb general
    focus $nb.general.name.e


    #
    # Execute dialogs and extract results.
    #

    # set initial focus
    focus $nb.general.name.e

    # exec the dialog, extract its contents if OK was pressed, then delete dialog
    if {[execOkCancelDialog $w NetworkProps:validate] == 1} {

        # process "General" page.
        set ned($key,name) [$nb.general.name.e get]
        set ned($key,type-name) [$nb.general.type.e get]
        #if {!$tmp(uselike)} {
        #    set ned($key,type-name) [$nb.general.type.e get]
        #    set ned($key,like-name) ""
        #} else {
        #    set ned($key,type-name) [$nb.general.likepar.e get]
        #    set ned($key,like-name) [$nb.general.likemod.e get]
        #}
        set ned($key,banner-comment) [getCommentFromText $nb.general.comment.t]

        # process "Parameters" page.
        updateNedFromSectionTables $nb.pars $key substparams substparam name

        # process "Machines" page.
        #updateNedFromSectionTables $nb.on $key substmachines substmachine value

        # check if module type specified already exists...
        #if {$ned($key,like-name)!=""} {
        #    set typename $ned($key,like-name)
        #} else {
        #    set typename $ned($key,type-name)
        #}
        #if {[itemKeyFromName $typename module]=="" && [itemKeyFromName $typename simple]==""} {
        #    if {"yes"==[tk_messageBox -type yesno -title GNED -icon question \
        #        -message "Module type '$typename' is unknown to GNED. Create it in this file?"]
        #    } {
        #        tk_messageBox -type ok -title GNED -icon error -message "Not implemented yet :-)"
        #    }
        #}

        markNedFileOfItemDirty $key
        updateTreeManager

    }
    destroy $w
}

# helper proc
proc NetworkProps:notUseLike {w} {
    set nb $w.f.nb
    $nb.general.type.e configure -state normal
    $nb.general.likepar.e configure -state disabled
    $nb.general.likemod.e configure -state disabled
}

# helper proc
proc NetworkProps:useLike {w} {
    set nb $w.f.nb
    $nb.general.type.e configure -state disabled
    $nb.general.likepar.e configure -state normal
    $nb.general.likemod.e configure -state normal
}

# NetworkProps:validate --
#
# Validation proc, for use with execOKCancelDialog.
#
proc NetworkProps:validate {w} {
    global tmp
    set nb $w.f.nb
    if [catch {
        assertEntryFilledIn $nb.general.name.e "network name"
        assertEntryIsValidName $nb.general.name.e "network name"
        # FIXME assertUnique

        assertEntryFilledIn $nb.general.type.e "module type"
        assertEntryIsValidName $nb.general.type.e "module type"

        #if {!$tmp(uselike)} {
        #    assertEntryFilledIn $nb.general.type.e "module type"
        #    assertEntryIsValidName $nb.general.type.e "module type"
        #} else {
        #    assertEntryFilledIn $nb.general.likepar.e "parameter name for module type"
        #    assertEntryIsValidName $nb.general.likepar.e "parameter name for module type"
        #
        #    assertEntryFilledIn $nb.general.likemod.e "prototype module"
        #    assertEntryIsValidName $nb.general.likemod.e "prototype module"
        #}

    } message] {
        tk_messageBox -parent $w -icon error -type ok -title "Error" -message $message
        return 0
    }
    return 1
}



