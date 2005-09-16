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
    global gned ned canvas config
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
    focus $nb.gates.from.modname

    # exec the dialog, extract its contents if OK was pressed, then delete dialog
    if {[execOkCancelDialog $w ConnProps:validate] == 1} {
        # "General" page
        set ned($key,banner-comment) [getCommentFromText $nb.general.comment.t]
        set ned($key,right-comment) [getCommentFromText $nb.general.rcomment.t]

        # 'Gates' page
        set ned($key,condition) [$nb.gates.condition.e get]

        set srcmodname [$nb.gates.from.modname get]
        if {$srcmodname=="" || $srcmodname=="<parent>"} {
            set ned($key,src-ownerkey) $modkey
        } else {
            splitIndexedName $srcmodname srcmodname0 ned($key,src-mod-index) dummy
            set ned($key,src-ownerkey) [findSubmodule $modkey $srcmodname0]
            if {$ned($key,src-ownerkey)==""} {
                # something bad happened (validation didn't work?) -- hide it...
                set ned($key,src-ownerkey) $modkey
            }
        }
        set destmodname [$nb.gates.to.modname get]
        if {$destmodname=="" || $destmodname=="<parent>"} {
            set ned($key,dest-ownerkey) $modkey
        } else {
            splitIndexedName $destmodname destmodname0 ned($key,dest-mod-index) dummy
            set ned($key,dest-ownerkey) [findSubmodule $modkey $destmodname0]
            if {$ned($key,dest-ownerkey)==""} {
                # something bad happened (validation didn't work?) -- hide it...
                set ned($key,dest-ownerkey) $modkey
            }
        }

        set srcgate [$nb.gates.from.gatename get]
        splitIndexedName $srcgate ned($key,srcgate) ned($key,src-gate-index) ned($key,src-gate-plusplus)

        set destgate [$nb.gates.to.gatename get]
        splitIndexedName $destgate ned($key,destgate) ned($key,dest-gate-index) ned($key,dest-gate-plusplus)

        # gate indices cannot be "..." here any more, because we assert that in the validation code

        if {$config(autosize)} {
            ConnProps:fillGateSize $key src
            ConnProps:fillGateSize $key dest
        }

        set ned($key,arrowdir-l2r) $tmp(l2r)

        # FIXME todo: 'for' to be handled

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
        assertSubmodExists $nb.gates.from.modname "source submodule" $modkey
        assertSubmodExists $nb.gates.to.modname "destination submodule" $modkey

        assertEntryFilledIn $nb.gates.from.gatename "source gate name"
        assertEntryIsValidGateSpec $nb.gates.from.gatename "source gate name"

        assertEntryFilledIn $nb.gates.to.gatename "destination gate name"
        assertEntryIsValidGateSpec $nb.gates.to.gatename "destination gate name"

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
    label $w.l1 -text  "Module:" -anchor w -width 8
    combobox::combobox $w.modname -width 25
    label $w.l2 -text  "  Gate:" -anchor w -width 8
    combobox::combobox $w.gatename -width 25
    pack $w.l1 $w.modname $w.l2 $w.gatename -expand 0 -side left -padx 2 -pady 2
}

proc ConnProps:fillGateSize {key srcdest} {
    global ned

    set max [getMaxGateIndex $key $srcdest]

    # if not all indices to this gate are numeric, give up
    if {$max==-1} {
        return
    }

    # generate/delete gatesize entries for the source/destination gates
    set wrk_module $ned($key,${srcdest}-ownerkey)
    set gsizkey [getChildrenWithType $wrk_module gatesizes]
    set gskey ""

    if {($gsizkey == "") && ($max!=-1)} {
        set gsizkey [addItem gatesizes $wrk_module]
    }

    if {$gsizkey != ""} {
        set gskey [getChildrenWithName $gsizkey $ned($key,${srcdest}gate)]
    }

    if {$max!=-1} {
        if {$gskey == ""} {
           set gskey [addItem gatesize $gsizkey]
           set ned($gskey,name) $ned($key,${srcdest}gate)
           set ned($gskey,size) 1
           set ned($gskey,right-comment) " TBD\n"
        }

        set ned($gskey,size) [expr $max + 1]
    } else {
        if {$gskey != ""} {
            deleteItem $gskey
        }
    }
}

proc ConnProps:fillGateSpec {w key srcdest modkey} {
    global ned

    set submodkey $ned($key,${srcdest}-ownerkey)
    comboconfig $w.modname [concat "<parent>" [getSubmodNameList $modkey "\[...\]"]]
    $w.modname delete 0 end
    if {$ned($submodkey,type)=="submod"} {
        if {$ned($key,${srcdest}-mod-index)!=""} {
            $w.modname insert 0 "$ned($submodkey,name)\[$ned($key,${srcdest}-mod-index)\]"
        } else {
            $w.modname insert 0 $ned($submodkey,name)
        }
    } else {
        $w.modname insert 0 "<parent>"
    }
    ConnProps:refreshGateCombo $w $modkey $srcdest
    $w.gatename delete 0 end
    if {$ned($key,${srcdest}-gate-plusplus)} {
        $w.gatename insert 0 "$ned($key,${srcdest}gate)++"
    } elseif {$ned($key,${srcdest}-gate-index)!=""} {
        $w.gatename insert 0 "$ned($key,${srcdest}gate)\[$ned($key,${srcdest}-gate-index)\]"
    } else {
        $w.gatename insert 0 $ned($key,${srcdest}gate)
    }

    bind $w.gatename <FocusIn> [list ConnProps:refreshGateCombo $w $modkey $srcdest]
}

