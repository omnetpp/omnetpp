#=================================================================
#  MODULEINSPECTORUTIL.TCL - part of
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


proc ModuleInspector:addRunButtons {w} {
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
    packIconButton $w.toolbar.vruncfg  -image $icons(down_vs) -command "ModuleInspector:setRunmode $w.toolbar.vrun"

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
proc ModuleInspector:setRunmode {w} {
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
