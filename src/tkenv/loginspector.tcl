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
    createLogViewer $w $w.main
}

proc createEmbeddedLogInspector {w} {
    frame $w.main
    pack $w.main -expand 1 -fill both -side top
    createLogViewer $w $w.main
}

proc createLogViewer {w f} {
    global config fonts B3

    text $f.text -yscrollcommand "$f.sb set" -width 80 -height 15 -font $fonts(text)
    ttk::scrollbar $f.sb -command "$f.text yview"
    logTextWidget:configureTags $f.text

    pack $f.sb -anchor center -expand 0 -fill y -side right
    pack $f.text -anchor center -expand 1 -fill both -side left

    # bindings for find
    bindCommandsToTextWidget $f.text

    # bind Ctrl+H ('break' is needed because originally ^H is bound to DEL)
    set ww [winfo toplevel $f]
    bind $ww <Control-h> "puts $w; LogInspector:openFilterDialog $w; break"
    bind $ww <Control-H> "puts $w; LogInspector:openFilterDialog $w; break"

    # bind a context menu as well
    catch {$f.text config -wrap $config(editor-wrap)}
    bind $f.text <Button-$B3> [list textwidget:contextMenu %W $w %X %Y]

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


