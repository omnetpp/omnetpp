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
# General procs for inspectors
#
# Misc inspectors: objects, containers, cPar, cMessage,
#    (non-graphical) module inspectors etc.
#

#===================================================================
#    HELPER FUNCTIONS FOR INSPECTOR WINDOWS
#===================================================================

proc create_inspector_toplevel {w {geom ""}} {

    # create toplevel inspector window

    global fonts icons help_tips

    toplevel $w -class Toplevel
    wm focusmodel $w passive
    if {$geom != ""} {wm geometry $w $geom}
    wm maxsize $w 1009 738
    wm minsize $w 1 1
    wm overrideredirect $w 0
    wm resizable $w 1 1
    wm protocol $w WM_DELETE_WINDOW "opp_deleteinspector $w"

    # add the "Inspect As.." icon at the top
    frame $w.toolbar -relief raised -borderwidth 1
    pack $w.toolbar -anchor w -side top -fill x -expand 0

    iconbutton $w.toolbar.sep0 -separator
    iconbutton $w.toolbar.inspect -image $icons(as) -command "inspect_this_as $w"
    foreach i {sep0 inspect} {
       pack $w.toolbar.$i -anchor n -side left -padx 0 -pady 2
    }

    set help_tips($w.toolbar.inspect) {Open another inspector for this object}

    bind $w <Escape>     "catch {.popup unpost}"
    bind $w <Button-1>   "catch {.popup unpost}"
    bind $w <Key-Return> "opp_writebackinspector $w; opp_updateinspectors"
}

proc create_inspector_listbox {w} {
    # create a listbox

    label $w.label -text "# objects:"
    pack $w.label -side top -anchor w

    frame $w.main
    pack $w.main -expand 1 -fill both -side top
    listbox $w.main.list  -width 60 -yscrollcommand "$w.main.sb set"
    scrollbar $w.main.sb  -command "$w.main.list yview"
    pack $w.main.sb -anchor center -expand 0 -fill y -side right
    pack $w.main.list -expand 1 -fill both  -side left

    bind $w.main.list <Double-Button-1> "inspect_item_in $w.main.list"
    bind $w.main.list <Button-3> "popup_insp_menu \[lindex \[$w.main.list get @%x,%y\] 0\] %X %Y"
    bind $w.main.list <Key-Return> "inspect_item_in $w.main.list"

    focus $w.main.list
}

proc popup_insp_menu {ptr X Y} {

   if {$ptr==""} return
   set insptypes [opp_supported_insp_types $ptr]

   catch {destroy .popup}
   menu .popup -tearoff 0
   foreach type $insptypes {
      .popup add command -label "$type..." -command "opp_inspect $ptr \{$type\}"
   }
   .popup post $X $Y
}

proc ask_inspectortype {ptr} {
    set w .asktype
    createOkCancelDialog $w {Inspector type...}
    label-combo $w.f.type {Inspect} [opp_supported_insp_types $ptr]
    pack $w.f.type -expand 0 -fill x -side top

    set type ""
    if [execOkCancelDialog $w] {
        set type [$w.f.type.e cget -text]
    }
    destroy $w
    return $type
}

proc inspect_item_in {lb} {
    # called on double-clicking in a container inspector;
    # inspect the current item in the listbox of an inspector listwindow

    set sel [$lb curselection]
    if {$sel != ""} {
        set ptr [lindex [$lb get $sel] 0]
        opp_inspect $ptr {(default)}
    }
}

proc inspectas_item_in {lb} {
    # called on double-clicking in a container inspector;
    # inspect the current item in the listbox of an inspector listwindow

    set sel [$lb curselection]
    if {$sel != ""} {
        set ptr [lindex [$lb get $sel] 0]
        set type [ask_inspectortype $ptr]
        if {$type != ""} {
            opp_inspect $ptr $type
        }
    }
}

proc inspect_this_as {win} {
    # called by the "Inspect As.." button at the TOP of an inspector
    # extract object pointer from window path name and create inpector
    regexp {\.(ptr.*)-[0-9]+} $win match ptr

    set type [ask_inspectortype $ptr]
    if {$type != ""} {
        opp_inspect $ptr $type
    }
}

proc inspect_this {win type} {
    # extract object pointer from window path name and create inspector
    regexp {\.(ptr.*)-[0-9]+} $win match object
    opp_inspect $object $type
}

#===================================================================
#    INSPECTOR WINDOWS
#===================================================================

proc create_objinspector {name} {
    # create default inspector: displays the info() string
    set w $name
    create_inspector_toplevel $w

    frame $w.main -borderwidth 1
    label $w.main.lbl -text "\"object->info()\" string:"
    message $w.main.info -relief sunken -aspect 1000 -justify left

    pack $w.main -anchor center -expand 1 -fill both -side top
    pack $w.main.lbl -anchor center -expand 1 -fill both -side top
    pack $w.main.info -anchor center -expand 1 -fill both -side top
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

    notebook_addpage $nb info     {General}
    notebook_addpage $nb send     {Sending/Arrival}
    # notebook_addpage $nb fields {Fields}
    notebook_addpage $nb params   {Params}

    # page 1: info
    label-entry $nb.info.name Name:
    label-entry $nb.info.kind Kind:
    label-entry $nb.info.length Length:
    label-entry $nb.info.prio Priority:
    label-entry $nb.info.error BitErrors:
    button $nb.info.encapmsg -font $fonts(normal) -width 30

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
    # ---future extension---

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
