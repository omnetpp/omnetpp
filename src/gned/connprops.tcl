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

    tk_messageBox -icon warning -type ok -title GNED -message "Sorry! This dialog hasn't been implemented yet."
    return

    # create dialog with OK and Cancel buttons
    createOkCancelDialog .connprops "Connection Properties"

    # add notebook pages
    notebook .connprops.f.nb
    pack .connprops.f.nb -expand 1 -fill both
    notebook_addpage .connprops.f.nb gates "Gates"
    notebook_addpage .connprops.f.nb props "Properties"
    notebook_addpage .connprops.f.nb loop  "Loop"

    # add entry fields for Gates
    label-entry .connprops.f.nb.gates.srcgate "From gate:"
    label-entry .connprops.f.nb.gates.destgate "To gate:"
    label-entry .connprops.f.nb.gates.condition "Condition:"
    label-text  .connprops.f.nb.gates.comment "Comments:" 2
    # Properties
    label-entry .connprops.f.nb.props.error "Bit error rate:"
    label-entry .connprops.f.nb.props.delay "Prop. delay:"
    label-entry .connprops.f.nb.props.datarate "Data Rate:"
    label-entry .connprops.f.nb.props.channel "Channel name:"
    # Loop
    label-entry .connprops.f.nb.loop.src_index "Source Index:"
    label-entry .connprops.f.nb.loop.dest_index "Destination Index:"
    label-entry .connprops.f.nb.loop.src_gate_index "Source Gate Index:"
    label-entry .connprops.f.nb.loop.dest_gate_index "Destinaton Gate Index:"
    label-entry .connprops.f.nb.loop.for "For Expression:"
    # Gates
    pack .connprops.f.nb.gates.srcgate  -expand 0 -fill x -side top
    pack .connprops.f.nb.gates.destgate  -expand 0 -fill x -side top
    pack .connprops.f.nb.gates.condition  -expand 0 -fill x -side top
    pack .connprops.f.nb.gates.comment -expand 1 -fill both -side top
    # Properties
    pack .connprops.f.nb.props.error  -expand 0 -fill x -side top
    pack .connprops.f.nb.props.delay  -expand 0 -fill x -side top
    pack .connprops.f.nb.props.datarate -expand 0 -fill x -side top
    pack .connprops.f.nb.props.channel -expand 0 -fill x -side top
    # Loop
    pack .connprops.f.nb.loop.src_index  -expand 0 -fill x -side top
    pack .connprops.f.nb.loop.dest_index  -expand 0 -fill x -side top
    pack .connprops.f.nb.loop.src_gate_index  -expand 0 -fill x -side top
    pack .connprops.f.nb.loop.dest_gate_index  -expand 0 -fill x -side top
    pack .connprops.f.nb.loop.for  -expand 0 -fill x -side top

    # Gates
    .connprops.f.nb.gates.srcgate.e insert 0 $ned($key,srcgate)
    .connprops.f.nb.gates.destgate.e insert 0 $ned($key,destgate)
    .connprops.f.nb.gates.condition.e  insert 0 $ned($key,condition)
    .connprops.f.nb.gates.comment.t insert 1.0 $ned($key,comment)
    if {$ned($key,channel-ownerkey)!=""} {
        set chown $ned($key,channel-ownerkey)
       .connprops.f.nb.props.error.e insert 0 $ned($chown,error)
       .connprops.f.nb.props.delay.e insert 0 $ned($chown,delay)
       .connprops.f.nb.props.datarate.e insert 0 $ned($chown,datarate)
       .connprops.f.nb.props.channel.e insert 0 $ned($chown,name)
    } else {
        # Properties
       .connprops.f.nb.props.error.e insert 0 $ned($key,error)
       .connprops.f.nb.props.delay.e insert 0 $ned($key,delay)
       .connprops.f.nb.props.datarate.e insert 0 $ned($key,datarate)
       .connprops.f.nb.props.channel.e insert 0 $ned($key,channel-name)
    }
    # Loop
    .connprops.f.nb.loop.src_index.e  insert 0 $ned($key,src_index)
    .connprops.f.nb.loop.dest_index.e  insert 0 $ned($key,dest_index)
    .connprops.f.nb.loop.src_gate_index.e  insert 0 $ned($key,src_gate_index)
    .connprops.f.nb.loop.dest_gate_index.e insert 0 $ned($key,dest_gate_index)
    .connprops.f.nb.loop.for.e insert 0 $ned($key,for_expression)
    # focus on first one
    focus .connprops.f.nb.gates.srcgate.e

    # exec the dialog, extract its contents if OK was pressed, then delete dialog
    if {[execOkCancelDialog .connprops] == 1} {
        # Gates
        set ned($key,srcgate)  [.connprops.f.nb.gates.srcgate.e get]
        set ned($key,destgate) [.connprops.f.nb.gates.destgate.e get]
        set ned($key,condition) [.connprops.f.nb.gates.condition.e get]
        set ned($key,comment)  [.connprops.f.nb.gates.comment.t get 1.0 end]
        # Loop
        set ned($key,src_index) [.connprops.f.nb.loop.src_index.e get]
        set ned($key,dest_index) [.connprops.f.nb.loop.dest_index.e get]
        set ned($key,src_gate_index) [.connprops.f.nb.loop.src_gate_index.e get]
        set ned($key,dest_gate_index) [.connprops.f.nb.loop.dest_gate_index.e get]
        set ned($key,for_expression) [.connprops.f.nb.loop.for.e get]

        set chname             [.connprops.f.nb.props.channel.e get]
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
                   set ned($chkey,error)    [.connprops.f.nb.props.error.e get]
                   set ned($chkey,delay)    [.connprops.f.nb.props.delay.e get]
                   set ned($chkey,datarate) [.connprops.f.nb.props.datarate.e get]
                   # kikeresek mindent ahoz hogy be tudja allitani a ownerkeyt
                   set canv_id $gned(canvas_id)
                   set modkey $canvas($canv_id,module-key)
                   set fkey $ned($modkey,parentkey)
                   set ned($chkey,parentkey) $fkey
                   # bejegyzem ehez a connectionhoz tulajdonosnak
                   set ned($key,channel-ownerkey) $chkey
                } else {
                   set ned($key,channel-ownerkey) ""
                   set ned($key,error)    [.connprops.f.nb.props.error.e get]
                   set ned($key,delay)    [.connprops.f.nb.props.delay.e get]
                   set ned($key,datarate) [.connprops.f.nb.props.datarate.e get]
                   set ned($key,channel-name) $chname
                }
           }
        } else {
           set ned($key,channel-ownerkey) ""
           set ned($key,error)    [.connprops.f.nb.props.error.e get]
           set ned($key,delay)    [.connprops.f.nb.props.delay.e get]
           set ned($key,datarate) [.connprops.f.nb.props.datarate.e get]
           set ned($key,channel-name) $chname
        }
    }
    destroy .connprops
}

