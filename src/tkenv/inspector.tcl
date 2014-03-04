#=================================================================
#  INSPECTOR.TCL - part of
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
# Invoked from concrete inspector subtypes
#
proc createInspectorToplevel {w geom} {
    global config fonts icons help_tips
    global B2 B3

    # Create window
    toplevel $w -class Toplevel
    wm focusmodel $w passive

    set state "normal"
    regexp {(.*):(.*)} $geom dummy geom state
    catch {wm geometry $w $geom}
    catch {wm state $w $state}

    wm minsize $w 1 1
    wm overrideredirect $w 0
    wm resizable $w 1 1
    wm protocol $w WM_DELETE_WINDOW "inspector:close $w"

    # Stay on top
    if {$config(keep-inspectors-on-top)} {
        makeTransient $w $geom
    }

    # Create toolbar
    frame $w.toolbar -relief raised -bd 1
    pack $w.toolbar -anchor w -side top -fill x -expand 0

    packIconButton $w.toolbar.sep0 -separator
    packIconButton $w.toolbar.owner -image $icons(parent) ;#command assigned from C++
    packIconButton $w.toolbar.copyobj -image $icons(copy) -command "inspector:namePopup $w $w.toolbar.copyobj"
    packIconButton $w.toolbar.objs -image $icons(findobj) -command "inspectFilteredObjectList $w"
    packIconButton $w.toolbar.sep01 -separator
    packIconButton $w.toolbar.inspectas -image $icons(as) -command "inspector:inspectAsPopup $w $w.toolbar.inspectas"

    set help_tips($w.toolbar.owner) {Inspect owner}
    set help_tips($w.toolbar.copyobj) {Copy name, type or pointer}
    set help_tips($w.toolbar.objs) {Find objects (Ctrl+S)}
    set help_tips($w.toolbar.inspectas) {Inspect}

    # Create info bar
    frame $w.infobar -relief raised -bd 1
    label $w.infobar.icon -anchor w -relief flat -image $icons(cogwheel_vs)
    label $w.infobar.name -anchor w -relief flat -justify left
    pack $w.infobar.icon -anchor n -side left -expand 0 -fill y -pady 1
    pack $w.infobar.name -anchor n -side left -expand 1 -fill both -pady 1
    pack $w.infobar -anchor w -side top -fill x -expand 0

    # Keyboard bindings
    bind $w <Escape>     "catch {.popup unpost}"
    bind $w <Button-1>   "catch {.popup unpost}"
    bind $w <Key-Return> "opp_commitinspector $w; opp_refreshinspectors"

    bindRunCommands $w
    bindOtherCommands $w
}

proc inspector:createInternalToolbar {w {parent ""}} {
    if {$parent==""} {set parent $w}
    frame $w.toolbar -border 2 -relief groove
    place $w.toolbar -in $parent -relx 1.0 -rely 0 -anchor ne -x -2 -y 2
    return $w.toolbar
}

#
# Invoked from C++
#
proc inspector:onSetObject {w} {
    set ptr [opp_inspector_getobject $w]
    set icon [inspector:getIconForObject $ptr]
    $w.infobar.icon config -image $icon
}

# icons used in the tree view and listboxes
set treeicons(cCompoundModule) {compound_vs}
set treeicons(cSimpleModule)   {simple_vs}
set treeicons(cPlaceholderModule) {placeholder_vs}
set treeicons(cGate)           {gate_vs}
set treeicons(cPar)            {param_vs}
set treeicons(cMessage)        {message_vs}
set treeicons(cQueue)          {queue_vs}
set treeicons(cLinkedList)     {queue_vs}
set treeicons(cArray)          {container_vs}
set treeicons(cMessageHeap)    {container_vs}
set treeicons(cChannel)        {chan_vs}
set treeicons(cStatistic)      {stat_vs}
set treeicons(cOutVector)      {outvect_vs}

#
# Returns the icon of an object (for tree view / listbox)
#
proc inspector:getIconForObject {ptr} {
    global icons treeicons
    if [opp_isnull $ptr] {
       return $icons(cogwheel_vs)  ;#FIXME make "null" icon
    }
    set class [opp_getobjectbaseclass $ptr]
    if [info exists treeicons($class)] {
       return $icons($treeicons($class))
    } else {
       return $icons(cogwheel_vs)
    }
}

#
# Invoked by the WM (Window Manager)
#
proc inspector:close {w} {
    # invokes app->deleteInspector(insp)
    opp_deleteinspector $w
}


#
# Called from C++
#
proc inspector:hostObjectDeleted {w} {
}

#
# Called from C++
#
proc inspector:destroy {w} {
    destroy $w
}

#
# Called from C++
#
proc inspector:show {w} {
    showWindow $w
}

proc inspector:namePopup {w toolbutton} {
    set ptr [opp_inspector_getobject $w]
    regsub {^ptr} $ptr {0x} p

    catch {destroy .popup}
    menu .popup -tearoff 0
    .popup add command -label "Copy pointer with cast (for debugger)" -command [list setClipboard "(([opp_getobjectfield $ptr className] *)$p)"]
    .popup add command -label "Copy pointer value (for debugger)" -command [list setClipboard $p]
    .popup add separator
    .popup add command -label "Copy full path" -command [list setClipboard [opp_getobjectfullpath $ptr]]
    .popup add command -label "Copy name" -command [list setClipboard [opp_getobjectfullname $ptr]]
    .popup add command -label "Copy class name" -command [list setClipboard [opp_getobjectfield $ptr className]]

    set x [winfo rootx $toolbutton]
    set y [expr [winfo rooty $toolbutton]+[winfo height $toolbutton]]
    tk_popup .popup $x $y
}

proc inspector:inspectAsPopup {w toolbutton} {
    set ptr [opp_inspector_getobject $w]
    set curtype [opp_inspectortype [opp_inspector_gettype $w]]
    set typelist [opp_supported_insp_types $ptr]

    catch {destroy .popup}
    menu .popup -tearoff 0
    foreach type $typelist {
        if {$type==$curtype} {set state disabled} else {set state normal}
        .popup add command -label "$type" -command [list opp_inspect $ptr $type] -state $state
    }

    set x [winfo rootx $toolbutton]
    set y [expr [winfo rooty $toolbutton]+[winfo height $toolbutton]]
    tk_popup .popup $x $y
}

