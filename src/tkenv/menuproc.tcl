#==========================================================================
#  MENUPROC.TCL -
#            part of the Tcl/Tk windowing environment of
#                            OMNeT++
#==========================================================================

#----------------------------------------------------------------#
#  Copyright (C) 1992,99 Andras Varga
#  Technical University of Budapest, Dept. of Telecommunications,
#  Stoczek u.2, H-1111 Budapest, Hungary.
#
#  This file is distributed WITHOUT ANY WARRANTY. See the file
#  `license' for details on this and other legal matters.
#----------------------------------------------------------------#

#------
# Parts of this file were created using Stewart Allen's Visual Tcl (vtcl)
#------

#===================================================================
#    HELPER/GUI PROCEDURES
#===================================================================

proc check_running {} {
    if [opp_is_running] {
       messagebox {Warning} {Sorry, you cannot do this while the simulation\
                             is running. Please stop it first.} info ok
       return 1
    }
    return 0
}

proc network_present {} {
    if {[opp_object_systemmodule] == [opp_object_nullpointer]} {
       messagebox {Error} {No network has been set up yet.} info ok
       return 0
    }
    return 1
}

proc network_ready {} {
    if {[network_present] == 0} {return 0}

    if {[opp_simulation_ok] == 1} {
        return 1
    } else {
        set ans [ messagebox {Warning} {Cannot continue this simulation. Rebuild network?} \
                  question yesno ]
        if {$ans == "yes"} {
            rebuild
            return [opp_simulation_ok]
        } else {
            return 0
        }
    }
}

#===================================================================
#    MENU PROCEDURES
#===================================================================

proc about {} {
    # implements Help|About

    messagebox {About OMNeT++}  {\
TKENV
OMNeT++ Discrete Event Simulation System

(c) Andras Varga, 1992-99
Technical University of Budapest
Dept. of Telecommunications
}\
    info ok
}

proc exit_omnetpp {} {

    if [check_running] return

    if {[opp_object_systemmodule] != [opp_object_nullpointer]} {
        set ans [messagebox {Warning} {Do you really want to quit?} warning yesno]
        if {$ans == "no"} {
            return
        }
    }
    opp_exitomnetpp
}

proc create_snapshot {} {
    # implements File|Create snapshot

    if {[network_present] == 0} return

    set label ""
    set ok [inputbox {Snapshot} {Enter label:} label]
    if {$ok == 1} {
        opp_createsnapshot $label
    }
}

proc new_network {} {
    # implements File|New network...

    if [check_running] return

    set network_name [opp_getnetworktype]
    set ok [inputbox {Set Up Network} {Enter network name to set up:} network_name]
    if {$ok == 1} {
       opp_newnetwork $network_name

       if {[opp_object_systemmodule] != [opp_object_nullpointer]} {
           opp_inspect [opp_object_systemmodule] (default)
       }
    }
}

proc new_run {} {
    # implements File|New run...

    if [check_running] return

    set run_number 1
    set ok [inputbox {Set Up New Run} {Set up a Run described in omnetpp.ini:} run_number]
    if {$ok == 1} {
       opp_newrun $run_number

       if {[opp_object_systemmodule] != [opp_object_nullpointer]} {
           opp_inspect [opp_object_systemmodule] (default)
       }
    }
}

proc one_step {} {
    # implements Simulate|One step

    if [check_running] return

    if {[network_ready] == 1} {
       opp_onestep
    }
}

proc slow_exec {} {
    # implements Simulate|Slow execution

    if [check_running] return

    if {[network_ready] == 1} {
        #display_stopdialog normal
        opp_slowexec
        #remove_stopdialog
    }
}

proc run {} {
    # implements Simulate|Run

    if [check_running] return

    if {[network_ready] == 1} {
        #display_stopdialog normal
        opp_run normal
        #remove_stopdialog
    }
}

proc run_fast {} {
    # implements Simulate|Fast Run

    if [check_running] return

    if {[network_ready] == 1} {
        #display_stopdialog normal
        opp_run fast
        #remove_stopdialog
    }
}

proc run_notracing {} {
    # implements Simulate|Express Run

    if [check_running] return

    if {[network_ready] == 1} {
        display_stopdialog with_update
        opp_run_notracing
        remove_stopdialog
    }
}

