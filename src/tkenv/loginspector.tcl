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


proc createLogInspector {insp geom} {
    global icons fonts help_tips B2 B3

    createInspectorToplevel $insp $geom
    set help_tips($insp.toolbar.parent)  "Go to parent module"

    packToolbutton $insp.toolbar.sep1 -separator
    textWindowAddIcons $insp modulewindow
    ModuleInspector:addRunButtons $insp

    ttk::frame $insp.main
    pack $insp.main -expand 1 -fill both -side top
    createLogViewer $insp $insp.main
}

proc createEmbeddedLogInspector {insp} {
    global icons help_tips CTRL_

    ttk::frame $insp.main
    pack $insp.main -expand 1 -fill both -side top

    createLogViewer $insp $insp.main

    set tb [inspector:createInternalToolbar $insp $insp.main.text]
    packToolbutton $tb.copy   -image $icons(copy) -command "editCopy $insp.main.text"
    packToolbutton $tb.find   -image $icons(find) -command "findDialog $insp.main.text"
    packToolbutton $tb.filter -image $icons(filter) -command "editFilterWindowContents $insp"

    set help_tips($tb.copy)   "Copy selected text to clipboard (${CTRL_}C)"
    set help_tips($tb.find)   "Find string in window (${CTRL_}F)"
    set help_tips($tb.filter) "Filter window contents (${CTRL_}H)"
}

proc createLogViewer {insp f} {
    global config fonts B3 Control

    text $f.text -yscrollcommand "$f.sb set" -width 80 -height 15 -font $fonts(text)
    ttk::scrollbar $f.sb -command "$f.text yview"
    logTextWidget:configureTags $f.text

    pack $f.sb -anchor center -expand 0 -fill y -side right
    pack $f.text -anchor center -expand 1 -fill both -side left

    # bindings for find
    bindCommandsToTextWidget $f.text

    # bind Ctrl+H to the whole (main or inspector) window
    # ('break' is needed because originally ^H is bound to DEL)
    set w [winfo toplevel $f]
    bind $w <$Control-h> "LogInspector:openFilterDialog $insp; break"
    bind $w <$Control-H> "LogInspector:openFilterDialog $insp; break"

    # bind a context menu as well
    catch {$f.text config -wrap $config(editor-wrap)}
    bind $f.text <Button-$B3> [list textwidget:contextMenu %W $insp %X %Y]
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

proc LogInspector:openFilterDialog {insp} {
    set modptr [opp_inspector_getobject $insp]
    set excludedModuleIds [opp_inspectorcommand $insp getexcludedmoduleids]
    set excludedModuleIds [moduleOutputFilterDialog $modptr $excludedModuleIds]
    if {$excludedModuleIds!="0"} {
        opp_inspectorcommand $insp setexcludedmoduleids $excludedModuleIds
        opp_inspectorcommand $insp redisplay
    }
}

proc LogInspector:trimlines {insp} {
    global config
    textwidget:trimLines $insp.main.text $config(logwindow-scrollbacklines)
}


