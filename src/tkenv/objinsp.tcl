#==========================================================================
#  OBJINSP.TCL -
#            part of the Tcl/Tk windowing environment of
#                            OMNeT++
#==========================================================================

#----------------------------------------------------------------#
#  Copyright (C) 1992-2003 Andras Varga
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
    $nb config  -width 300 -height 200
    pack $nb -expand 1 -fill both

    set fieldspage_needed [opp_hasdescriptor $w]

    notebook_addpage $nb info {General}
    if {$fieldspage_needed} {
        notebook_addpage $nb fields {Fields}
    }

    # page 1: info
    label-sunkenlabel $nb.info.name {Name:}
    label-sunkenlabel $nb.info.fullpath {Full path:}
    label-sunkenlabel $nb.info.class {C++ class:}
    label-sunkenlabel $nb.info.info {obj->info() says:}

    pack $nb.info.name -fill x -side top
    pack $nb.info.fullpath -fill x -side top
    pack $nb.info.class -fill x -side top
    pack $nb.info.info -fill x -side top

    # page 2: fields
    if {$fieldspage_needed} {
        create_structpanel $nb.fields
    }
}

proc create_containerinspector {name geom args} {
    # create a list window

    set w $name
    set typelist $args
    create_inspector_toplevel $w $geom

    # Create buttons at the bottom of the dialog.
    frame $w.buttons
    pack $w.buttons -expand 0 -fill x -side bottom -padx 5 -pady 5
    button $w.buttons.inspect -text {Inspect} -command "inspect_item_in $w.main.list"
    button $w.buttons.inspectas -text {Inspect As...} -command "inspectas_item_in $w.main.list"
    pack $w.buttons.inspectas -side right -expand 0
    pack $w.buttons.inspect -side right -expand 0

    create_inspector_listbox $w
}

proc create_messageinspector {name geom} {
    global fonts

    set w $name
    create_inspector_toplevel $w $geom

    set nb $w.nb
    notebook $nb
    $nb config  -width 300 -height 200
    pack $nb -expand 1 -fill both

    set fieldspage_needed [opp_hasdescriptor $w]

    notebook_addpage $nb info     {General}
    notebook_addpage $nb send     {Sending/Arrival}
    if {$fieldspage_needed} {
        notebook_addpage $nb fields {Fields}
    }
    notebook_addpage $nb params   {Params}

    # page 1: info
    label-entry $nb.info.name Name:
    label-entry $nb.info.kind Kind:
    label-entry $nb.info.length Length:
    label-entry $nb.info.prio Priority:
    label-entry $nb.info.error BitErrors:
    label-button $nb.info.encapmsg {Encapsulated msg:}

    pack $nb.info.name -fill x -side top
    pack $nb.info.kind -fill x -side top
    pack $nb.info.length -fill x -side top
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
}

proc create_watchinspector {name geom} {
    global fonts

    set w $name
    create_inspector_toplevel $w $geom

    frame $w.main
    pack $w.main -anchor center -expand 0 -fill both -side top

    label-entry $w.main.name xxx
    $w.main.name.l config -width 20
    focus $w.main.name.e
    pack $w.main.name -fill x -side top
}

proc create_parinspector {name geom} {
    global fonts

    set w $name
    create_inspector_toplevel $w $geom

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
    create_messageinspector $name

    set w $name
    set nb $w.nb

    label-entry $nb.info.protocol Protocol:
    label-entry $nb.info.pdu PDU:

    pack $nb.info.protocol -after $nb.info.kind -fill x -side top
    pack $nb.info.pdu -after $nb.info.protocol -fill x -side top
}

