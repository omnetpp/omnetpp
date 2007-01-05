global cpanel_toplevel
set cpanel_toplevel ".cpanel"

#
# The registerPlugin function below arranges certain plugin functions to be
# called on different Tkenv events. As of now, the only such functions are
# $pluginname:newNetwork, called after a network got set up, and
# $pluginname:inspectorUpdate, called when inspectors should be updated.
#
registerPlugin "cpanel"

proc cpanel:newNetwork {} {
    global param
}

proc cpanel:inspectorUpdate {} {
    #global counter
    #if {![info exist counter]} {set counter 0}
    #incr counter
    #puts $counter
}

#
# Set up dialog
#
proc cpanel_createControls {} {
    global param priv cpanel_toplevel

    toplevel $cpanel_toplevel
    wm title $cpanel_toplevel "Simulation parameters"
    wm focusmodel $cpanel_toplevel passive
    wm overrideredirect $cpanel_toplevel 0
    wm resizable $cpanel_toplevel 1 1

    # next line ensures this window cannot be closed
    wm protocol $cpanel_toplevel WM_DELETE_WINDOW {exit_omnetpp}

    # create sliders
    cpanel_addSlider debugWaitTime "Debug wait time" 0 0 1000 1 "dwt"
    cpanel_addSlider maxCalculationTime "Max calculation time" 10000 10 10000 10 "mct"
    cpanel_addSlider minTimeStep "Min time step" 0.01 0.001 0.1 0.001 "mits"
    cpanel_addSlider maxTimeStep "Max time step" 10 1 100 1 "mats"
    cpanel_addSlider minAccelerationError "Min acceleration error" 5 0.1 500 0.1 "miae"
    cpanel_addSlider maxAccelerationError "Max acceleration error" 10 0.1 1000 0.1 "maae"
    cpanel_addSlider minFrictionCoefficient "Min friction coefficient" 0.5 0.005 5 0.005 "mifc"
    cpanel_addSlider maxFrictionCoefficient "Max friction coefficient" 10 0.1 100 0.1 "mafc"
    cpanel_addSlider velocityRelaxLimit "Velocity relax limit" 0.5 0.005 5 0.005 "vrl"
    cpanel_addSlider springReposeLength "Spring repose length" 50 0 500 1 "srl"
    cpanel_addSlider springCoefficient "Spring coefficient" 0.1 0.001 1 0.001 "sc"
    cpanel_addSlider electricRepulsionCoefficient "Electrical repulsion coefficient" 10000 100 100000 100 "erc"

    cpanel_addCheckButton starTreeEmbedding "Star tree embedding" 1 "ste"
    cpanel_addCheckButton forceDirectedEmbedding "Force directed embedding" 1 "fde"
    cpanel_addCheckButton 3d "3d" 1 "3d"
}

#
# Set up a slider
#
proc cpanel_addSlider {variable label value min max step tag} {
    global cpanel_toplevel param
 
    frame $cpanel_toplevel.$variable
    label $cpanel_toplevel.$variable.l -anchor w -width 16 -text $label
    scale $cpanel_toplevel.$variable.e -orient horizontal -length 200
    pack $cpanel_toplevel.$variable.l -anchor center -expand 0 -fill none -padx 2 -pady 2 -side left
    pack $cpanel_toplevel.$variable.e -anchor center -expand 1 -fill x -padx 2 -pady 2 -side right

    $cpanel_toplevel.$variable.e config -from $min -to $max -resolution $step -variable param($variable)
    pack $cpanel_toplevel.$variable -expand 0 -fill x

    set param($variable) $value
    trace variable param($variable) w "cpanel_paramChanged $tag"
}

#
# Set up a check button
#
proc cpanel_addCheckButton {variable label value tag} {
    global cpanel_toplevel param

    checkbutton $cpanel_toplevel.$variable -text $label -variable param($variable)
    pack $cpanel_toplevel.$variable -expand 0 -fill none -anchor w

    set param($variable) $value
    trace variable param($variable) w "cpanel_paramChanged $tag"
}

#
# Update parameter values when sliders change
#
proc cpanel_paramChanged {tag arr name op} {
    if {$op!="write" && $op!="w"} return

    if [catch {
        global param
        set value $param($name)
        set modp [opp_object_systemmodule]
        #debug "$name changed to $value"

        opp_set_moduledisplaystring_tagarg $modp $tag 0 $value
    } msg] {
        puts "Error in cpanel plugin's callback: $msg"
    }
}

#
# Invoke initalization code
#
cpanel_createControls
