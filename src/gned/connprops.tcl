#==========================================================================
#  CONNPROPS.TCL -
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


proc editConnectionProps {key} {
    global gned ned canvas

puts "DBG: editConnectionProps: dialog doesn't work yet!"

    # create dialog with OK and Cancel buttons
    createOkCancelDialog .connprops "Connection Properties"
    wm geometry .connprops "490x380"

    # add notebook pages
    set nb .connprops.f.nb
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
    radiobutton $nb.gates.r1 -text "src --> dest" -value 0 -variable tmp(l2r)
    radiobutton $nb.gates.r2 -text "dest <-- src" -value 1 -variable tmp(l2r)
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
    radiobutton $nb.attrs.r1 -text "Predefined channel:" -value 1 -variable tmp(usechannel)
    label-entry $nb.attrs.channel "  Channel name:"
    radiobutton $nb.attrs.r2 -text "Custom:" -value 0  -variable tmp(usechannel)
    label-entry $nb.attrs.delay "  Prop. delay:"
    label-entry $nb.attrs.datarate "  Data Rate:"
    label-entry $nb.attrs.error "  Bit error rate:"

    label $nb.attrs.l -text  "  Additional attributes:"
    tableEdit $nb.attrs.tbl 10 {
      {Name               name           {entry $e -textvariable $v -width 20 -bd 1}}
      {Value              value          {entry $e -textvariable $v -width 20 -bd 1}}
      {{End-line comment} right-comment  {entry $e -textvariable $v -width 15 -bd 1}}
      {{Doc. comment}     banner-comment {entry $e -textvariable $v -width 15 -bd 1}}
    }

    pack $nb.attrs.r1 -expand 0 -fill none -side top -anchor w
    pack $nb.attrs.channel -expand 0 -fill x -side top
    pack $nb.attrs.r2 -expand 0 -fill none -side top -anchor w
    pack $nb.attrs.delay  -expand 0 -fill x -side top
    pack $nb.attrs.datarate -expand 0 -fill x -side top
    pack $nb.attrs.error  -expand 0 -fill x -side top

    pack $nb.attrs.l -side top -anchor w
    pack $nb.attrs.tbl -side top -pady 4

    # fill "Gates" page
    ConnProps:fillGateSpec $nb.gates.from $key src
    ConnProps:fillGateSpec $nb.gates.to $key dest
    $nb.gates.condition.e  insert 0 $ned($key,condition)

    # fill "Attributes" page
if 0 {
    if {$ned($key,channel-ownerkey)!=""} {
       set chown $ned($key,channel-ownerkey)
       $nb.attrs.error.e insert 0 $ned($chown,error)
       $nb.attrs.delay.e insert 0 $ned($chown,delay)
       $nb.attrs.datarate.e insert 0 $ned($chown,datarate)
       $nb.attrs.channel.e insert 0 $ned($chown,name)
    } else {
       # Attributes
       $nb.attrs.error.e insert 0 $ned($key,error)
       $nb.attrs.delay.e insert 0 $ned($key,delay)
       $nb.attrs.datarate.e insert 0 $ned($key,datarate)
       $nb.attrs.channel.e insert 0 $ned($key,channel-name)
    }
}

    # focus on first one
#    focus $nb.gates.srcgate.e

    # exec the dialog, extract its contents if OK was pressed, then delete dialog
    if {[execOkCancelDialog .connprops] == 1} {
        # Gates
        set ned($key,srcgate)  [$nb.gates.srcgate.e get]
        set ned($key,destgate) [$nb.gates.destgate.e get]
        set ned($key,condition) [$nb.gates.condition.e get]

        set ned($key,src_index) [$nb.gates.src_index.e get]
        set ned($key,dest_index) [$nb.gates.dest_index.e get]
        set ned($key,src_gate_index) [$nb.gates.src_gate_index.e get]
        set ned($key,dest_gate_index) [$nb.gates.dest_gate_index.e get]
        set ned($key,for_expression) [$nb.gates.for.e get]

        set chname             [$nb.attrs.channel.e get]
        if {$chname!=""} {
           #megnezem hogy van e ilyen nevu channel
           set chkey [itemKeyFromName $chname channel]
           if {$chkey!=""} {
               set ned($key,channel-ownerkey) $chkey
           } else {
               #kerdest teszek fel
                set reply [tk_messageBox -title "Last chance" -icon warning -type yesno \
                                                  -message "Do you want to Define this channel  now?"]
                if {$reply == "yes"} {
                   # beszurom az uj channelt
                   set chkey [addItem channel]
                   # kitoltom a nevet
                   set ned($chkey,name) $chname
                   # feltoltom ertekekkel
                   set ned($chkey,error)    [$nb.attrs.error.e get]
                   set ned($chkey,delay)    [$nb.attrs.delay.e get]
                   set ned($chkey,datarate) [$nb.attrs.datarate.e get]
                   # kikeresek mindent ahoz hogy be tudja allitani a ownerkeyt
                   set canv_id $gned(canvas_id)
                   set modkey $canvas($canv_id,module-key)
                   set fkey $ned($modkey,parentkey)
                   set ned($chkey,parentkey) $fkey
                   # bejegyzem ehez a connectionhoz tulajdonosnak
                   set ned($key,channel-ownerkey) $chkey
                } else {
                   set ned($key,channel-ownerkey) ""
                   set ned($key,error)    [$nb.attrs.error.e get]
                   set ned($key,delay)    [$nb.attrs.delay.e get]
                   set ned($key,datarate) [$nb.attrs.datarate.e get]
                   set ned($key,channel-name) $chname
                }
           }
        } else {
           set ned($key,channel-ownerkey) ""
           set ned($key,error)    [$nb.attrs.error.e get]
           set ned($key,delay)    [$nb.attrs.delay.e get]
           set ned($key,datarate) [$nb.attrs.datarate.e get]
           set ned($key,channel-name) $chname
        }
    }
    destroy .connprops
}


