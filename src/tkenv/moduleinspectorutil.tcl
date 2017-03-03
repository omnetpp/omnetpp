#=================================================================
#  MODULEINSPECTORUTIL.TCL - part of
#
#                     OMNeT++/OMNEST
#            Discrete System Simulation in C++
#
#=================================================================

#----------------------------------------------------------------#
#  Copyright (C) 1992-2017 Andras Varga
#
#  This file is distributed WITHOUT ANY WARRANTY. See the file
#  `license' for details on this and other legal matters.
#----------------------------------------------------------------#


proc ModuleInspector:addRunButtons {insp} {
    global icons help_tips Control CTRL_

    packToolbutton $insp.toolbar.mrun    -image $icons(mrun)    -command [list runSimulationLocal $insp normal]
    packToolbutton $insp.toolbar.mfast   -image $icons(mfast)   -command [list runSimulationLocal $insp fast]
    packToolbutton $insp.toolbar.stop    -image $icons(stop)    -command {stopSimulation}

    bind $insp <$Control-F4> [list runSimulationLocal $insp fast]

    set help_tips($insp.toolbar.mrun)    "Run until next event in this module"
    set help_tips($insp.toolbar.mfast)   "Fast run until next event in this module (${CTRL_}F4)"
    set help_tips($insp.toolbar.stop)    "Stop the simulation (F8)"
}

proc runSimulationLocal {insp mode {ptr ""}} {
    # invoked from toolbar in module inspectors
    if [isRunning] {
        setGuiForRunmode $mode $insp
        opp_set_run_mode $mode
        if {$ptr==""} {set ptr [opp_inspector_getobject $insp]}
        opp_set_run_until_module $ptr
    } else {
        if {![networkReady]} {return}
        setGuiForRunmode $mode $insp
        if {$ptr==""} {set ptr [opp_inspector_getobject $insp]}
        opp_onestepinmodule $ptr $mode
        setGuiForRunmode notrunning
    }
}
