#=================================================================
#  OBJINSP.TCL - part of
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

#
# Misc inspectors: cObject, container, cPar, cMessage, cPacket, cWatch
#

proc create_objinspector {name geom} {

    set w $name
    create_inspector_toplevel $w $geom

    set nb $w.nb
    notebook $nb
    $nb config -width 460 -height 260
    pack $nb -expand 1 -fill both

    set fieldspage_needed [opp_hasdescriptor $w]

    notebook_addpage $nb info {General}
    if {$fieldspage_needed} {
        notebook_addpage $nb fields {Fields}
    }

    # page 1: info
    label-sunkenlabel $nb.info.name {Name:}   ;# TBD make it editfield
    label-sunkenlabel $nb.info.fullpath {Full path:} ;# TBD make it disabled editfield, and next ones too
    label-sunkenlabel $nb.info.class {C++ class:}
    label-sunkenlabel $nb.info.info {Info:}
    label-message $nb.info.details {Detailed info:} ;# TBD make it disabled text

    pack $nb.info.name -fill x -side top
    pack $nb.info.fullpath -fill x -side top
    pack $nb.info.class -fill x -side top
    pack $nb.info.info -fill x -side top
    pack $nb.info.details -fill x -side top

    # page 2: fields
    if {$fieldspage_needed} {
        create_structpanel $nb.fields
        notebook_showpage $nb fields
    }
}

proc create_containerinspector {name geom args} {
    # create a list window

    set w $name
    set typelist $args
    create_inspector_toplevel $w $geom

    # Create buttons at the bottom of the dialog.
    #frame $w.buttons
    #pack $w.buttons -expand 0 -fill x -side bottom -padx 5 -pady 5
    #button $w.buttons.inspect -text {Inspect} -command "inspect_item_in $w.main.list"
    #button $w.buttons.inspectas -text {Inspect As...} -command "inspectas_item_in $w.main.list"
    #pack $w.buttons.inspectas -side right -expand 0
    #pack $w.buttons.inspect -side right -expand 0

    create_inspector_listbox $w
}

proc create_messageinspector {name geom} {
    global fonts icons help_tips

    set w $name
    create_inspector_toplevel $w $geom

    iconbutton $w.toolbar.apply  -image $icons(apply) -command "opp_writebackinspector $w; opp_updateinspectors"
    iconbutton $w.toolbar.revert -image $icons(revert) -command "opp_updateinspectors"
    pack $w.toolbar.revert -anchor n -side right -padx 0 -pady 2
    pack $w.toolbar.apply -anchor n -side right -padx 0 -pady 2

    set help_tips($w.toolbar.apply)   {Apply changes (Enter)}
    set help_tips($w.toolbar.revert)  {Revert}

    set nb $w.nb
    notebook $nb
    $nb config -width 460 -height 260
    pack $nb -expand 1 -fill both

    set fieldspage_needed [opp_hasdescriptor $w]

    notebook_addpage $nb info     {General}
    notebook_addpage $nb send     {Sending/Arrival}
    if {$fieldspage_needed} {
        notebook_addpage $nb fields {Fields}
    }
    notebook_addpage $nb controlinfo {Control Info}
    notebook_addpage $nb params      {Params}

    notebook_showpage $nb info

    # page 1: info
    label-entry $nb.info.name Name:
    label-entry $nb.info.kind Kind:
    label-entry $nb.info.length {Length (bits):}
    label-sunkenlabel $nb.info.lengthb {Length (bytes):}
    label-entry $nb.info.prio Priority:
    label-entry $nb.info.error BitErrors:
    label-button $nb.info.encapmsg {Encapsulated msg:}

    pack $nb.info.name -fill x -side top
    pack $nb.info.kind -fill x -side top
    pack $nb.info.length -fill x -side top
    pack $nb.info.lengthb -fill x -side top
    pack $nb.info.prio -fill x -side top
    pack $nb.info.error -fill x -side top
    pack $nb.info.encapmsg -fill x -side top

    # page 2: send/deliv
    label-sunkenlabel $nb.send.created Created:
    label-sunkenlabel $nb.send.sent Sent:
    label-sunkenlabel $nb.send.arrived Arrival:
    label-entry $nb.send.tstamp Timestamp:
    label-button $nb.send.owner {Current owner:}
    label-button $nb.send.src Sender:
    label-button $nb.send.dest Destination:

    pack $nb.send.created -fill x -side top
    pack $nb.send.sent -fill x -side top
    pack $nb.send.arrived -fill x -side top
    pack $nb.send.tstamp -fill x -side top
    pack $nb.send.owner -fill x -side top
    pack $nb.send.src -fill x -side top
    pack $nb.send.dest -fill x -side top

    # page 3: fields
    if {$fieldspage_needed} {
        create_structpanel $nb.fields
    }

    # page 4: params
    create_inspector_listbox $nb.params

    # page 5: control info
    create_structpanel $nb.controlinfo
}

proc create_watchinspector {name geom} {
    global fonts

    set w $name
    create_inspector_toplevel $w $geom

    frame $w.main
    pack $w.main -anchor center -expand 0 -fill both -side top

    label-entry $w.main.name ""
    $w.main.name.l config -width 20
    focus $w.main.name.e
    pack $w.main.name -fill x -side top
}

proc create_parinspector {name geom} {
    global fonts icons help_tips

    set w $name
    create_inspector_toplevel $w $geom

    iconbutton $w.toolbar.apply  -image $icons(apply) -command "opp_writebackinspector $w; opp_updateinspectors"
    iconbutton $w.toolbar.revert -image $icons(revert) -command "opp_updateinspectors"
    pack $w.toolbar.revert -anchor n -side right -padx 0 -pady 2
    pack $w.toolbar.apply -anchor n -side right -padx 0 -pady 2

    set help_tips($w.toolbar.apply)   {Apply changes (Enter)}
    set help_tips($w.toolbar.revert)  {Revert}

    frame $w.main
    pack $w.main -anchor center -expand 1 -fill both -side top

    label-entry $w.main.value Value:
    label-sunkenlabel $w.main.type Type:
    label-entry $w.main.newtype {New type:}
    label-entry $w.main.prompt Prompt:
    label-entry $w.main.input {Input flag:}
    label-button $w.main.indirection {Redirected to:}

    pack $w.main.value -fill x -side top
    pack $w.main.type -fill x -side top
    pack $w.main.newtype -fill x -side top
    pack $w.main.prompt -fill x -side top
    pack $w.main.input -fill x -side top
    pack $w.main.indirection -fill x -side top
}


proc create_packetinspector {name geom} {
    create_messageinspector $name $geom

    set w $name
    set nb $w.nb

    label-entry $nb.info.protocol Protocol:
    label-entry $nb.info.pdu PDU:

    pack $nb.info.protocol -after $nb.info.kind -fill x -side top
    pack $nb.info.pdu -after $nb.info.protocol -fill x -side top
}

