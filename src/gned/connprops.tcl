#=================================================================
#  CONNPROPS.TCL - part of
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


proc editConnectionProps {key} {
    global gned ned canvas
    global tmp

    set modkey [getContainingModule $key]
    set tmp(modulekey) $modkey   ;# needed by validation proc...

    # create dialog with OK and Cancel buttons
    set w .connprops
    createOkCancelDialog $w "Connection Properties"
    wm geometry $w "490x380"

    # add notebook pages
    set nb $w.f.nb
    notebook $nb
    pack $nb -expand 1 -fill both
    notebook_addpage $nb general "General"
    notebook_addpage $nb gates "Gates"
    notebook_addpage $nb attrs "Attributes"
    notebook_showpage $nb gates

    # create "General" page
    label-text  $nb.general.comment "Doc. comments:" 6
    label-text  $nb.general.rcomment "End-line comments:" 2
    pack $nb.general.comment -expand 0 -fill x -side top
    pack $nb.general.rcomment -expand 0 -fill x -side top

    # create "Gates" page
    ConnProps:forLoopEdit $nb.gates.for
    label $nb.gates.lfrom -text  "From:"
    ConnProps:gateSpec $nb.gates.from
    label $nb.gates.lto -text  "To:"
    ConnProps:gateSpec $nb.gates.to
    label $nb.gates.ldir -text  "Arrow direction in NED source:"
    radiobutton $nb.gates.r1 -text "source --> destination" -value 1 -variable tmp(l2r)
    radiobutton $nb.gates.r2 -text "destination <-- source" -value 0 -variable tmp(l2r)
    label-entry $nb.gates.condition "Condition:"

    pack $nb.gates.for -expand 0 -fill x -side top
    pack $nb.gates.lfrom  -expand 0 -fill none -side top -anchor w
    pack $nb.gates.from  -expand 0 -fill x -side top
    pack $nb.gates.lto  -expand 0 -fill none -side top -anchor w
    pack $nb.gates.to -expand 0 -fill x -side top
    pack $nb.gates.ldir  -expand 0 -fill none -side top -anchor w
    pack $nb.gates.r1  -expand 0 -fill none -side top -anchor w
    pack $nb.gates.r2  -expand 0 -fill none -side top -anchor w
    pack $nb.gates.condition  -expand 0 -fill x -side top

    # create "Attributes" page
    radiobutton $nb.attrs.r1 -text "Predefined channel:" -value 1 -variable tmp(usechannel) -command "ConnProps:useChannel $w"
    label-combo2 $nb.attrs.channel "  Channel name:" [getChannelNameList]
    radiobutton $nb.attrs.r2 -text "Custom:" -value 0  -variable tmp(usechannel) -command "ConnProps:notUseChannel $w"
    label-entry $nb.attrs.delay "  Prop. delay:"
    label-entry $nb.attrs.datarate "  Data Rate:"
    label-entry $nb.attrs.error "  Bit error rate:"

    #label $nb.attrs.l -text  "  Additional attributes:"
    #tableEdit $nb.attrs.tbl 10 {
    #  {Name               name           {entry $e -textvariable $v -width 20 -bd 1}}
    #  {Value              value          {entry $e -textvariable $v -width 20 -bd 1}}
    #  {{End-line comment} right-comment  {entry $e -textvariable $v -width 15 -bd 1}}
    #  {{Doc. comment}     banner-comment {entry $e -textvariable $v -width 15 -bd 1}}
    #}

    pack $nb.attrs.r1 -expand 0 -fill none -side top -anchor w
    pack $nb.attrs.channel -expand 0 -fill x -side top
    pack $nb.attrs.r2 -expand 0 -fill none -side top -anchor w
    pack $nb.attrs.delay  -expand 0 -fill x -side top
    pack $nb.attrs.datarate -expand 0 -fill x -side top
    pack $nb.attrs.error  -expand 0 -fill x -side top

    #pack $nb.attrs.l -side top -anchor w
    #pack $nb.attrs.tbl -side top -pady 4

    # fill "General" page
    $nb.general.comment.t insert 1.0 $ned($key,banner-comment)
    $nb.general.rcomment.t insert 1.0 $ned($key,right-comment)

    # fill "Gates" page
    ConnProps:fillGateSpec $nb.gates.from $key src $modkey
    ConnProps:fillGateSpec $nb.gates.to $key dest $modkey
    ConnProps:refreshGateCombo $nb.gates.from $modkey
    ConnProps:refreshGateCombo $nb.gates.to $modkey
    $nb.gates.condition.e  insert 0 $ned($key,condition)
    set tmp(l2r) $ned($key,arrowdir-l2r)

    # fill "Attributes" page
    set tmp(usechannel) 0
    ConnProps:notUseChannel $w
     foreach connattr_key [getChildren $key] {
        if {$ned($connattr_key,type)!="connattr"} {error "non-connattr child of conn!"}
        set attrname  $ned($connattr_key,name)
        set attrvalue $ned($connattr_key,value)
        switch $attrname {
            channel   {set tmp(usechannel) 1; ConnProps:useChannel $w; $nb.attrs.channel.e insert 0 $attrvalue}
            delay     {$nb.attrs.delay.e insert 0 $attrvalue}
            error     {$nb.attrs.error.e insert 0 $attrvalue}
            datarate  {$nb.attrs.datarate.e insert 0 $attrvalue}
            default   {error "nonstandard attribute of a conn: $attrname!"}
        }
    }

    # focus on first one
    focus $nb.gates.from.mod.name

    # exec the dialog, extract its contents if OK was pressed, then delete dialog
    if {[execOkCancelDialog $w ConnProps:validate] == 1} {
        # "General" page
        set ned($key,banner-comment) [getCommentFromText $nb.general.comment.t]
        set ned($key,right-comment) [getCommentFromText $nb.general.rcomment.t]

        # 'Gates' page
        set ned($key,condition) [$nb.gates.condition.e get]

        set srcmodname [$nb.gates.from.mod.name get]
        if {$srcmodname=="" || $srcmodname=="<parent>"} {
            set ned($key,src-ownerkey) $modkey
        } else {
            set ned($key,src-ownerkey) [findSubmodule $modkey $srcmodname]
            if {$ned($key,src-ownerkey)==""} {
                # something bad happened (validation didn't work?) -- hide it...
                set ned($key,src-ownerkey) $modkey
            }
        }
        set destmodname [$nb.gates.to.mod.name get]
        if {$destmodname=="" || $destmodname=="<parent>"} {
            set ned($key,dest-ownerkey) $modkey
        } else {
            set ned($key,dest-ownerkey) [findSubmodule $modkey $destmodname]
            if {$ned($key,dest-ownerkey)==""} {
                # something bad happened (validation didn't work?) -- hide it...
                set ned($key,dest-ownerkey) $modkey
            }
        }

        set ned($key,src-mod-index) [$nb.gates.from.mod.index get]
        set ned($key,dest-mod-index) [$nb.gates.to.mod.index get]

        set ned($key,srcgate) [$nb.gates.from.gate.name get]
        set ned($key,destgate) [$nb.gates.to.gate.name get]

        set ned($key,src-gate-index) [$nb.gates.from.gate.index get]
        set ned($key,dest-gate-index) [$nb.gates.to.gate.index get]

        set ned($key,arrowdir-l2r) $tmp(l2r)

        puts "FIXME todo: 'for' to be handled!"

        # 'Attributes' page
        foreach connattr_key [getChildren $key] {
            deleteItem $connattr_key
        }
        if {$tmp(usechannel)} {
            set channel [$nb.attrs.channel.e get]
            if {$channel==""} {
                tk_messageBox -type ok -title GNED -icon warning -message "Channel name not defined! Using 'Channel1' as default."
                set channel "Channel1"
            }
            ConnProps:addConnAttr $key channel $channel
        } else {
            set delay [$nb.attrs.delay.e get]
            set error [$nb.attrs.error.e get]
            set datarate [$nb.attrs.datarate.e get]

            if {$delay!=""} {ConnProps:addConnAttr $key delay $delay}
            if {$error!=""} {ConnProps:addConnAttr $key error $error}
            if {$datarate!=""} {ConnProps:addConnAttr $key datarate $datarate}
        }

        # well redraw is not explicitly needed now, but cannot hurt
        redrawItemOnAnyCanvas $key
        markNedFileOfItemDirty $key
        updateTreeManager
    }
    destroy $w
}


