#=================================================================
#  MODINSP1.TCL - part of
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

#
#  non-graphical module inspectors etc.
#

proc moduleInspector:addRunButtons {w} {
    global icons help_tips

    packIconButton $w.toolbar.minfo   -image $icons(info) -command "modelInfoDialog $w"
    packIconButton $w.toolbar.type    -image $icons(modtype) -command "inspectComponentType $w"
    #packIconButton $w.toolbar.sep15   -separator
    packIconButton $w.toolbar.objs    -image $icons(findobj) -command "inspectFilteredObjectList $w"
    packIconButton $w.toolbar.sep11   -separator

    packIconButton $w.toolbar.mrun    -image $icons(mrun)    -command "runSimulationLocal $w normal"
    packIconButton $w.toolbar.mfast   -image $icons(mfast)   -command "runSimulationLocal $w fast"
    #packIconButton $w.toolbar.sep13   -separator

    packIconButton $w.toolbar.vrun     -image $icons(run)     -command {runNormal}
    packIconButton $w.toolbar.vruncfg  -image $icons(down_vs) -command "moduleInspector:setRunmode $w.toolbar.vrun"

    #packIconButton $w.toolbar.fastrun -image $icons(fast)    -command {runFast}
    #packIconButton $w.toolbar.exprrun -image $icons(express) -command {runExpress}
    #packIconButton $w.toolbar.sep12   -separator
    #packIconButton $w.toolbar.until   -image $icons(until)   -command {runUntil}
    #packIconButton $w.toolbar.sep14   -separator

    packIconButton $w.toolbar.stop    -image $icons(stop)    -command {stopSimulation}
    packIconButton $w.toolbar.sep16   -separator

    bind $w <Control-F4> "runSimulationLocal $w fast"

    set help_tips($w.toolbar.type)    {Inspect component type}
    set help_tips(.toolbar.minfo)     {Model information}
    set help_tips($w.toolbar.objs)    {Find and inspect messages, queues, watched variables, statistics, etc (Ctrl+S)}
    set help_tips($w.toolbar.mrun)    {Run until next event in this module}
    set help_tips($w.toolbar.mfast)   {Fast run until next event in this module (Ctrl+F4)}
    set help_tips($w.toolbar.vrun)    {Run with full animation (F5)}
    set help_tips($w.toolbar.stop)    {Stop the simulation (F8)}
}

#
# Invoked by the small down arrow next to the "Run" icon on the toolbar, and displays
# a menu to select between Run, Fast, Express and Until. $w is the icon button to configure.
#
proc moduleInspector:setRunmode {w} {
    global icons help_tips

    catch {destroy .popup}
    menu .popup -tearoff 0

    .popup add command -label "Run" \
        -command [list iconButton:configure $w $icons(run)  runNormal "Run with full animation (F5)"]
    .popup add command -label "Fast Run" \
        -command [list iconButton:configure $w $icons(fast) runFast "Run faster: no animation and rare inspector updates (F6)"]
    .popup add command -label "Express Run" \
        -command [list iconButton:configure $w $icons(express) runExpress "Run at full speed: no text output, animation or inspector updates (F7)"]
    .popup add command -label "Until..."  \
        -command [list iconButton:configure $w $icons(until) runUntil "Run until time or event number"]

    tk_popup .popup [winfo rootx $w] [expr [winfo rooty $w]+[winfo height $w]]

}

proc runSimulationLocal {w mode} {
    # invoked from toolbar in module inspectors
    if [isRunning] {
        setGuiForRunmode $mode $w
        opp_set_run_mode $mode
        opp_set_run_until_module $w
    } else {
        if {![networkReady]} {return}
        setGuiForRunmode $mode $w
        opp_onestepinmodule $w $mode
        setGuiForRunmode notrunning
    }
}

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
    text $w.main.text -yscrollcommand "$w.main.sb set" -width 80 -height 15 -font $fonts(text)
    scrollbar $w.main.sb -command "$w.main.text yview"
    logTextWidget:configureTags $w.main.text

    #pack $w.statusbar -anchor center -expand 0 -fill x -side top
    #pack $w.statusbar.name -anchor n -expand 1 -fill x -side left
    #pack $w.statusbar.phase   -anchor n -expand 1 -fill x -side right
    pack $w.main -anchor center -expand 1 -fill both -side top
    pack $w.main.sb -anchor center -expand 0 -fill y -side right
    pack $w.main.text -anchor center -expand 1 -fill both -side left

    # bindings for find
    bindCommandsToTextWidget $w.main.text modulewindow
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
    textwidget:trimLines .main.text $config(logwindow-scrollbacklines)
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


