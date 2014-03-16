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


proc ModuleInspector:addRunButtons {insp} {
    global icons help_tips

    packIconButton $insp.toolbar.minfo   -image $icons(info) -command "modelInfoDialog $insp"
    packIconButton $insp.toolbar.type    -image $icons(modtype) -command "inspectComponentType $insp"
    #packIconButton $insp.toolbar.sep15   -separator
#    packIconButton $insp.toolbar.objs    -image $icons(findobj) -command "inspectFilteredObjectList $insp"
    packIconButton $insp.toolbar.sep11   -separator

    packIconButton $insp.toolbar.mrun    -image $icons(mrun)    -command "runSimulationLocal $insp normal"
    packIconButton $insp.toolbar.mfast   -image $icons(mfast)   -command "runSimulationLocal $insp fast"
    #packIconButton $insp.toolbar.sep13   -separator

    packIconButton $insp.toolbar.vrun     -image $icons(run)     -command {runNormal}
    packIconButton $insp.toolbar.vruncfg  -image $icons(down_vs) -command "ModuleInspector:setRunmode $insp.toolbar.vrun"

    #packIconButton $insp.toolbar.fastrun -image $icons(fast)    -command {runFast}
    #packIconButton $insp.toolbar.exprrun -image $icons(express) -command {runExpress}
    #packIconButton $insp.toolbar.sep12   -separator
    #packIconButton $insp.toolbar.until   -image $icons(until)   -command {runUntil}
    #packIconButton $insp.toolbar.sep14   -separator

    packIconButton $insp.toolbar.stop    -image $icons(stop)    -command {stopSimulation}
    packIconButton $insp.toolbar.sep16   -separator

    bind $insp <Control-F4> "runSimulationLocal $insp fast"

    set help_tips($insp.toolbar.minfo)   "Model information"
    set help_tips($insp.toolbar.type)    "Inspect component type"
#    set help_tips($insp.toolbar.objs)    "Find and inspect messages, queues, watched variables, statistics, etc (Ctrl+S)"
    set help_tips($insp.toolbar.mrun)    "Run until next event in this module"
    set help_tips($insp.toolbar.mfast)   "Fast run until next event in this module (Ctrl+F4)"
    set help_tips($insp.toolbar.vrun)    "Run with full animation (F5)"
    set help_tips($insp.toolbar.stop)    "Stop the simulation (F8)"
}

#
# Invoked by the small down arrow next to the "Run" icon on the toolbar, and displays
# a menu to select between Run, Fast, Express and Until. $insp is the icon button to configure.
#
proc ModuleInspector:setRunmode {insp} {
    global icons help_tips

    catch {destroy .popup}
    menu .popup -tearoff 0

    .popup add command -label "Run" \
        -command [list iconButton:configure $insp $icons(run)  runNormal "Run with full animation (F5)"]
    .popup add command -label "Fast Run" \
        -command [list iconButton:configure $insp $icons(fast) runFast "Run faster: no animation and rare inspector updates (F6)"]
    .popup add command -label "Express Run" \
        -command [list iconButton:configure $insp $icons(express) runExpress "Run at full speed: no text output, animation or inspector updates (F7)"]
    .popup add command -label "Until..."  \
        -command [list iconButton:configure $insp $icons(until) runUntil "Run until time or event number"]

    tk_popup .popup [winfo rootx $insp] [expr [winfo rooty $insp]+[winfo height $insp]]

}

proc runSimulationLocal {insp mode} {
    # invoked from toolbar in module inspectors
    if [isRunning] {
        setGuiForRunmode $mode $insp
        opp_set_run_mode $mode
        opp_set_run_until_module $insp
    } else {
        if {![networkReady]} {return}
        setGuiForRunmode $mode $insp
        opp_onestepinmodule $insp $mode
        setGuiForRunmode notrunning
    }
}
