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


proc createLogInspector {w geom} {
    global icons fonts help_tips B2 B3

    createInspectorToplevel $w $geom
    set help_tips($w.toolbar.parent)  {Inspect parent}

    packIconButton $w.toolbar.sep1 -separator
    textWindowAddIcons $w modulewindow
    ModuleInspector:addRunButtons $w

    frame $w.main
    pack $w.main -expand 1 -fill both -side top
    createLogViewer $w.main
}

proc createEmbeddedLogInspector {w} {
    frame $w.main
    pack $w.main -expand 1 -fill both -side top
    createLogViewer $w.main
}

proc createLogViewer {w} {
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

proc LogInspector:openFilterDialog {w} {
    set modptr [opp_inspector_getobject $w]
    set excludedModuleIds [opp_inspectorcommand $w getexcludedmoduleids]
    set excludedModuleIds [moduleOutputFilterDialog $modptr $excludedModuleIds]
    if {$excludedModuleIds!="0"} {
        opp_inspectorcommand $w setexcludedmoduleids $excludedModuleIds
        opp_inspectorcommand $w redisplay
    }
}

proc LogInspector:trimlines {w} {
    global config
    textwidget:trimLines $w.main.text $config(logwindow-scrollbacklines)
}


