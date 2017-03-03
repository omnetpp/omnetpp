#=================================================================
#  GENERICOBJECTINSPECTOR.TCL - part of
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


proc createGenericObjectInspector {insp geom} {
    createInspectorToplevel $insp $geom
    createGenericObjectViewer $insp
}

proc createEmbeddedGenericObjectInspector {insp} {
    global icons help_tips

    # Create info bar
    ttk::frame $insp.infobar  -borderwidth 0
    ttk::label $insp.infobar.icon -anchor w -relief flat -image $icons(none_vs)
    ttk::label $insp.infobar.name -anchor w -relief flat -justify left
    pack $insp.infobar.icon -anchor n -side left -expand 0 -fill y -pady 1
    pack $insp.infobar.name -anchor n -side left -expand 1 -fill both -pady 1
    pack $insp.infobar -anchor w -side top -fill x -expand 0

    createGenericObjectViewer $insp

    set tb [inspector:createInternalToolbar $insp $insp]

    packToolbutton $tb.back    -image $icons(back)    -command "inspector:back $insp"
    packToolbutton $tb.forward -image $icons(forward) -command "inspector:forward $insp"
    packToolbutton $tb.parent  -image $icons(parent)  -command "inspector:inspectParent $insp"

    set help_tips($tb.back)    "Back"
    set help_tips($tb.forward) "Forward"
    set help_tips($tb.parent)  "Go to parent"
}

proc createGenericObjectViewer {insp} {
    set nb $insp.nb
    ttk::notebook $nb -width 460 -height 260
    pack $nb -expand 1 -fill both

    $nb add [ttk::frame $nb.fields] -text "Fields"
    createFieldsPage $nb.fields $insp

    $nb add [ttk::frame $nb.contents] -text "Contents"
    set lb [createInspectorListbox $nb.contents $insp]

    # restore columns widths -- note: for the main embedded inspector this is too soon, as the config hasn't been loaded yet
    inspectorListbox:restoreColumnWidths $lb "inspector:columnwidths"

    inspector:bindSideButtons $insp
}

proc GenericObjectInspector:onSetObject {insp} {
    global icons help_tips

    set object [opp_inspector_getobject $insp]
    if [opp_isnull $object] return  ;# leave inspector as it is
    set type [opp_getobjectbaseclass $object]

    set showContentsPage  [lcontains {cArray cQueue cFutureEventSet cSimpleModule cModule cChannel cRegistrationList cCanvas cFigure cSimulation cMessage} $type]
    set focusContentsPage [lcontains {cArray cQueue cFutureEventSet cSimpleModule cModule cChannel cRegistrationList cCanvas} $type]

    if {$showContentsPage} {
        $insp.nb tab $insp.nb.contents -state normal
    } else {
        $insp.nb tab $insp.nb.contents -state disabled
    }

    if {$focusContentsPage} {
        $insp.nb select $insp.nb.contents
    } else {
        $insp.nb select $insp.nb.fields
    }
}

proc GenericObjectInspector:refresh {insp} {
    fieldsPage:refresh $insp

    set lb $insp.nb.contents.main.list
    ttkTreeview:deleteAll $lb
    set ptr [opp_inspector_getobject $insp]
    if [opp_isnull $ptr] {
        $insp.nb tab $insp.nb.contents -text "Contents (0)"
    } else {
        set count [opp_fillinspectorlistbox $lb $ptr 0]
        $insp.nb tab $insp.nb.contents -text "Contents ($count)"
    }
}


