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
    cpanel_slider $w.arrivalRate1 "Source 1 arrival rate"
    $w.arrivalRate1.e config -from 100 -to 1000 -resolution 1 -variable param(arrivalRate1)
    pack $w.arrivalRate1 -expand 0 -fill x

    cpanel_slider $w.arrivalRate2 "Source 2 arrival rate"
    $w.arrivalRate2.e config -from 100 -to 1000 -resolution 1 -variable param(arrivalRate2)
    pack $w.arrivalRate2 -expand 0 -fill x

    cpanel_slider $w.serviceRate1 "Queue 1 service rate"
    $w.serviceRate1.e config -from 100 -to 1000 -resolution 1 -variable param(serviceRate1)
    pack $w.serviceRate1 -expand 0 -fill x

    cpanel_slider $w.serviceRate2 "Queue 2 service rate"
    $w.serviceRate2.e config -from 100 -to 1000 -resolution 1 -variable param(serviceRate2)
    pack $w.serviceRate2 -expand 0 -fill x

    # create play/stop buttons
    set img_play [image create photo -file "plugins/play.gif"]
    set img_stop [image create photo -file "plugins/stop.gif"]
    frame $w.f
    pack $w.f -expand 1 -fill both
    button $w.f.run -image $img_play -command "run_normal" -relief flat
    button $w.f.stop -image $img_stop -command "stop_simulation" -relief flat

    # animation on/off button
    set img_anim [image create photo -file "plugins/anim.gif"]
    set img_noanim [image create photo -file "plugins/noanim.gif"]
    button $w.f.anim -relief flat -image $img_anim -command "toggleAnimation $w.f.anim $img_anim $img_noanim"

    frame $w.f.dummy1
    frame $w.f.dummy2
    pack $w.f.dummy1 -side left -expand 1
    pack $w.f.run $w.f.stop -side left
    pack $w.f.dummy2 -side left -expand 1
    pack $w.f.anim -side left -anchor s

    # create "speed" slider
    cpanel_slider $w.animspeed "Animation speed"
    $w.animspeed.e config -from 0.5 -to 3 -resolution 0.01 -variable priv(animspeed)
    pack $w.animspeed -expand 0 -fill x

    # arrange the paramChanged function to be called whenever sliders change

    # following lines are only understood by Tcl8.4 -- no good for 8.3
    #trace add variable param(arrivalRate1) write cpanel_paramChanged
    #trace add variable param(arrivalRate2) write cpanel_paramChanged
    #trace add variable param(serviceRate1) write cpanel_paramChanged
    #trace add variable param(serviceRate2) write cpanel_paramChanged

    # this is the old syntax, still accepted by Tcl8.4
    trace variable param(arrivalRate1) w cpanel_paramChanged
    trace variable param(arrivalRate2) w cpanel_paramChanged
    trace variable param(serviceRate1) w cpanel_paramChanged
    trace variable param(serviceRate2) w cpanel_paramChanged

    # following line uses undocumented Tkenv internals and will break next time Tkenv changes
    trace variable priv(animspeed) w animSpeedChanged

    # this button brings back main window (if it was withdrawn originally)
    set img_more [image create photo -file "plugins/more.gif"]
    button $w.mainwin -relief flat -image $img_more -command "wm deiconify .; $w.mainwin config -state disabled"
    pack $w.mainwin -side top -anchor e -expand 0 -fill none -padx 2 -pady 6
}

#
# Read initial parameter values from model
#
proc cpanel_readParams {} {
    global param

    catch {
        set modp [opp_modulebypath "fifonet1.gen1"]
        set param(arrivalRate1) [opp_getmodulepar $modp "arrivalRate"]
        set modp [opp_modulebypath "fifonet1.gen2"]
        set param(arrivalRate2) [opp_getmodulepar $modp "arrivalRate"]
        set modp [opp_modulebypath "fifonet1"]
        set param(serviceRate1) [opp_getmodulepar $modp "serviceRate1"]
        set param(serviceRate2) [opp_getmodulepar $modp "serviceRate2"]
    }

    # override original settings...
    opp_setsimoption "animation_enabled" 1
    opp_setsimoption "nexteventmarkers" 1
}

#
# Update parameter values when sliders change
#
proc cpanel_paramChanged {arr name op} {
    if {$op!="write" && $op!="w"} return

    if [catch {
        global param
        set value $param($name)
        #debug "$name changed to $value"

        if {$name=="arrivalRate1"} {
            set modp [opp_modulebypath "fifonet1.gen1"]
            opp_setmodulepar $modp "arrivalRate" $value
        } elseif {$name=="arrivalRate2"} {
            set modp [opp_modulebypath "fifonet1.gen2"]
            opp_setmodulepar $modp "arrivalRate" $value
        } elseif {$name=="serviceRate1"} {
            set modp [opp_modulebypath "fifonet1"]
            opp_setmodulepar $modp "serviceRate1" $value
        } elseif {$name=="serviceRate2"} {
            set modp [opp_modulebypath "fifonet1"]
            opp_setmodulepar $modp "serviceRate2" $value
        } else {
            error "wrong param name"
        }
    } msg] {
        puts "Error in cpanel plugin's callback: $msg"
    }
    opp_updateinspectors
}

#
# Callback for "animation on/off" button
#
proc toggleAnimation {button img_on img_off} {
    if [opp_getsimoption "animation_enabled"] {
        opp_setsimoption "animation_enabled" 0
        opp_setsimoption "nexteventmarkers" 0
        $button config -image $img_off
    } else {
        opp_setsimoption "animation_enabled" 1
        opp_setsimoption "nexteventmarkers" 1
        $button config -image $img_on
    }
}


#
# Invoke initalization code
#
cpanel_createControls

# remove main window (optional)
wm withdraw .
