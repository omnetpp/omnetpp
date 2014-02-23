#=================================================================
#  OBJINSP.TCL - part of
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


proc createGenericObjectInspector {name geom wantcontentspage focuscontentspage} {
    global icons help_tips

    set w $name
    createInspectorToplevel $w $geom

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
        moduleInspector:addRunButtons $w
    } else {
        set insptypes [opp_supported_insp_types $object]
        if {[lsearch -exact $insptypes "As Graphics"]!=-1} {
            packIconButton $w.toolbar.graph  -image $icons(asgraphics) -command "inspectThis $w {As Graphics}"
            set help_tips($w.toolbar.graph)  {Inspect graphically}
        }
    }

    createGenericObjectViewer $w $wantcontentspage $focuscontentspage
}

proc createGenericObjectViewer {w wantcontentspage focuscontentspage} {
    set nb [inspector:createNotebook $w]

    inspector:createFields2Page $w

    if {$wantcontentspage} {
        notebook:addPage $nb contents {Contents}
        createInspectorListbox $nb.contents
        if {$focuscontentspage} {
            notebook:showPage $nb contents
        } else {
            notebook:showPage $nb fields2
        }
    }
}

proc createWatchInspector {name geom} {
    global fonts

    set w $name
    createInspectorToplevel $w $geom

    frame $w.main
    pack $w.main -expand 0 -fill both -side top

    createWatchViewer $w.main
}

proc createWatchViewer {w} {
    label-entry $w.name ""
    $w.name.l config -width 20
    focus $w.name.e
    pack $w.name -fill x -side top
}


