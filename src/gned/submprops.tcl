#=================================================================
#  SUBMPROPS.TCL - part of
#
#                     OMNeT++/OMNEST
#            Discrete System Simulation in C++
#
#  Parts by: Istvan Pataki, Gabor Vincze ({deity|vinczeg}@sch.bme.hu)
#
#=================================================================

#----------------------------------------------------------------#
#  Copyright (C) 1992-2005 Andras Varga
#
#  This file is distributed WITHOUT ANY WARRANTY. See the file
#  `license' for details on this and other legal matters.
#----------------------------------------------------------------#


proc editSubmoduleProps {key} {
    global gned ned tmp

    #
    # Set up dialog.
    #

    # create dialog with OK and Cancel buttons
    set w .submprops
    createOkCancelDialog $w "Submodule Properties"
    wm geometry $w "490x330"

    set nb $w.f.nb

    # add notebook pages
    notebook $nb
    pack $nb -expand 1 -fill both
    notebook_addpage $nb general "General"
    notebook_addpage $nb pars "Parameters"
    notebook_addpage $nb gates "Gate sizes"
    #notebook_addpage $nb on "Machines"

    # create "General" page
    label-entry $nb.general.name "Name:"
    label-entry $nb.general.vs   "Vector size:"
    radiobutton $nb.general.r1 -text "Type is fixed:" -value 0  -variable tmp(uselike) -command "SubmoduleProps:notUseLike $w"
    label-combo2 $nb.general.type "  Type:" [getModuleNameList]
    radiobutton $nb.general.r2 -text "Type is passed in a parameter:" -value 1  -variable tmp(uselike) -command "SubmoduleProps:useLike $w"
    #label-entry-chooser $nb.general.likepar "  Parameter name:" "" puts
    label-combo2 $nb.general.likepar "  Parameter name:" [getNameList $ned($ned($key,parentkey),parentkey) "params"]; #FIXME BAD!!!!
    label-combo2 $nb.general.likemod "  Prototype module:" [getModuleNameList]
    label-text  $nb.general.comment "Doc. comments:" 3
    label-text  $nb.general.rcomment "End-line comments:" 2

    pack $nb.general.name  -expand 0 -fill x -side top
    pack $nb.general.vs    -expand 0 -fill x -side top
    pack $nb.general.r1  -expand 0 -fill none -side top -anchor w
    pack $nb.general.type  -expand 0 -fill x -side top
    pack $nb.general.r2  -expand 0 -fill none -side top -anchor w
    pack $nb.general.likepar  -expand 0 -fill x -side top
    pack $nb.general.likemod  -expand 0 -fill x -side top
    pack $nb.general.comment -expand 0 -fill x -side top
    pack $nb.general.rcomment -expand 0 -fill x -side top

    # fill "General" page with values
    $nb.general.name.e insert 0 $ned($key,name)
    $nb.general.vs.e insert 0 $ned($key,vectorsize)
    if {$ned($key,like-name)==""} {
        set tmp(uselike) 0
        $nb.general.type.e insert 0 $ned($key,type-name)
        SubmoduleProps:notUseLike $w
    } else {
        set tmp(uselike) 1
        $nb.general.likepar.e insert 0 $ned($key,type-name)
        $nb.general.likemod.e insert 0 $ned($key,like-name)
        SubmoduleProps:useLike $w
    }
    $nb.general.comment.t insert 1.0 $ned($key,banner-comment)
    $nb.general.rcomment.t insert 1.0 $ned($key,right-comment)

    # create "Parameters" page
    createSectionsComboAndTables $nb.pars $key substparams parameters  {
      {Name    name    {entry $e -textvariable $v -width 20 -bd 1}}
      {Value   value   {entry $e -textvariable $v -width 20 -bd 1}}
      {{End-line comment} right-comment {entry $e -textvariable $v -width 15 -bd 1}}
      {{Doc. comment} banner-comment {entry $e -textvariable $v -width 15 -bd 1}}
    }
    button $nb.pars.xcheck -text "Consult module declaration" -command "showModuleDecl $nb.general.type.e"
    pack $nb.pars.xcheck -side top -anchor w -padx 4 -pady 4

    # create "Gate sizes" page
    createSectionsComboAndTables $nb.gates $key gatesizes gatesizes  {
      {Gate   name     {entry $e -textvariable $v -width 16 -bd 1}}
      {Size   size     {entry $e -textvariable $v -width 20 -bd 1}}
      {{End-line comment} right-comment {entry $e -textvariable $v -width 16 -bd 1}}
      {{Doc. comment} banner-comment {entry $e -textvariable $v -width 18 -bd 1}}
    }
    button $nb.gates.xcheck -text "Consult module declaration" -command "showModuleDecl $nb.general.type.e"
    pack $nb.gates.xcheck -side top -anchor w -padx 4 -pady 4

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
    if {[execOkCancelDialog $w SubmoduleProps:validate] == 1} {

        # process "General" page.
        set ned($key,name) [$nb.general.name.e get]
        set ned($key,vectorsize) [$nb.general.vs.e get]

        if {!$tmp(uselike)} {
            set ned($key,type-name) [$nb.general.type.e get]
            set ned($key,like-name) ""
        } else {
            set ned($key,type-name) [$nb.general.likepar.e get]
            set ned($key,like-name) [$nb.general.likemod.e get]
        }
        set ned($key,banner-comment) [getCommentFromText $nb.general.comment.t]
        set ned($key,right-comment) [getCommentFromText $nb.general.rcomment.t]

        # process "Parameters" page.
        updateNedFromSectionTables $nb.pars $key substparams substparam name

        # process "Gatesizes" page.
        updateNedFromSectionTables $nb.gates $key gatesizes gatesize name

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

        # redraw because name might have changed
        redrawItemOnAnyCanvas $key
        markNedFileOfItemDirty $key
        updateTreeManager
    }
    destroy $w
}

