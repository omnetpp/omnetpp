#=================================================================
#  CHANPROPS.TCL - part of
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


proc editChannelProps {key} {
    global gned ned canvas

#    tk_messageBox -title "GNED" -icon warning -type ok \
#                 -message "Dialog implementation not finished yet"
#    return

    # create dialog with OK and Cancel buttons
    set w .chanprops
    createOkCancelDialog $w "Channel Properties"
    wm geometry $w "380x260"

    # add notebook pages
    set nb $w.f.nb
    notebook $nb
    pack $nb -expand 1 -fill both
    notebook_addpage $nb general "General"
    notebook_addpage $nb attrs "Channel attributes"

    # create "General" page
    label-entry $nb.general.name "Name:"
    label-text  $nb.general.comment "Doc. comments:" 6
    label-text  $nb.general.rcomment "End-of-line comments:" 2
    pack $nb.general.name -expand 0 -fill x -side top
    pack $nb.general.comment -expand 0 -fill x -side top
    pack $nb.general.rcomment -expand 0 -fill x -side top

    # add "Attributes" page
    label $nb.attrs.l -text  "Attributes:"

    label-entry $nb.attrs.delay "  Prop. delay:"
    label-entry $nb.attrs.datarate "  Data Rate:"
    label-entry $nb.attrs.error "  Bit error rate:"

    # tableEdit $nb.attrs.tbl 10 {
    #  {Name               name           {entry $e -textvariable $v -width 20 -bd 1}}
    #  {Value              value          {entry $e -textvariable $v -width 20 -bd 1}}
    #  {{End-line comment} right-comment  {entry $e -textvariable $v -width 15 -bd 1}}
    #  {{Doc. comment}     banner-comment {entry $e -textvariable $v -width 15 -bd 1}}
    # }
    # button $nb.attrs.def -text "Add standard atttributes"

    pack $nb.attrs.l -side top -anchor w
    pack $nb.attrs.delay  -expand 0 -fill x -side top
    pack $nb.attrs.datarate -expand 0 -fill x -side top
    pack $nb.attrs.error  -expand 0 -fill x -side top
    # pack $nb.attrs.tbl -side top
    # pack $nb.attrs.def -side top -anchor w -padx 4 -pady 4

    # fill page
    $nb.general.name.e insert 0 $ned($key,name)
    $nb.general.comment.t insert 1.0 $ned($key,banner-comment)
    $nb.general.rcomment.t insert 1.0 $ned($key,right-comment)
    foreach chanattr_key [getChildren $key] {
        if {$ned($chanattr_key,type)!="chanattr"} {error "non-chanattr child of chan!"}
        set attrname  $ned($chanattr_key,name)
        set attrvalue $ned($chanattr_key,value)
        switch $attrname {
            delay     {$nb.attrs.delay.e insert 0 $attrvalue}
            error     {$nb.attrs.error.e insert 0 $attrvalue}
            datarate  {$nb.attrs.datarate.e insert 0 $attrvalue}
            default   {error "nonstandard attribute of a chan: $attrname!"}
        }
    }
    # fillTableEditFromNed $nb.attrs.tbl $key
    # debug "editChannelProps: implement (Add standard attributes) button"

    notebook_showpage $nb general

    # exec the dialog, extract its contents if OK was pressed, then delete dialog
    if {[execOkCancelDialog $w ChanProps:validate] == 1} {
        set ned($key,name) [$nb.general.name.e get]
        set ned($key,banner-comment) [getCommentFromText $nb.general.comment.t]
        set ned($key,right-comment) [getCommentFromText $nb.general.rcomment.t]

        foreach chanattr_key [getChildren $key] {
            deleteItem $chanattr_key
        }
        set delay [$nb.attrs.delay.e get]
        set error [$nb.attrs.error.e get]
        set datarate [$nb.attrs.datarate.e get]

        if {$delay!=""} {ChanProps:addChanAttr $key delay $delay}
        if {$error!=""} {ChanProps:addChanAttr $key error $error}
        if {$datarate!=""} {ChanProps:addChanAttr $key datarate $datarate}

        # updateNedFromTableEdit $nb.attrs.tbl $key chanattr name

        markNedFileOfItemDirty $key
        updateTreeManager

    }
    destroy $w
}


# ChanProps:validate --
#
# Validation proc, for use with execOKCancelDialog.
#
proc ChanProps:validate {w} {
    set nb $w.f.nb
    if [catch {
        assertEntryFilledIn $nb.general.name.e "channel name"
        assertEntryIsValidName $nb.general.name.e "channel name"
        # FIXME assertUnique
        # FIXME validate tables too
    } message] {
        tk_messageBox -parent $w -icon error -type ok -title "Error" -message $message
        return 0
    }
    return 1
}


proc ChanProps:addChanAttr {chan_key attrname attrvalue} {
    global ned

    set key [addItem chanattr $chan_key]
    set ned($key,name) $attrname
    set ned($key,value) $attrvalue
    return $key
}