proc run_until {} {
    # implements Simulate|Run until...

    if [check_running] return

    if {[network_ready] == 0} {
       return
    }

    set time ""
    set event ""
    set mode ""    ;# will be set to Normal, Fast or Express
    set ok [rununtil_dialog time event mode]
    if {$ok==0} return

    if {$mode=="Normal"} {
       #display_stopdialog normal
       opp_run normal $time $event
       #remove_stopdialog
    }
    if {$mode=="Fast"} {
       #display_stopdialog normal
       opp_run fast $time $event
       #remove_stopdialog
    }
    if {$mode=="Express"} {
       display_stopdialog with_update
       opp_run_notracing $time $event
       remove_stopdialog
    }
}

proc start_all {} {

    if [check_running] return

    if {[network_present] == 0} return
    opp_start_all
}

proc call_finish {} {

    if [check_running] return

    if {[network_present] == 0} return
    opp_call_finish
}

proc rebuild {} {
    # implements Simulate|Rebuild

    if [check_running] return

    if {[network_present] == 0} return
    opp_rebuild
}


proc stop_simulation {} {
    # implements Simulate|Stop

    if [opp_is_running] {
       opp_stopsimulation
    } else {
       messagebox {Error} {Simulation is not running.} info ok
    }
}

proc module_windows {} {
    # implements Trace|Module output...
    if {[network_present] == 0} return
    inspectfromlistbox \
         "Open module windows" \
         "Open trace window for simple modules:" \
         {Module output} \
         "modules [opp_object_systemmodule] deep simpleonly"
    # set w [opp_inspect [opp_object_systemmodule] {As Container} ds]
    # $w.buttons.type config -text modulewindow
}

proc message_windows {} {
    # implements Trace|Message sending...
    if {[network_present] == 0} return
    create_messagewindow .messagewindow
}

proc clear_windows {} {
    # implements Trace|Clear windows...
    # should delete the contents of all text windows!!!
    .main.text delete 1.0 end
}

proc inspect_anyobject {} {
    # implements Inspect|Any simulation object...
    opp_inspect [opp_object_simulation] {As Container} d
}

proc inspect_matching {} {
    # implements Inspect|By pattern matching...
    set pattern ""
    set ok [inputbox {Inspect by pattern matching...} \
                     "Enter pattern to match object pathname.\n\n\
                      Wildcards can be used: *=any string, ?=any char, {a-z}=any char from set,\n\
                      {^a-z}=any char NOT from the set. Example: '*.node\[{5-8}].*.histogram'" \
                     pattern]
    if {$ok == 0} return

    set count [opp_inspect_matching $pattern (default) countonly]
    if {$count == 0} {
        messagebox {Inspect...} "No match from pattern `$pattern'." info ok
    } elseif {$count > 8} {
        set ans [messagebox {Warning...} \
                            "The pattern `$pattern' matches $count objects. \
                             Do you really want to inspect them all?" \
                            question yesno]
        if {$ans == "yes"} {
           set type [selectfromlistbox {Choose Type...} {Select inspector type.} \
                         [opp_inspectortype all]]
           if {$type == ""} return
           opp_inspect_matching $pattern $type
        }
    } else {
        set type [selectfromlistbox {Choose Type...} {Select inspector type.} \
                         [opp_inspectortype all]]
        if {$type == ""} return
        opp_inspect_matching $pattern $type
    }
}

proc inspect_bypointer {} {
    # implements Inspect|By pointer...
    set pointer "ptr"
    set ok [inputbox {Inspect by pointer...} {Enter pointer (invalid pointer may cause segmentation fault!):} pointer]
    if {$ok == 1} {
        opp_inspect $pointer (default)
    }
}

proc inspect_systemmodule {} {
    # implements Inspect|Toplevel modules...
    if {[network_present] == 0} return
    opp_inspect [opp_object_systemmodule] (default)
}

proc inspect_messagequeue {} {
    # implements Inspect|Message queue...
    opp_inspect [opp_object_messagequeue] (default)
}

# proc inspect_modulelocals {} {
#    # implements Inspect|Local objects...
#    if {[network_present] == 0} return
#    inspectfromlistbox \
#         "Open local variables" \
#         "Open list of local variables for modules:" \
#         {Local vars} \
#         "modules [opp_object_systemmodule] deep simpleonly"
#    # set w [opp_inspect [opp_object_systemmodule] container ds]
#    # $w.buttons.type config -text {Local vars}
#}

