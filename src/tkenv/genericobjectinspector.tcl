#=================================================================
#  GENERICOBJECTINSPECTOR.TCL - part of
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


proc createGenericObjectInspector {w geom} {
    createInspectorToplevel $w $geom
    createGenericObjectViewer $w
}

proc createEmbeddedGenericObjectInspector {w} {
    global icons

    # Create info bar
    frame $w.infobar  -borderwidth 0
    label $w.infobar.icon -anchor w -relief flat -image $icons(none_vs)
    label $w.infobar.name -anchor w -relief flat -justify left
    pack $w.infobar.icon -anchor n -side left -expand 0 -fill y -pady 1
    pack $w.infobar.name -anchor n -side left -expand 1 -fill both -pady 1
    pack $w.infobar -anchor w -side top -fill x -expand 0

    createGenericObjectViewer $w
}

proc createGenericObjectViewer {w} {
    set nb $w.nb
    ttk::notebook $nb -width 460 -height 260
    pack $nb -expand 1 -fill both

    $nb add [frame $nb.fields] -text "Fields"
    createFieldsPage $nb.fields $w

    $nb add [frame $nb.contents] -text "Contents"
    createInspectorListbox $nb.contents $w
}

proc GenericObjectInspector:onSetObject {w} {
    global icons help_tips

    set object [opp_inspector_getobject $w]
    if [opp_isnull $object] return  ;# leave inspector as it is
    set type [opp_getobjectbaseclass $object]

    set showContentsPage  [lcontains {cArray cQueue cMessageHeap cSimpleModule cCompoundModule cChannel cRegistrationList cSimulation } $type]
    set focusContentsPage [lcontains {cArray cQueue cMessageHeap cSimpleModule cCompoundModule cChannel cRegistrationList} $type]

    if {$showContentsPage} {
        $w.nb tab $w.nb.contents -state normal
    } else {
        $w.nb tab $w.nb.contents -state disabled
    }

    if {$focusContentsPage} {
        $w.nb select $w.nb.contents
    } else {
        $w.nb select $w.nb.fields
    }
}

proc lcontains {list item} {
    set i [lsearch -exact $list $item]
    return [expr $i != -1]
}