# helper proc
proc ConnProps:notUseChannel {w} {
    set nb $w.f.nb
    $nb.attrs.channel.e  configure -state disabled
    $nb.attrs.delay.e  configure -state normal
    $nb.attrs.error.e  configure -state normal
    $nb.attrs.datarate.e  configure -state normal
}

# helper proc
proc ConnProps:useChannel {w} {
    set nb $w.f.nb
    $nb.attrs.channel.e  configure -state normal
    $nb.attrs.delay.e  configure -state disabled
    $nb.attrs.error.e  configure -state disabled
    $nb.attrs.datarate.e  configure -state disabled
}

# ConnProps:validate --
#
# Validation proc, for use with execOKCancelDialog.
#
proc ConnProps:validate {w} {
    global tmp
    set nb $w.f.nb
    if [catch {
        set modkey $tmp(modulekey) ;# this is ugly, but how otherwise...
        assertSubmodExists $nb.gates.from.mod.name "source submodule" $modkey
        assertSubmodExists $nb.gates.to.mod.name "destination submodule" $modkey

        assertEntryFilledIn $nb.gates.from.gate.name "source gate name"
        assertEntryIsValidName $nb.gates.from.gate.name "source gate name"

        assertEntryFilledIn $nb.gates.to.gate.name "destination gate name"
        assertEntryIsValidName $nb.gates.to.gate.name "destination gate name"

        if {$tmp(usechannel)} {
            assertEntryFilledIn $nb.attrs.channel.e "channel type"
            assertEntryIsValidName $nb.attrs.channel.e "channel type"
        } else {
            # cannot validate delay, error and data rate
        }
    } message] {
        tk_messageBox -parent $w -icon error -type ok -title "Error" -message $message
        return 0
    }
    return 1
}


