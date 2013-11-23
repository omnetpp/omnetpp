#=================================================================
#  OBJINSP.TCL - part of
#
#                     OMNeT++/OMNEST
#            Discrete System Simulation in C++
#
#=================================================================

#----------------------------------------------------------------#
#  Copyright (C) 1992-2008 Andras Varga
#
#  This file is distributed WITHOUT ANY WARRANTY. See the file
#  `license' for details on this and other legal matters.
#----------------------------------------------------------------#

#
# Misc inspectors: cObject, container, cPar, cMessage, cPacket, cWatch
#


proc createGenericObjectInspector {name geom wantcontentspage focuscontentspage} {
    global icons help_tips

    set w $name
    createInspectorToplevel $w $geom

    if {![regexp {\.(ptr.*)-([0-9]+)} $w match object type]} {
        error "window name $w doesn't look like an inspector window"
    }

    set type [opp_getobjectbaseclass $object]

    if {$type=="cSimpleModule" || $type=="cCompoundModule"} {
        if {$type=="cCompoundModule"} {
            packIconButton $w.toolbar.graph  -image $icons(asgraphics) -command "inspectThis $w {As Graphics}"
            set help_tips($w.toolbar.graph)  {Network graphics}
        }
        packIconButton $w.toolbar.win    -image $icons(asoutput) -command "inspectThis $w {Module output}"
        packIconButton $w.toolbar.sep1   -separator
        set help_tips($w.toolbar.owner)  {Inspect parent module}
        set help_tips($w.toolbar.win)    {See module output}
        moduleInspector:addRunButtons $w
    } else {
        set insptypes [opp_supported_insp_types $object]
        if {[lsearch -exact $insptypes "As Graphics"]!=-1} {
            packIconButton $w.toolbar.graph  -image $icons(asgraphics) -command "inspectThis $w {As Graphics}"
            set help_tips($w.toolbar.graph)  {Inspect graphically}
        }
    }

    set nb [inspector:createNotebook $w]

    inspector:createFields2Page $w

    if {$wantcontentspage} {
        notebook:addPage $nb contents {Contents}
        createInspectorListbox $nb.contents

        if {$focuscontentspage} {
            notebook:showPage $nb contents
        } else {
            notebook:showPage $nb fields2
        }
    }
}


