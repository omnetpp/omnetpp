#==========================================================================
#  TKENVRC.TCL -
#            part of the Tcl/Tk windowing environment of
#                            OMNeT++
#==========================================================================

#----------------------------------------------------------------#
#  Copyright (C) 1992-2003 Andras Varga
#
#  This file is distributed WITHOUT ANY WARRANTY. See the file
#  `license' for details on this and other legal matters.
#----------------------------------------------------------------#



# save_tkenvrc --
#
#
proc save_tkenvrc {{fname ".tkenvrc"}} {
    global config

    if [catch {
        set fout [open $fname w]
        puts $fout "# Tkenv configuration"
        foreach key {
            updatefreq_fast
            updatefreq_express
            stepdelay
            use_mainwindow
            print_banners
            animation_enabled
            nexteventmarkers
            senddirect_arrows
            anim_methodcalls
            methodcalls_delay
            animation_msgnames
            animation_msgcolors
            penguin_mode
            showlayouting
            bubbles
            animation_speed
            bkpts_enabled
        } {
            set value [opp_getsimoption $key]
            puts $fout "option $key\t{$value}"
        }

        foreach key [array names config] {
            set value $config($key)
            puts $fout "config $key\t{$value}"
        }

        puts $fout [inspectorlist_tkenvrc_get_contents]

        close $fout

    } err] {
       tk_messageBox -icon error -type ok -message "Error: $err" -title {Error}
       return
    }
}


# load_tkenvrc --
#
#
proc load_tkenvrc {{fname ".tkenvrc"}} {
    global config

    if [catch {open $fname r} fin] {
        return
    }

    inspectorlist_tkenvrc_reset

    set lineno 1
    while {[gets $fin line] >= 0} {
        if {$line == ""} {incr lineno; continue}
        if [string match {#*} $line] {incr lineno; continue}

        if [catch {
            set cat [lindex $line 0]
            if {$cat == "inspector"} {
                inspectorlist_tkenvrc_process_line $line
            } elseif {$cat == "option"} {
                set key [lindex $line 1]
                set value [lindex $line 2]
                opp_setsimoption $key $value
            } elseif {$cat == "config"} {
                set key [lindex $line 1]
                set value [lindex $line 2]
                set config($key) $value
            } elseif {[llength $line]==4} {
                # old tkenvrc, patch it up
                inspectorlist_tkenvrc_process_line [concat "inspector" $line]
            }
        }] {
            tk_messageBox -icon warning -type ok -title {Error reading tkenvrc} -message "$fname line $lineno is invalid, ignoring"
        }
        incr lineno
    }
    close $fin

    opp_updateinspectors
    inspectorlist_openinspectors
    reflectSettingsInGui
}


# reflectSettingsInGui --
#
#
proc reflectSettingsInGui {} {
   toggle_treeview
   toggle_treeview
}

