#==========================================================================
# INSPECTOR.TCL -
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


#===================================================================
#    INSPECTOR TOPLEVEL WINDOWS
#===================================================================

#
# gets called from concrete inspector subtypes
#
proc create_inspector_toplevel {w geom} {

    # create toplevel inspector window

    global fonts icons help_tips

    toplevel $w -class Toplevel
    wm focusmodel $w passive
    if {$geom != ""} {wm geometry $w $geom}
    wm maxsize $w 1009 738
    wm minsize $w 1 1
    wm overrideredirect $w 0
    wm resizable $w 1 1
    wm protocol $w WM_DELETE_WINDOW "close_inspector_toplevel $w"

    inspectorlist_remove $w
    inspectorlist_storename $w

    # add the "Inspect As.." icon at the top
    frame $w.toolbar -relief raised -borderwidth 1
    pack $w.toolbar -anchor w -side top -fill x -expand 0

    iconbutton $w.toolbar.sep0 -separator
    iconbutton $w.toolbar.inspect -image $icons(as) -command "inspect_this_as $w"
    foreach i {sep0 inspect} {
       pack $w.toolbar.$i -anchor n -side left -padx 0 -pady 2
    }

    set help_tips($w.toolbar.inspect) {Open another inspector for this object}

    # Keyboard bindings
    bind $w <Escape>     "catch {.popup unpost}"
    bind $w <Button-1>   "catch {.popup unpost}"
    bind $w <Key-Return> "opp_writebackinspector $w; opp_updateinspectors"

    bind_runcommands $w
}

#
# gets called by WM
#
proc close_inspector_toplevel {w} {
    opp_deleteinspector $w
}


#
# gets called from C++
#
proc inspector_hostobjectdeleted {w} {
    # if the insp window is destroyed because the object no longer exists,
    # prepare to reopen it with same geometry if the object reappears
    inspectorlist_add $w
}

#
# gets called from C++
#
proc destroy_inspector_toplevel {w} {
    destroy $w
}


#===================================================================
#    UTILITY FUNCTIONS FOR INSPECTOR WINDOWS
#===================================================================

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
        set type [$w.f.type.e cget -value]
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
#    STRUCT (FIELDS) PANEL
#===================================================================

proc create_structpanel {w} {
    # FIXME: a text is a temporary solution... should be sth like a property sheet
    scrollbar $w.sb -borderwidth 1 -command "$w.txt yview"
    text $w.txt  -height 12 -width 40 -yscrollcommand "$w.sb set"

    pack $w.sb -anchor center -expand 0 -fill y -side right
    pack $w.txt -anchor center -expand 1 -fill both -side left

#      text $w.txt -height 12 -width 40
#      pack $w.txt -expand 1 -fill both -side top
}


