#=================================================================
#  INSPECTOR.TCL - part of
#
#                     OMNeT++/OMNEST
#            Discrete System Simulation in C++
#
#=================================================================

#----------------------------------------------------------------#
#  Copyright (C) 1992-2017 Andras Varga
#
#  This file is distributed WITHOUT ANY WARRANTY. See the file
#  `license' for details on this and other legal matters.
#----------------------------------------------------------------#


#
# Invoked from concrete inspector subtypes
#
proc createInspectorToplevel {insp geom} {
    global config icons help_tips
    global B2 B3 CTRL_

    # Create window
    toplevel $insp -class Toplevel
    wm focusmodel $insp passive

    set state "normal"
    regexp {(.*):(.*)} $geom dummy geom state
    catch {wm geometry $insp $geom}
    catch {wm state $insp $state}

    wm minsize $insp 1 1
    wm overrideredirect $insp 0
    wm resizable $insp 1 1
    wm protocol $insp WM_DELETE_WINDOW "inspector:close $insp"

    # Stay on top
    if {$config(keep-inspectors-on-top)} {
        makeTransient $insp $geom
    }
    if {$geom==""} {
        placeWindowNearMouse $insp
    }

    # Create toolbar
    ttk::frame $insp.toolbar
    pack $insp.toolbar -anchor w -side top -fill x -expand 0

    packToolbutton $insp.toolbar.sep0 -separator
    packToolbutton $insp.toolbar.back -image $icons(back) -command "inspector:back $insp"
    packToolbutton $insp.toolbar.forward -image $icons(forward) -command "inspector:forward $insp"
    packToolbutton $insp.toolbar.parent -image $icons(parent) -command "inspector:inspectParent $insp"
    packToolbutton $insp.toolbar.sep01 -separator
    packToolbutton $insp.toolbar.inspectas -image $icons(inspectas) -command "inspector:inspectAsPopup $insp $insp.toolbar.inspectas"
    packToolbutton $insp.toolbar.copyobj -image $icons(copyptr) -command "inspector:namePopup $insp $insp.toolbar.copyobj"
    packToolbutton $insp.toolbar.objs -image $icons(findobj) -command "inspectFilteredObjectList $insp"

    set help_tips($insp.toolbar.back) "Back"
    set help_tips($insp.toolbar.forward) "Forward"
    set help_tips($insp.toolbar.parent) "Go to parent"
    set help_tips($insp.toolbar.inspectas) "Inspect"
    set help_tips($insp.toolbar.copyobj) "Copy name, type or pointer"
    set help_tips($insp.toolbar.objs) "Find objects (${CTRL_}S)"

    # Keyboard bindings
    bind $insp <Escape>     "catch {.popup unpost}"
    bind $insp <Button-1>   "catch {.popup unpost}"
    bind $insp <Key-Return> "opp_commitinspector $insp; opp_refreshinspectors"

    bindRunCommands $insp
    bindOtherCommands $insp $insp
}

proc inspector:createInternalToolbar {insp {parent ""}} {
    if {$parent==""} {set parent $insp}
    ttk::frame $insp.toolbar -border 2 -relief groove
    place $insp.toolbar -in $parent -relx 1.0 -rely 0 -anchor ne -x -2 -y 2
    return $insp.toolbar
}

#
# Invoked from C++
#
proc inspector:onSetObject {insp} {
    set ptr [opp_inspector_getobject $insp]
    set icon [opp_getobjecticon $ptr]
    if [winfo exist $insp.infobar] {  ;#FIXME add proper condition
        $insp.infobar.icon config -image $icon
    }
    if {$insp==".network"} {
        mainWindow:networkViewInputChanged $ptr
    }
}