# proc inspect_modulemembers {} {
#    # implements Inspect|Class members...
#    if {[network_present] == 0} return
#    inspectfromlistbox \
#         "Open class members" \
#         "Open list of module class members for modules:" \
#         {Class members} \
#         "modules [opp_object_systemmodule] deep simpleonly"
#    # set w [opp_inspect [opp_object_systemmodule] container ds]
#    # $w.buttons.type config -text {Class members}
#}

proc inspect_networks {} {
    opp_inspect [opp_object_networks] {As Container}
}

proc inspect_moduletypes {} {
    opp_inspect [opp_object_moduletypes] {As Container}
}

proc inspect_channeltypes {} {
    opp_inspect [opp_object_channeltypes] {As Container}
}

proc inspect_functions {} {
    opp_inspect [opp_object_functions] {As Container}
}

proc simulation_options {} {
    options_dialog
}

proc save_inspectorlist {} {
    set filename "inspect.lst"
    set filename [tk_getSaveFile -title {Save inspector list} \
                  -defaultextension "lst" -initialfile $filename \
                  -filetypes {{{Inspector list files} {*.lst}} {{All files} {*}}}]

    if {$filename!=""} {
       if [catch {open $filename w} f] {
          messagebox {Error} "Error: $f" info ok
          return
       }
       # loop through all toplevel windows and if the window name looks like
       # and inspector window's name, write a line about it in the file
       foreach win [winfo children .] {
          if [regexp {\.(ptr.*)-([0-9]+)} $win match object type] {
              puts $f "\"[opp_getobjectfullpath $object]\" \
                         [opp_inspectortype $type]"
          }
       }
       close $f
    }
}

proc load_inspectorlist {} {
    set filename "inspect.lst"
    set filename [tk_getOpenFile -title {Load inspector list} \
                  -defaultextension "lst" -initialfile $filename \
                  -filetypes {{{Inspector list files} {*.lst}} {{All files} {*}}}]

    if {$filename!=""} {
       # open file
       if [catch {open $filename r} f] {
          messagebox {Error} "Error: $f" info ok
          return
       }

       # read file line by line
       set lineno 1
       while {[gets $f line] >= 0} {
         if {$line == ""} {incr lineno; continue}
         if [string match {#*} $line] {incr lineno; continue}
         if [catch {set objectname [lindex $line 0]; set type [lindex $line 1]}] {
             messagebox {Open Inspectors in File} "`$filename' line $lineno is invalid." info ok
             incr lineno; continue
         }
         # each line contains an object name and an inspector type
         # check how many are there of them and act accordingly
         set count [opp_inspect_matching $objectname $type countonly]
         if {$count == 0} {
             messagebox {Open Inspectors in File} \
                                "`$filename' line $lineno:\n\
                                There is no object of name `$objectname'." info ok
         } elseif {$count >= 2} {
             set ans [messagebox {Open Inspectors in File} \
                                 "`$filename' line $lineno:\n\
                                 There are $count objects with name `$objectname'.\n\
                                 Do you really want to inspect them?" \
                                 question yesno]
             if {$ans == "yes"} {
                opp_inspect_matching $objectname $type
             }
         } else {
             opp_inspect_matching $objectname $type
         }
         incr lineno
       }
       close $f
    }
}

proc edit_textfile {} {
    # implements Options|File name...
    set types {
         {{Text files}             {*.txt}}
         {{Ini files}              {*.ini}}
         {{NED files}              {*.ned}}
         {{C++ files}              {*.cc}}
         {{C++ headers}            {*.h}}
         {{Output vectors}         {*.vec}}
         {{Snapshot files}         {*.sna}}
         {{Parameter change logs}  {*.pch}}
         {{Inspector lists} {*.lst}}
         {{All files}       {*}}
    }
    set filename [tk_getOpenFile -title {View/Edit text file} \
                  -defaultextension "txt" -initialfile "" \
                  -filetypes $types]

    if {$filename!=""} {
       create_fileviewer $filename
    }
}

proc view_file {filename} {
    # implements Options|Snapshot file etc.
    if [catch {open $filename r} f] {
       messagebox {Error} "Error: $f" info ok
       return
    } else {
       create_fileviewer $filename
    }
}

