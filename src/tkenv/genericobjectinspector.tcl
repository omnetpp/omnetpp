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

    #FIXME TODO: (1) rebuild toolbar  (2) enable/disable "contents" page as needed

    set object [opp_inspector_getobject $w]
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
        ModuleInspector:addRunButtons $w
    } else {
        set insptypes [opp_supported_insp_types $object]
        if {[lsearch -exact $insptypes "As Graphics"]!=-1} {
            packIconButton $w.toolbar.graph  -image $icons(asgraphics) -command "inspectThis $w {As Graphics}"
            set help_tips($w.toolbar.graph)  {Inspect graphically}
        }
    }

    notebook:setPageEnabled $w.nb contents 0  ;# disable --- FIXME do it depending on the object type
}