# helper proc
proc SubmoduleProps:notUseLike {w} {
    set nb $w.f.nb
    $nb.general.type.e configure -state normal
    $nb.general.likepar.e configure -state disabled
    $nb.general.likemod.e configure -state disabled
}

# helper proc
proc SubmoduleProps:useLike {w} {
    set nb $w.f.nb
    $nb.general.type.e configure -state disabled
    $nb.general.likepar.e configure -state normal
    $nb.general.likemod.e configure -state normal
}

# editSubmoduleProps:validate --
#
# Validation proc, for use with execOKCancelDialog.
#
proc SubmoduleProps:validate {w} {
    global tmp
    set nb $w.f.nb
    if [catch {
        assertEntryFilledIn $nb.general.name.e "submodule name"
        assertEntryIsValidName $nb.general.name.e "submodule name"
        # FIXME assertUnique

        if {!$tmp(uselike)} {
            assertEntryFilledIn $nb.general.type.e "module type"
            assertEntryIsValidName $nb.general.type.e "module type"
        } else {
            assertEntryFilledIn $nb.general.likepar.e "parameter name for module type"
            assertEntryIsValidName $nb.general.likepar.e "parameter name for module type"

            assertEntryFilledIn $nb.general.likemod.e "prototype module"
            assertEntryIsValidName $nb.general.likemod.e "prototype module"
        }
        # FIXME validate tables too
    } message] {
        tk_messageBox -parent $w -icon error -type ok -title "Error" -message $message
        return 0
    }
    return 1
}


# createSectionsComboAndTables --
#
# private (helper) procedure for editSubmoduleProps
#
proc createSectionsComboAndTables {frame submodkey sectiontype keyword columnlist} {
    global ned
    global fonts

    # get list of sections into $keylist and $cond(key)
    set keylist [getChildrenWithType $submodkey $sectiontype]
    if {[llength $keylist]==0} {
        set keylist {new}
        set cond(new) {}
    } else {
        foreach key $keylist {
            set cond($key) $ned($key,condition)
        }
    }

    # assemble command for combo that packs/unpacks tables
    set unmapTablesCmd "pack forget"
    foreach key $keylist {
        append unmapTablesCmd " " $frame.tbl$key
    }

    # create and pack combo widget
    set w $frame.combo
    frame $w
    label $w.l -anchor w -text "Section:"

    menubutton $w.e -menu $w.e.m -relief raised -width 30 -indicatoron 1 -font $fonts(normal)
    menu $w.e.m -tearoff 0
    foreach key $keylist {
        if {$cond($key)==""} {
           set lbl "$keyword: (unconditional)"
        } else {
           set lbl "$keyword if $cond($key):"
        }
        $w.e.m add command -label $lbl -command "$w.e configure -text \{$lbl\}; $unmapTablesCmd; pack $frame.tbl$key -after $frame.combo -expand 1 -fill both"
    }
    pack $w.l -anchor center  -padx 2 -pady 2 -side left
    pack [wsize $w.e 200 20] -anchor center  -padx 2 -pady 2 -side left

    pack $w -expand 0 -fill x -side top

    # create tables, fill them and map first one (indirectly via combo)
    foreach key $keylist {
        if {$key=="new"} {
            tableEdit $frame.tbl$key 15 $columnlist
        } else {
            set numlines [expr [llength [getChildren $key]]+15]
            tableEdit $frame.tbl$key $numlines $columnlist
            fillTableEditFromNed $frame.tbl$key $key
        }
    }
    $w.e.m invoke 0
}


# updateNedFromSectionTables --
#
# private (helper) procedure for editSubmoduleProps
#
proc updateNedFromSectionTables {frame submodkey sectiontype itemtype keyattr} {

    # get list of sections into $keylist
    set keylist [getChildrenWithType $submodkey $sectiontype]
    if {[llength $keylist]==0} {
        set keylist {new}
    }

    # update each section
    foreach key $keylist {
        if {$key=="new"} {
            set newkey [addItem $sectiontype $submodkey]
            set isempty [updateNedFromTableEdit $frame.tbl$key $newkey $itemtype $keyattr]
            if {$isempty} {
                deleteItem $newkey
            }
        } else {
            updateNedFromTableEdit $frame.tbl$key $key $itemtype $keyattr
        }
    }
}



