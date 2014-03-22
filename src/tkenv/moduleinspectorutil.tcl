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
    global icons help_tips Control CTRL_

    packIconButton $insp.toolbar.mrun    -image $icons(mrun)    -command "runSimulationLocal $insp normal"
    packIconButton $insp.toolbar.mfast   -image $icons(mfast)   -command "runSimulationLocal $insp fast"
    packIconButton $insp.toolbar.stop    -image $icons(stop)    -command {stopSimulation}

    bind $insp <$Control-F4> "runSimulationLocal $insp fast"

    set help_tips($insp.toolbar.mrun)    "Run until next event in this module"
    set help_tips($insp.toolbar.mfast)   "Fast run until next event in this module (${CTRL_}F4)"
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

proc runSimulationLocal {insp mode {ptr ""}} {
    # invoked from toolbar in module inspectors
    if [isRunning] {
        setGuiForRunmode $mode $insp
        opp_set_run_mode $mode
        opp_set_run_until_module $insp
    } else {
        if {![networkReady]} {return}
        setGuiForRunmode $mode $insp
        if {$ptr==""} {set ptr [opp_inspector_getobject $insp]}
        opp_onestepinmodule $ptr $mode
        setGuiForRunmode notrunning
    }
}