#proc createObjInspector {name geom} {
#
#    set w $name
#    createInspectorToplevel $w $geom
#
#    set nb [inspector:createNotebook $w]
#
#    notebook:addPage $nb info {General}
#
#    # note: experimental page
#    inspector:createFields2Page $w
#
#    # page 1: info
#    label-sunkenlabel $nb.info.name {Name:}   ;# TBD make it editfield
#    label-sunkenlabel $nb.info.fullpath {Full path:} ;# TBD make it disabled editfield, and next ones too
#    label-sunkenlabel $nb.info.class {C++ class:}
#    label-sunkenlabel $nb.info.info {Info:}
#    label-text $nb.info.details {Detailed info:} 10
#
#    pack $nb.info.name -fill x -side top
#    pack $nb.info.fullpath -fill x -side top
#    pack $nb.info.class -fill x -side top
#    pack $nb.info.info -fill x -side top
#    pack $nb.info.details -expand 1 -fill both -side top
#}
#
#proc createContainerInspector {name geom args} {
#    # create a list window
#
#    set w $name
#    set typelist $args
#    createInspectorToplevel $w $geom
#
#    set nb [inspector:createNotebook $w]
#
#    # XXX experimental page
#    inspector:createFields2Page $w
#
#    notebook:addPage $nb contents  {Contents}
#    createInspectorListbox $nb.contents
#}
#
#proc createMessageInspector {name geom} {
#    global fonts icons help_tips
#
#    set w $name
#    createInspectorToplevel $w $geom
#
#    packIconButton $w.toolbar.apply  -image $icons(apply) -command "opp_writebackinspector $w; opp_updateinspectors"
#    packIconButton $w.toolbar.revert -image $icons(revert) -command "opp_updateinspectors"
#
#    set help_tips($w.toolbar.apply)   {Apply changes (Enter)}
#    set help_tips($w.toolbar.revert)  {Revert}
#
#    set nb [inspector:createNotebook $w]
#
#    # XXX experimental page
#    inspector:createFields2Page $w
#
#    notebook:addPage $nb info        {General}
#    notebook:addPage $nb send        {Sending/Arrival}
#    notebook:addPage $nb controlinfo {Control Info}
#    notebook:addPage $nb params      {Params}
#
#    notebook:showPage $nb info
#
#    # page 2: info
#    label-entry $nb.info.name Name:
#    label-entry $nb.info.kind Kind:
#    label-entry $nb.info.length {Length (bits):}
#    label-sunkenlabel $nb.info.lengthb {Length (bytes):}
#    label-entry $nb.info.prio Priority:
#    label-entry $nb.info.error BitErrors:
#    label-button $nb.info.encapmsg {Encapsulated msg:}
#
#    pack $nb.info.name -fill x -side top
#    pack $nb.info.kind -fill x -side top
#    pack $nb.info.length -fill x -side top
#    pack $nb.info.lengthb -fill x -side top
#    pack $nb.info.prio -fill x -side top
#    pack $nb.info.error -fill x -side top
#    pack $nb.info.encapmsg -fill x -side top
#
#    # page 3: send/deliv
#    label-sunkenlabel $nb.send.created Created:
#    label-sunkenlabel $nb.send.sent Sent:
#    label-sunkenlabel $nb.send.arrived Arrival:
#    label-entry $nb.send.tstamp Timestamp:
#    label-button $nb.send.owner {Current owner:}
#    label-button $nb.send.src Sender:
#    label-button $nb.send.dest Destination:
#
#    pack $nb.send.created -fill x -side top
#    pack $nb.send.sent -fill x -side top
#    pack $nb.send.arrived -fill x -side top
#    pack $nb.send.tstamp -fill x -side top
#    pack $nb.send.owner -fill x -side top
#    pack $nb.send.src -fill x -side top
#    pack $nb.send.dest -fill x -side top
#
#    # page 4: params
#    createInspectorListbox $nb.params
#
#    # page 5: control info
#    create_structpanel $nb.controlinfo
#}

proc createWatchInspector {name geom} {
    global fonts

    set w $name
    createInspectorToplevel $w $geom

    frame $w.main
    pack $w.main -anchor center -expand 0 -fill both -side top

    label-entry $w.main.name ""
    $w.main.name.l config -width 20
    focus $w.main.name.e
    pack $w.main.name -fill x -side top
}

#proc createParInspector {name geom} {
#    global fonts icons help_tips
#
#    set w $name
#    createInspectorToplevel $w $geom
#
#    packIconButton $w.toolbar.apply  -image $icons(apply) -command "opp_writebackinspector $w; opp_updateinspectors"
#    packIconButton $w.toolbar.revert -image $icons(revert) -command "opp_updateinspectors"
#
#    set help_tips($w.toolbar.apply)   {Apply changes (Enter)}
#    set help_tips($w.toolbar.revert)  {Revert}
#
#    frame $w.main
#    pack $w.main -anchor center -expand 1 -fill both -side top
#
#    label-entry $w.main.value "Value:"
#    pack $w.main.value -fill x -side top
#}
#
#proc createPacketInspector {name geom} {
#    createMessageInspector $name $geom
#
#    set w $name
#    set nb $w.nb
#
#    label-entry $nb.info.protocol Protocol:
#    label-entry $nb.info.pdu PDU:
#
#    pack $nb.info.protocol -after $nb.info.kind -fill x -side top
#    pack $nb.info.pdu -after $nb.info.protocol -fill x -side top
#}

