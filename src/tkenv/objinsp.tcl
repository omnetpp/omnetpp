#==========================================================================
#  OBJINSP.TCL -
#            part of the Tcl/Tk windowing environment of
#                            OMNeT++
#==========================================================================

#----------------------------------------------------------------#
#  Copyright (C) 1992-2001 Andras Varga
#  Technical University of Budapest, Dept. of Telecommunications,
#  Stoczek u.2, H-1111 Budapest, Hungary.
#
#  This file is distributed WITHOUT ANY WARRANTY. See the file
#  `license' for details on this and other legal matters.
#----------------------------------------------------------------#

#
# Misc inspectors: cObject, container, cPar, cMessage, cPacket, cWatch
#

proc create_objinspector {name} {

    set w $name
    create_inspector_toplevel $w

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

    pack $nb.info.name -expand 1 -fill x -side top
    pack $nb.info.fullpath -expand 1 -fill x -side top
    pack $nb.info.class -expand 1 -fill x -side top
    pack $nb.info.info -expand 1 -fill x -side top

    # page 2: fields
    if {$fieldspage_needed} {
        create_structpanel $nb.fields
    }
}

proc create_containerinspector {name args} {
    # create a list window

    set w $name
    set typelist $args
    create_inspector_toplevel $w

    # Create buttons at the bottom of the dialog.
    frame $w.buttons
    pack $w.buttons -expand 0 -fill x -side bottom -padx 5 -pady 5
    button $w.buttons.inspect -text {Inspect} -command "inspect_item_in $w.main.list"
    button $w.buttons.inspectas -text {Inspect As...} -command "inspectas_item_in $w.main.list"
    pack $w.buttons.inspectas -side right -expand 0
    pack $w.buttons.inspect -side right -expand 0

    create_inspector_listbox $w
}

proc create_messageinspector {name} {
    global fonts

    set w $name
    create_inspector_toplevel $w

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
    label-entry $nb.info.class Class:
    label-entry $nb.info.name Name:
    label-entry $nb.info.kind Kind:
    label-entry $nb.info.length Length:
    label-entry $nb.info.prio Priority:
    label-entry $nb.info.error BitErrors:
    button $nb.info.encapmsg -font $fonts(normal) -width 30

    pack $nb.info.class -expand 1 -fill x -side top
    pack $nb.info.name -expand 1 -fill x -side top
    pack $nb.info.kind -expand 1 -fill x -side top
    pack $nb.info.length -expand 1 -fill x -side top
    pack $nb.info.prio -expand 1 -fill x -side top
    pack $nb.info.error -expand 1 -fill x -side top
    pack $nb.info.encapmsg -expand 1 -fill x -side top

    # page 2: send/deliv
    label-sunkenlabel $nb.send.created Created:
    label-sunkenlabel $nb.send.sent Sent:
    label-sunkenlabel $nb.send.arrived Arrival:
    label-entry $nb.send.tstamp Timestamp:
    button $nb.send.owner -font $fonts(normal) -width 30
    button $nb.send.src -font $fonts(normal) -width 30
    button $nb.send.dest -font $fonts(normal) -width 30

    pack $nb.send.created -expand 1 -fill x -side top
    pack $nb.send.sent -expand 1 -fill x -side top
    pack $nb.send.arrived -expand 1 -fill x -side top
    pack $nb.send.tstamp -expand 1 -fill x -side top
    pack $nb.send.owner -expand 1 -fill x -side top
    pack $nb.send.src -expand 1 -fill x -side top
    pack $nb.send.dest -expand 1 -fill x -side top

    # page 3: fields
    if {$fieldspage_needed} {
        create_structpanel $nb.fields
    }

    # page 4: params
    create_inspector_listbox $nb.params
}

proc create_watchinspector {name} {
    global fonts

    set w $name
    create_inspector_toplevel $w

    frame $w.main
    pack $w.main -anchor center -expand 0 -fill both -side top

    label-entry $w.main.name xxx
    $w.main.name.l config -width 20
    focus $w.main.name.e
    pack $w.main.name -anchor center -expand 1 -fill x -side top
}

proc create_parinspector {name} {
    global fonts

    set w $name
    create_inspector_toplevel $w

    frame $w.main
    frame $w.bot
    pack $w.main -anchor center -expand 0 -fill both -side top
    pack $w.bot -anchor center -expand 0 -fill x -side bottom

    label-entry $w.main.value Value:
    label-sunkenlabel $w.main.type Type:
    label-entry $w.main.newtype {New type:}
    label-entry $w.main.prompt Prompt:
    label-entry $w.main.input {Input flag:}

    pack $w.main.value -anchor center -expand 1 -fill x -side top
    pack $w.main.type -anchor center -expand 1 -fill x -side top
    pack $w.main.newtype -anchor center -expand 1 -fill x -side top
    pack $w.main.prompt -anchor center -expand 1 -fill x -side top
    pack $w.main.input -anchor center -expand 1 -fill x -side top

    button $w.bot.indirection -font $fonts(normal)
    pack $w.bot.indirection -anchor center -expand 1 -fill x -side left
}


proc create_packetinspector {name} {
    create_messageinspector $name

    set w $name
    set nb $w.nb

    label-entry $nb.info.protocol Protocol:
    label-entry $nb.info.pdu PDU:

    pack $nb.info.protocol -after $nb.info.kind -expand 1 -fill x -side top
    pack $nb.info.pdu -after $nb.info.protocol -expand 1 -fill x -side top
}

