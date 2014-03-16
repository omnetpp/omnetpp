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
proc createInspectorToplevel {insp geom} {
    global config fonts icons help_tips
    global B2 B3

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

    # Create toolbar
    frame $insp.toolbar -relief raised -bd 1
    pack $insp.toolbar -anchor w -side top -fill x -expand 0

    packIconButton $insp.toolbar.sep0 -separator
    packIconButton $insp.toolbar.back -image $icons(back) -command "inspector:back $insp"
    packIconButton $insp.toolbar.forward -image $icons(forward) -command "inspector:forward $insp"
    packIconButton $insp.toolbar.parent -image $icons(parent) -command "inspector:inspectParent $insp"
    packIconButton $insp.toolbar.sep01 -separator
    packIconButton $insp.toolbar.inspectas -image $icons(inspectas) -command "inspector:inspectAsPopup $insp $insp.toolbar.inspectas"
    packIconButton $insp.toolbar.copyobj -image $icons(copy) -command "inspector:namePopup $insp $insp.toolbar.copyobj"
    packIconButton $insp.toolbar.objs -image $icons(findobj) -command "inspectFilteredObjectList $insp"

    set help_tips($insp.toolbar.parent) "Inspect parent"
    set help_tips($insp.toolbar.inspectas) "Inspect"
    set help_tips($insp.toolbar.copyobj) "Copy name, type or pointer"
    set help_tips($insp.toolbar.objs) "Find objects (Ctrl+S)"

    # Keyboard bindings
    bind $insp <Escape>     "catch {.popup unpost}"
    bind $insp <Button-1>   "catch {.popup unpost}"
    bind $insp <Key-Return> "opp_commitinspector $insp; opp_refreshinspectors"

    bindRunCommands $insp
    bindOtherCommands $insp $insp
}

proc inspector:createInternalToolbar {insp {parent ""}} {
    if {$parent==""} {set parent $insp}
    frame $insp.toolbar -border 2 -relief groove
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
            $insp.infobar.name config -text "n/a"
        } else {
            set typename [opp_getobjectshorttypename $ptr]
            set fullpath [opp_getobjectfullpath $ptr]
            #set info [opp_getobjectinfostring $ptr]
            set str "($typename) $fullpath"   ;#TODO short info?
            $insp.infobar.name config -text $str
        }
    }

    if [winfo exist $insp.toolbar.parent] {  ;#FIXME add proper condition
        if [opp_isnull $ptr] {
            $insp.toolbar.parent config -state disabled
        } else {
            set parentptr [opp_getobjectparent $ptr]
            if [opp_isnull $parentptr] {set state disabled} else {set state normal}
            $insp.toolbar.parent config -state $state

            if {[opp_inspector_supportsobject $insp $parentptr] && $config(reuse-inspectors)} {
                set help_tips($insp.toolbar.parent) "Go up"
            } else {
                set help_tips($insp.toolbar.parent) "Inspect parent"
            }
        }
    }

    if [opp_inspectorcommand $insp cangoback] {set state normal} else {set state disabled}
    catch {$insp.toolbar.back config -state $state} ;#FIXME add proper condition whether button exists

    if [opp_inspectorcommand $insp cangoforward] {set state normal} else {set state disabled}
    catch {$insp.toolbar.forward config -state $state}  ;#FIXME add proper condition whether button exists

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
        opp_inspect $parentptr "(default)"
    }
}

proc inspector:dblClick {insp ptr} {
    global config

    # inspect in current inspector if possible (and allowed), otherwise open a new one
    if {[opp_inspector_supportsobject $insp $ptr] && $config(reuse-inspectors)} {
        opp_inspector_setobject $insp $ptr
    } else {
        opp_inspect $ptr "(default)"
    }
}

proc inspector:namePopup {insp toolbutton} {
    set ptr [opp_inspector_getobject $insp]
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

proc inspector:inspectAsPopup {insp toolbutton} {
    set ptr [opp_inspector_getobject $insp]
    set curtype [opp_inspectortype [opp_inspector_gettype $insp]]
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

