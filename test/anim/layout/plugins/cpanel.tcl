#
# The registerPlugin function below arranges certain plugin functions to be
# called on different Tkenv events. As of now, the only such functions are
# $pluginname:newNetwork, called after a network got set up, and
# $pluginname:inspectorUpdate, called when inspectors should be updated.
#
registerPlugin "cpanel"

proc cpanel:newNetwork {} {
    cpanel_readParams
}

proc cpanel:inspectorUpdate {} {
    #global counter
    #if {![info exist counter]} {set counter 0}
    #incr counter
    #puts $counter
}

#
# Utility function to create a slider
#
proc cpanel_slider {w label} {
    frame $w
    label $w.l -anchor w -width 16 -text $label
    scale $w.e -orient horizontal -length 200
    pack $w.l -anchor center -expand 0 -fill none -padx 2 -pady 2 -side left
    pack $w.e -anchor center -expand 1 -fill x -padx 2 -pady 2 -side right
}

#
# Set up dialog
#
proc cpanel_createControls {} {
    global param priv

    set w ".cpanel"
    toplevel $w
    wm title $w "Simulation parameters"
    wm focusmodel $w passive
    wm overrideredirect $w 0
    wm resizable $w 1 1

    # next line ensures this window cannot be closed
    wm protocol $w WM_DELETE_WINDOW {exit_omnetpp}

    # create sliders
    cpanel_slider $w.maxCalculationTime "Max Calculation Time"
    $w.maxCalculationTime.e config -from 100 -to 100000 -resolution 100 -variable param(maxCalculationTime)
    pack $w.maxCalculationTime -expand 0 -fill x

    cpanel_slider $w.springCoefficient "Spring Coefficient"
    $w.springCoefficient.e config -from 0 -to 10 -resolution 0.01 -variable param(springCoefficient)
    pack $w.springCoefficient -expand 0 -fill x

    cpanel_slider $w.electricRepealCoefficient "Electrical Repeal Coefficient"
    $w.electricRepealCoefficient.e config -from 1000 -to 500000 -resolution 1000 -variable param(electricRepealCoefficient)
    pack $w.electricRepealCoefficient -expand 0 -fill x

    cpanel_slider $w.velocityRelaxLimit "Velocity relax limit"
    $w.velocityRelaxLimit.e config -from 0.01 -to 2 -resolution 0.01 -variable param(velocityRelaxLimit)
    pack $w.velocityRelaxLimit -expand 0 -fill x

    checkbutton $w.starTreeEmbedding -text "star tree embedding" -variable param(starTreeEmbedding)
    pack $w.starTreeEmbedding -expand 0 -fill none -anchor w

    checkbutton $w.forceDirectedEmbedding -text "force directed embedding" -variable param(forceDirectedEmbedding)
    pack $w.forceDirectedEmbedding -expand 0 -fill none -anchor w

    checkbutton $w.3d -text "3d" -variable param(3d)
    pack $w.3d -expand 0 -fill none -anchor w

    # this is the old syntax, still accepted by Tcl8.4
    trace variable param(springCoefficient) w cpanel_paramChanged
    trace variable param(electricRepealCoefficient) w cpanel_paramChanged
    trace variable param(velocityRelaxLimit) w cpanel_paramChanged
    trace variable param(starTreeEmbedding) w cpanel_paramChanged
    trace variable param(forceDirectedEmbedding) w cpanel_paramChanged
    trace variable param(3d) w cpanel_paramChanged
}

#
# Read initial parameter values from model
#
proc cpanel_readParams {} {
    global param

    catch {
        set modp [opp_object_systemmodule]
        set dispstr [opp_getobjectfield $modp displayString]

        set param(maxCalculationTime) 10000
        set param(springCoefficient) 0.1
        set param(electricRepealCoefficient) 10000
        set param(velocityRelaxLimit) 0.5
        set param(starTreeEmbedding) 1
        set param(forceDirectedEmbedding) 1
        set param(3d) 1

    }
}

#
# Update parameter values when sliders change
#
proc cpanel_paramChanged {arr name op} {
    if {$op!="write" && $op!="w"} return

    if [catch {
        global param
        set value $param($name)
        set modp [opp_object_systemmodule]
        #debug "$name changed to $value"

        if {$name=="maxCalculationTime"} {
            opp_set_moduledisplaystring_tagarg $modp "mct" 0 $value
        } elseif {$name=="springCoefficient"} {
            opp_set_moduledisplaystring_tagarg $modp "sc" 0 $value
        } elseif {$name=="electricRepealCoefficient"} {
            opp_set_moduledisplaystring_tagarg $modp "erc" 0 $value
        } elseif {$name=="velocityRelaxLimit"} {
            opp_set_moduledisplaystring_tagarg $modp "vrl" 0 $value
        } elseif {$name=="starTreeEmbedding"} {
            opp_set_moduledisplaystring_tagarg $modp "ste" 0 $value
        } elseif {$name=="forceDirectedEmbedding"} {
            opp_set_moduledisplaystring_tagarg $modp "fde" 0 $value
        } elseif {$name=="3d"} {
            opp_set_moduledisplaystring_tagarg $modp "3d" 0 $value
        } else {
            error "wrong param name"
        }
    } msg] {
        puts "Error in cpanel plugin's callback: $msg"
    }
}

#
# Invoke initalization code
#
cpanel_createControls

