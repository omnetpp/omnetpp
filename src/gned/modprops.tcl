#=================================================================
#  MODPROPS.TCL - part of
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


proc editModuleProps {key} {
    global gned ned config tmp_nocheck

    # create dialog with OK and Cancel buttons
    set w .modprops
    createOkCancelDialog $w "Module Properties"
    wm geometry $w "490x300"

    set nb $w.f.nb

    # add notebook pages
    notebook $nb
    pack $nb -expand 1 -fill both
    notebook_addpage $nb general "General"
    notebook_addpage $nb pars  "Parameters"
    notebook_addpage $nb gates "Gates"
    #notebook_addpage $nb mach  "Machines"

    # create "General" page
    label-entry $nb.general.name "Name:"
    label-text  $nb.general.comment "Doc. comments:" 4
    label-text  $nb.general.rcomment "End-line comments:" 2
    checkbutton $nb.general.r1 -text "Allow unconnected gates inside (\"nocheck\")" -variable tmp_nocheck
    pack $nb.general.name  -expand 0 -fill x -side top
    pack $nb.general.comment -expand 1 -fill both -side top
    pack $nb.general.rcomment -expand 0 -fill x -side top
    if {$ned($key,type)=="module"} {
        # don't show this control for simple modules
        pack $nb.general.r1  -expand 0 -fill none -side top -anchor w
    }

    # create "Parameters" page
    label $nb.pars.l -text  "Parameters:"
    tableEdit $nb.pars.tbl 15 {
      {Name               name           {entry $e -textvariable $v -width 20 -bd 1}}
      {Type               datatype       {entry $e -textvariable $v -width 20 -bd 1}}
      {{End-line comment} right-comment  {entry $e -textvariable $v -width 15 -bd 1}}
      {{Doc. comment}     banner-comment {entry $e -textvariable $v -width 15 -bd 1}}
    }
    pack $nb.pars.l -side top -anchor w
    pack $nb.pars.tbl -side top

    # create "Gates" page
    label $nb.gates.l -text  "Gates:"
    tableEdit $nb.gates.tbl 15 {
      {Name               name           {entry $e -textvariable $v -width 20 -bd 1}}
      {Direction          gatetype       {entry $e -textvariable $v -width 10 -bd 1}}
      {Vector?            isvector       {entry $e -textvariable $v -width 8  -bd 1}}
      {{End-line comment} right-comment  {entry $e -textvariable $v -width 15 -bd 1}}
      {{Doc. comment}     banner-comment {entry $e -textvariable $v -width 16 -bd 1}}
    }
    pack $nb.gates.l -side top -anchor w
    pack $nb.gates.tbl -side top

    # create "Machines" page
    #label $nb.mach.l -text  "Machines:"
    #tableEdit $nb.mach.tbl 15 {
    #  {Name               name           {entry $e -textvariable $v -width 20 -bd 1}}
    #  {{End-line comment} right-comment  {entry $e -textvariable $v -width 25 -bd 1}}
    #  {{Doc. comment}     banner-comment {entry $e -textvariable $v -width 26 -bd 1}}
    #}
    #pack $nb.mach.l -side top -anchor w
    #pack $nb.mach.tbl -side top

    notebook_showpage $nb general
    focus $nb.general.name.e

    # fill "General" page
    $nb.general.name.e insert 0 $ned($key,name)
    $nb.general.comment.t insert 1.0 $ned($key,banner-comment)
    $nb.general.rcomment.t insert 1.0 $ned($key,right-comment)

    if {$ned($key,type)=="module"} {
        # simple modules don't have "connections:"
        set connskey [getChildrenWithType $key conns]
        if {[llength $connskey]==0} {
            set connskey [addItem conns $key]
            set ned($connskey,nocheck) $config(noportcheck)
        }
        set tmp_nocheck $ned($connskey,nocheck)
    }

    # fill tables
    ModProps:fillTableEditFromNed $nb.pars.tbl  $key params
    ModProps:fillTableEditFromNed $nb.gates.tbl $key gates
    #ModProps:fillTableEditFromNed $nb.mach.tbl  $key machines

    # exec the dialog, extract its contents if OK was pressed, then delete dialog
    if {[execOkCancelDialog $w ModProps:validate] == 1} {

        set ned($key,name) [$nb.general.name.e get]
        set ned($key,banner-comment) [getCommentFromText $nb.general.comment.t]
        set ned($key,right-comment) [getCommentFromText $nb.general.rcomment.t]

        if {$ned($key,type)=="module"} {
            # simple modules don't have "connections:"
            set ned($connskey,nocheck) $tmp_nocheck
            if {$tmp_nocheck} {
                showTextOnceDialog "nocheckSet"
            }
        }

        ModProps:updateNedFromTableEdit $nb.pars.tbl  $key params   param   name
        ModProps:updateNedFromTableEdit $nb.gates.tbl $key gates    gate    name
        #ModProps:updateNedFromTableEdit $nb.mach.tbl  $key machines machine name

        # redraw because name might have changed
        redrawItemOnAnyCanvas $key
        markNedFileOfItemDirty $key
        updateTreeManager
        adjustWindowTitle
    }
    destroy $w
}


# ModProps:validate --
#
# Validation proc, for use with execOKCancelDialog.
#
proc ModProps:validate {w} {
    set nb $w.f.nb
    if [catch {
        assertEntryFilledIn $nb.general.name.e "module name"
        assertEntryIsValidName $nb.general.name.e "module name"
        # FIXME assertUnique
        # FIXME validate tables too
    } message] {
        tk_messageBox -parent $w -icon error -type ok -title "Error" -message $message
        return 0
    }
    return 1
}

proc ModProps:fillTableEditFromNed {w modkey sectiontype} {
    set sectionkey [getChildrenWithType $modkey $sectiontype]
    if {[llength $sectionkey]!=0} {
        fillTableEditFromNed $w $sectionkey
    }
}

proc ModProps:updateNedFromTableEdit {w modkey sectiontype itemtype keyattr} {
    set sectionkey [getChildrenWithType $modkey $sectiontype]
    if {[llength $sectionkey]==0} {
        set sectionkey [addItem $sectiontype $modkey]
    }
    set isempty [updateNedFromTableEdit $w $sectionkey $itemtype $keyattr]
    if {$isempty} {
        deleteItem $sectionkey
    }
}