proc set_plusplus {connkey srcdest} {
    global ned

    set modkey $ned($connkey,${srcdest}-ownerkey)

    if {$ned($modkey,type)=="submod"} {
        #
        # find appropriate module definition and look up what gates it has
        #
        set submodkey [findSubmodule [getContainingModule $connkey] $ned($modkey,name)]
        set submodkey [lindex $submodkey 0]; # to be safe

        if {$submodkey==""} {
            set ned($connkey,${srcdest}-gate-plusplus) {1}
            return
        }

        if {$ned($submodkey,like-name)!=""} {
            set modtypename $ned($submodkey,like-name)
        } else {
            set modtypename $ned($submodkey,type-name)
        }
        set modtypekey [concat [itemKeyFromName $modtypename module] \
                               [itemKeyFromName $modtypename simple]]

        if {$modtypekey==""} {
            set ned($connkey,${srcdest}-gate-plusplus) {1}
            return
        }

        # if there are multiple definitions of this type, just take the first one
        set modtypekey [lindex $modtypekey 0]
        set wrk_gates [getChildrenWithType $modtypekey gates]

        if {$wrk_gates != ""} {
           set wrk_gate [getChildrenWithName $wrk_gates $ned($connkey,${srcdest}gate)]
           if {$wrk_gate=="" || ($ned($connkey,${srcdest}-gate-index) == "" && $ned($wrk_gate,isvector))} {
              set ned($connkey,${srcdest}-gate-plusplus) {1}
           } else {
              set ned($connkey,${srcdest}-gate-plusplus) {0}
           }
        }
    } else {
        # gates of parent module
        set wrk_gates [getChildrenWithType $modkey gates]
        if {$wrk_gates != ""} {
           set wrk_gate [getChildrenWithName $wrk_gates $ned($connkey,${srcdest}gate)]
           if {$wrk_gate != "" && $ned($wrk_gate,isvector)} {
              if {$ned($connkey,${srcdest}-gate-index) == ""} {
                 set ned($connkey,${srcdest}-gate-index) [expr [getMaxGateIndex $connkey $srcdest] + 1]
              }
           } else {
              set ned($connkey,${srcdest}-gate-index) {}
           }
        }
        set ned($connkey,${srcdest}-gate-plusplus) {0}
    }
}

proc ConnProps:refreshGateCombo {w modkey srcdest} {
    global ned config

    #debug "modkey=$modkey"
    set submodname [$w.modname get]
    #debug "$srcdest submodname=$submodname"
    if {$submodname=="" || $submodname=="<parent>"} {
        # gates of parent module
        if {$srcdest=="src"} {set gtype "in"} else {set gtype "out"}
        # if {$config(autoextend)} {set vecsuffix "++"} else {set vecsuffix "\[...\]"}   # "++" not allowed on parent module's gates
        #set vecsuffix "\[...\]"
        # hack: comboconfig overwrites editbox content too, so save and restore it
        set oldtext [$w.gatename get]
        comboconfig $w.gatename [getGateNameList $modkey $modkey $modkey $gtype]
        $w.gatename delete 0 end
        $w.gatename insert 0 $oldtext
    } else {
        #
        # find appropriate module definition and look up what gates it has
        #
        splitIndexedName $submodname submodname0 index dummy
        set submodkey [findSubmodule $modkey $submodname0]
        set submodkey [lindex $submodkey 0]; # to be safe
        #debug "submodkey=$submodkey"
        if {$submodkey==""} {return}
        if {$ned($submodkey,like-name)!=""} {
            set modtypename $ned($submodkey,like-name)
        } else {
            set modtypename $ned($submodkey,type-name)
        }
        #debug "$srcdest submodtype=$modtypename"
        set modtypekey [concat [itemKeyFromName $modtypename module] \
                               [itemKeyFromName $modtypename simple]]
        #debug "modtypekey=$modtypekey"
        if {$modtypekey==""} {
            comboconfig $w.gatename [list "" "? ($modtypename unknown)"]
            return
        }
        # if there are multiple definitions of this type, just take the first one
        set modtypekey [lindex $modtypekey 0]
        if {$srcdest=="src"} {set gtype "out"} else {set gtype "in"}
        #if {$config(autoextend)} {set vecsuffix "++"} else {set vecsuffix "\[...\]"}
        # hack: comboconfig overwrites editbox content too, so save and restore it
        set oldtext [$w.gatename get]
        comboconfig $w.gatename [getGateNameList $modkey $submodkey $modtypekey $gtype]
        $w.gatename delete 0 end
        $w.gatename insert 0 $oldtext
    }
}

proc ConnProps:addConnAttr {conn_key attrname attrvalue} {
    global ned

    set key [addItem connattr $conn_key]
    set ned($key,name) $attrname
    set ned($key,value) $attrvalue
    return $key
}