proc ConnProps:forLoopEdit {w} {
    set list {{i=0..1,j=1..5} {i=0..10}}
    frame $w
    label $w.l -text "For loop around connection:"
    combo $w.e $list
    button $w.c -text "Edit..."
    pack $w.l -expand 0 -fill none -padx 2 -pady 2 -side left
    pack $w.e -expand 1 -fill x -padx 2 -pady 2 -side left
    pack $w.c -expand 0 -fill none -padx 2 -pady 2 -side left
    $w.e config -text [lindex $list 0]
}

proc ConnProps:gateSpec {w} {
    frame $w
    frame $w.mod
    frame $w.gate
    pack $w.mod -expand 0 -fill x -side top
    pack $w.gate -expand 0 -fill x -side top

    # add "Module ... index [...]" line
    label $w.mod.l1 -text  "  Module:" -anchor w -width 8
    label $w.mod.name -width 20 -relief sunken -anchor w
    label $w.mod.lb -text  "  index \["
    entry $w.mod.index -width 8
    label $w.mod.rb -text  "\]   "
    pack $w.mod.l1 -expand 0 -side left -padx 2 -pady 2
    pack $w.mod.name  -expand 1 -fill x -side left -padx 2 -pady 2
    pack $w.mod.lb -expand 0 -side left -padx 2 -pady 2
    pack $w.mod.index -expand 1 -fill x -side left -padx 2 -pady 2
    pack $w.mod.rb -expand 0 -side left -padx 2 -pady 2

    # add "Gate ... index [...]" line
    label $w.gate.l1 -text  "  Gate:" -anchor w -width 8
    entry $w.gate.name -width 17
    button $w.gate.c -text "..." -width 3
    label $w.gate.lb -text  "  index \["
    entry $w.gate.index -width 8
    label $w.gate.rb -text  "\]   "
    pack $w.gate.l1 -expand 0 -side left -padx 2 -pady 2
    pack $w.gate.name  -expand 1 -fill x -side left -padx 2 -pady 2
    pack $w.gate.c -expand 0 -side left -padx 0 -pady 2
    pack $w.gate.lb -expand 0 -side left -padx 2 -pady 2
    pack $w.gate.index -expand 1 -fill x -side left -padx 2 -pady 2
    pack $w.gate.rb -expand 0 -side left -padx 2 -pady 2
}

proc ConnProps:fillGateSpec {w key srcdest} {
    global ned

    set modkey $ned($key,${srcdest}-ownerkey)
    $w.mod.name config -text $ned($modkey,name)
    $w.mod.index insert 0 $ned($key,${srcdest}_index)
    $w.gate.name insert 0 $ned($key,${srcdest}gate)
    $w.gate.index insert 0 $ned($key,${srcdest}_gate_index)
}