proc ConnProps:forLoopEdit {w} {
    debug "TODO: 'for' handling to be implemented!"

    frame $w
    label $w.l -text "Managing \"for\"-loops around connections is not possible from this dialog."
    #combo $w.e $list
    #button $w.c -text "Edit..."
    pack $w.l -expand 0 -fill none -padx 2 -pady 2 -side left
    #pack $w.e -expand 1 -fill x -padx 2 -pady 2 -side left
    #pack $w.c -expand 0 -fill none -padx 2 -pady 2 -side left
    #set list {{i=0..1,j=1..5} {i=0..10}}
    #$w.e config -text [lindex $list 0]
}

proc ConnProps:gateSpec {w} {
    frame $w
    frame $w.mod
    frame $w.gate
    pack $w.mod -expand 0 -fill x -side top
    pack $w.gate -expand 0 -fill x -side top

    # add "Module ... index [...]" line
    label $w.mod.l1 -text  "  Module:" -anchor w -width 8
    #label $w.mod.name -width 20 -relief sunken -anchor w
    combobox::combobox $w.mod.name -width 14
    label $w.mod.lb -text  "  index \["
    entry $w.mod.index -width 6
    label $w.mod.rb -text  "\]   "
    pack $w.mod.l1 -expand 0 -side left -padx 2 -pady 2
    pack $w.mod.name  -expand 1 -fill x -side left -padx 2 -pady 2
    pack $w.mod.lb -expand 0 -side left -padx 2 -pady 2
    pack $w.mod.index -expand 1 -fill x -side left -padx 2 -pady 2
    pack $w.mod.rb -expand 0 -side left -padx 2 -pady 2

    # add "Gate ... index [...]" line
    label $w.gate.l1 -text  "  Gate:" -anchor w -width 8
    #entry $w.gate.name -width 14
    #button $w.gate.c -text "..." -width 3
    combobox::combobox $w.gate.name -width 14
    label $w.gate.lb -text  "  index \["
    entry $w.gate.index -width 6
    label $w.gate.rb -text  "\]   "
    pack $w.gate.l1 -expand 0 -side left -padx 2 -pady 2
    pack $w.gate.name  -expand 1 -fill x -side left -padx 2 -pady 2
    #pack [wsize $w.gate.c 20 20] -expand 0 -side left -padx 0 -pady 2
    pack $w.gate.lb -expand 0 -side left -padx 2 -pady 2
    pack $w.gate.index -expand 1 -fill x -side left -padx 2 -pady 2
    pack $w.gate.rb -expand 0 -side left -padx 2 -pady 2
}

proc ConnProps:fillGateSpec {w key srcdest modkey} {
    global ned

    set submodkey $ned($key,${srcdest}-ownerkey)
    #$w.mod.name config -text $ned($submodkey,name)
    comboconfig $w.mod.name [concat "<parent>" [getNameList $modkey submods]]
    $w.mod.name delete 0 end
    if {$ned($submodkey,type)=="submod"} {
        $w.mod.name insert 0 $ned($submodkey,name)
    } else {
        $w.mod.name insert 0 "<parent>"
    }
    $w.mod.index insert 0 $ned($key,${srcdest}-mod-index)
    $w.gate.name insert 0 $ned($key,${srcdest}gate)
    $w.gate.index insert 0 $ned($key,${srcdest}-gate-index)

    bind $w.gate.name <FocusIn> [list ConnProps:refreshGateCombo $w $modkey]
}

proc ConnProps:refreshGateCombo {w modkey} {
    global ned
    #debug "modkey=$modkey"
    #set submodname [$w.mod.name cget -text]
    set submodname [$w.mod.name get]
    if {$submodname=="" || $submodname=="<parent>"} {
        # gates of parent module
        comboconfig $w.gate.name [getNameList $modkey gates]
    } else {
        #
        # find appropriate module definition and look up what gates it has
        #
        set submodkey [findSubmodule $modkey $submodname]
        set submodkey [lindex $submodkey 0]; # to be safe
        #debug "submodkey=$submodkey"
        if {$submodkey==""} {return}
        if {$ned($submodkey,like-name)!=""} {
            set modtypename $ned($submodkey,like-name)
        } else {
            set modtypename $ned($submodkey,type-name)
        }
        set modtypekey [concat [itemKeyFromName $modtypename module] \
                               [itemKeyFromName $modtypename simple]]
        #debug "modtypekey=$modtypekey"
        if {$modtypekey==""} {return}
        # if there are multiple definitions of this type, just take the first one
        set modtypekey [lindex $modtypekey 0]
        #debug "modtypekey-2=$modtypekey"
        comboconfig $w.gate.name [getNameList $modtypekey gates]
    }
}

proc ConnProps:addConnAttr {conn_key attrname attrvalue} {
    global ned

    set key [addItem connattr $conn_key]
    set ned($key,name) $attrname
    set ned($key,value) $attrvalue
    return $key
}

