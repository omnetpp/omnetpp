#=================================================================
#  LOGINSPECTOR.TCL - part of
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


proc createSimpleModuleWindow {name geom} {
    _createModuleWindow $name $geom 0
}

proc createCompoundModuleWindow {name geom} {
    _createModuleWindow $name $geom 1
}

proc _createModuleWindow {name geom iscompound} {
    global icons fonts help_tips B2 B3

    set w $name
    createInspectorToplevel $w $geom

    set modptr [opp_inspector_getobject $w]

    # Add icons
    if {$iscompound} {
        # for compound module
        packIconButton $w.toolbar.graph  -image $icons(asgraphics) -command "inspectThis $w {As Graphics}"
        packIconButton $w.toolbar.obj    -image $icons(asobject) -command "inspectThis $w {As Object}"
        packIconButton $w.toolbar.sep1   -separator
        set help_tips($w.toolbar.owner)  {Inspect parent module}
        set help_tips($w.toolbar.graph)  {Inspect as network graphics}
        set help_tips($w.toolbar.obj)    {Inspect as object}

        textWindowAddIcons $w modulewindow

        moduleInspector:addRunButtons $w

    } else {
        # for simple module
        packIconButton $w.toolbar.obj    -image $icons(asobject) -command "inspectThis $w {As Object}"
        packIconButton $w.toolbar.sep1   -separator
        set help_tips($w.toolbar.owner)  {Inspect parent module}
        set help_tips($w.toolbar.obj)    {Inspect as object}

        textWindowAddIcons $w modulewindow

        moduleInspector:addRunButtons $w
    }

    frame $w.main
    pack $w.main -expand 1 -fill both -side top
    createModuleLogViewer $w.main
}

proc createModuleLogViewer {w} {
    global fonts
    text $w.text -yscrollcommand "$w.sb set" -width 80 -height 15 -font $fonts(text)
    scrollbar $w.sb -command "$w.text yview"
    logTextWidget:configureTags $w.text

    pack $w.sb -anchor center -expand 0 -fill y -side right
    pack $w.text -anchor center -expand 1 -fill both -side left

    # bindings for find
    bindCommandsToTextWidget $w.text modulewindow
}

proc logTextWidget:configureTags {txt} {
    $txt tag configure SELECT -back #808080 -fore #ffffff
    $txt tag configure event -foreground blue
    $txt tag configure log -foreground #006000
}

proc logTextWidget:clear {txt} {
    # Note: the direct way ($txt delete 0.1 end) is very slow if there's
    # a lot of lines (100,000). Luckily, removing all tags beforehand
    # speeds up the whole process about a hundred times. We need to
    # re-define the tags afterwards though.

    $txt tag delete log event prefix
    $txt delete 0.1 end
    logTextWidget:configureTags $txt
}

proc mainlogWindow:openFilterDialog {} {
    set modptr [opp_object_systemmodule]
    set excludedModuleIds [opp_getmainwindowexcludedmoduleids]
    set excludedModuleIds [moduleOutputFilterDialog $modptr $excludedModuleIds]
    if {$excludedModuleIds!="0"} {
        opp_setmainwindowexcludedmoduleids $excludedModuleIds
    }
}

proc moduleWindow:openFilterDialog {w} {
    set modptr [opp_inspector_getobject $w]
    set excludedModuleIds [opp_inspectorcommand $w getexcludedmoduleids]
    set excludedModuleIds [moduleOutputFilterDialog $modptr $excludedModuleIds]
    if {$excludedModuleIds!="0"} {
        opp_inspectorcommand $w setexcludedmoduleids $excludedModuleIds
    }
}

proc mainlogWindow:trimlines {} {
    global config
    textwidget:trimLines .log.text $config(logwindow-scrollbacklines)
}

proc moduleWindow:trimlines {w} {
    global config
    textwidget:trimLines $w.main.text $config(logwindow-scrollbacklines)
}

proc textwidget:trimLines {t numlines} {
    if {$numlines==""} {return}
    set endline [$t index {end linestart}]
    if {$endline > $numlines + 100} {  ;# for performance, we want to delete in at least 100-line chunks
        set linestodelete [expr int($endline-$numlines)]
        $t delete 1.0 $linestodelete.0
    }
}