#
# Invoked from C++
#
proc inspector:refresh {insp} {
    global config help_tips

    set ptr [opp_inspector_getobject $insp]

    # Info bar
    if [winfo exist $insp.infobar] {  ;#FIXME add proper condition
        if [opp_isnull $ptr] {
            $insp.infobar.name config -text "N/A"
        } else {
            set typename [opp_getobjectshorttypename $ptr]
            set fullpath [opp_getobjectfullpath $ptr]
            set str "($typename) $fullpath"
            $insp.infobar.name config -text $str
        }
    }

    if [winfo exist $insp.toolbar.parent] {  ;#FIXME add proper condition
        if [opp_isnull $ptr] {
            toolbutton:setstate $insp.toolbar.parent disabled
        } else {
            set parentptr [opp_getobjectparent $ptr]
            if [opp_isnull $parentptr] {set state disabled} else {set state normal}
            toolbutton:setstate $insp.toolbar.parent $state
        }
    }

    if [opp_inspectorcommand $insp cangoback] {set state normal} else {set state disabled}
    catch {toolbutton:setstate $insp.toolbar.back $state} ;#FIXME add proper condition whether button exists

    if [opp_inspectorcommand $insp cangoforward] {set state normal} else {set state disabled}
    catch {toolbutton:setstate $insp.toolbar.forward $state}  ;#FIXME add proper condition whether button exists

}

#
# Invoked by the WM (Window Manager)
#
proc inspector:close {insp} {
    # invokes app->deleteInspector(insp)
    opp_deleteinspector $insp
}

#
# Called from C++
#
proc inspector:destroy {insp} {
    destroy $insp
}

#
# Called from C++
#
proc inspector:show {insp} {
    showWindow $insp
}

proc inspector:back {insp} {
    opp_inspectorcommand $insp goback
}

proc inspector:forward {insp} {
    opp_inspectorcommand $insp goforward
}

proc inspector:bindSideButtons {insp} {
    bindRec $insp <Button> "inspector:handleSideButtons $insp %b"
}

proc inspector:handleSideButtons {insp button} {
    global BBack BFwd

    if {$button==$BBack} {
        inspector:back $insp
    } elseif {$button==$BFwd} {
        inspector:forward $insp
    }
}

proc inspector:inspectParent {insp} {
    global config

    set ptr [opp_inspector_getobject $insp]
    set parentptr [opp_getobjectparent $ptr]
    if [opp_isnull $parentptr] {
        return
    }

    # inspect in current inspector if possible (and allowed), otherwise open a new one
    if {[opp_inspector_supportsobject $insp $parentptr] && $config(reuse-inspectors)} {
        opp_inspector_setobject $insp $parentptr
    } else {
        opp_inspect $parentptr
    }
}

proc inspector:dblClick {insp ptr} {
    global config

    # inspect in current inspector if possible (and allowed), otherwise open a new one
    if {[opp_inspector_supportsobject $insp $ptr] && $config(reuse-inspectors)} {
        opp_inspector_setobject $insp $ptr
    } else {
        opp_inspect $ptr
    }
}

proc inspector:namePopup {insp toolbutton} {
    set ptr [opp_inspector_getobject $insp]
    if [opp_isnull $ptr] {return}

    catch {destroy .popup}
    menu .popup -tearoff 0
    .popup add command -label "Copy Pointer With Cast (for Debugger)" -command [list copyToClipboard $ptr ptrWithCast]
    .popup add command -label "Copy Pointer Value (for Debugger)" -command [list copyToClipboard $ptr ptr]
    .popup add separator
    .popup add command -label "Copy Full Path" -command [list copyToClipboard $ptr fullPath]
    .popup add command -label "Copy Name" -command [list copyToClipboard $ptr fullName]
    .popup add command -label "Copy Class Name" -command [list copyToClipboard $ptr className]

    set x [winfo rootx $toolbutton]
    set y [expr [winfo rooty $toolbutton]+[winfo height $toolbutton]]
    tk_popup .popup $x $y
}

proc inspector:inspectAsPopup {insp toolbutton} {
    set ptr [opp_inspector_getobject $insp]
    if [opp_isnull $ptr] {return}

    set curtype [opp_inspectortype [opp_inspector_gettype $insp]]
    set typelist [opp_supported_insp_types $ptr]

    catch {destroy .popup}
    menu .popup -tearoff 0
    foreach type $typelist {
        if {$type==$curtype} {set state disabled} else {set state normal}
       set label [getInspectMenuLabel $type]
        .popup add command -label $label -command [list opp_inspect $ptr $type] -state $state
    }

    set x [winfo rootx $toolbutton]
    set y [expr [winfo rooty $toolbutton]+[winfo height $toolbutton]]
    tk_popup .popup $x $y
}

