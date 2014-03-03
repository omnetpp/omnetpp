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

    notebook:addPage $nb contents {Contents}
    createInspectorListbox $nb.contents
}

proc GenericObjectInspector:onSetObject {w} {
    global icons help_tips

    #FIXME enable/disable "contents" page as needed
    set object [opp_inspector_getobject $w]
    set type [opp_getobjectbaseclass $object]
    notebook:setPageEnabled $w.nb contents 0  ;# disable --- FIXME do it depending on the object type
}


