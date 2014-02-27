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


proc createGenericObjectInspector {name geom} {
    set w $name
    createInspectorToplevel $w $geom
    createGenericObjectViewer $w
}

proc createGenericObjectViewer {w} {
    set nb [inspector:createNotebook $w]

    inspector:createFields2Page $w

    $nb add [frame $nb.contents] -text "Contents"
    createInspectorListbox $nb.contents
}

proc GenericObjectInspector:onSetObject {w} {
    global icons help_tips

    set object [opp_inspector_getobject $w]
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
