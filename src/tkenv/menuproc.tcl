#==========================================================================
#  MENUPROC.TCL -
#            part of the Tcl/Tk windowing environment of
#                            OMNeT++
#==========================================================================

#----------------------------------------------------------------#
#  Copyright (C) 1992-2003 Andras Varga
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
    if {[opp_getsimulationstate] == "SIM_RUNNING"} {
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

    if {[is_simulation_ok] == 1} {
        return 1
    } else {
        set ans [messagebox {Warning} {Cannot continue this simulation. Rebuild network?} question yesno]
        if {$ans == "yes"} {
            rebuild
            return [is_simulation_ok]
        } else {
            return 0
        }
    }
}

proc is_simulation_ok {} {

    set state [opp_getsimulationstate]
    if {$state == "SIM_NEW" || $state == "SIM_RUNNING" || $state == "SIM_READY"} {
        return 1
    } else {
        return 0
    }
}

#===================================================================
#    MENU PROCEDURES
#===================================================================

proc about {} {
    # implements Help|About

    aboutDialog "About OMNeT++/Tkenv" {\
OMNeT++/Tkenv

(c) Andras Varga, 1992-2003

NO WARRANTY -- see license for details.
}
}

proc exit_omnetpp {} {

    if [check_running] return

    if {[opp_object_systemmodule] != [opp_object_nullpointer]} {
        set ans [messagebox {Warning} {Do you really want to quit?} warning yesno]
        if {$ans == "no"} {
            return
        }
    }

    save_tkenvrc

    opp_exitomnetpp
}

proc create_snapshot {} {
    # implements File|Create snapshot

    if {[network_present] == 0} return

    set label ""
    set ok [inputbox {Snapshot} {Give a label to current simulation snapshot:} label]
    if {$ok == 1} {
        if [catch {opp_createsnapshot $label} err] {
          messagebox {Error} "Error: $err" error ok
          return
        }
        set snapshotfile [opp_getfilename snapshot]
        if {$snapshotfile==""} {
            messagebox {Snapshot created} "Current state of simulation has been saved." info ok
        } else {
            set ans [messagebox {Snapshot created} "Current state of simulation has been \
saved into \"$snapshotfile\". Do you want to open it now in a file viewer window?" question yesno]
            if {$ans == "yes"} {
                view_snapshotfile
            }
        }
    }
}

proc new_network {} {
    # implements File|New network...

    if [check_running] return

    # get list of network names
    set networks [opp_getchildobjects [opp_object_networks]]
    set netnames {}
    foreach net $networks {
        lappend netnames [opp_getobjectfullname $net]
    }

    # pop up dialog, with current network as default
    set netname [opp_getnetworktype]
    set ok [comboSelectionDialog "Set up network" "Set up a network, using parameters described \nin the \[General\] section of omnetpp.ini." "Select network:" netname $netnames]
    if {$ok == 1} {
       opp_newnetwork $netname

       if {[opp_object_systemmodule] != [opp_object_nullpointer]} {
           opp_inspect [opp_object_systemmodule] (default)
       }
    }
}

proc new_run {} {
    # implements File|New run...

    if [check_running] return

    # compile list of runs
    set sectionlist [opp_getinisectionnames]
    set runlist {General}
    foreach section $sectionlist {
       if {[regexp -nocase -- {^(Run *)?([0-9]+) *$} $section dummy dummy1 runno]} {
           lappend runlist "Run $runno"
       }
    }

    # determine detault run to be offered
    set run "Run [opp_getrunnumber]"
    if {$run=="Run 0"} {
        if {[llength $runlist]>1} {
            set run [lindex $runlist 1]
        } else {
            set run "General"
        }
    }

    # pop up selection dialog
    set ok [comboSelectionDialog "Set up new Run" "Set up one of the runs described in omnetpp.ini." {Select run:} run $runlist]
    if {$ok == 1} {
       if {$run == "General"} {
           set runno "-1"
       } elseif {[regexp -nocase -- {^(Run *)?([0-9]+) *$} $run dummy dummy1 runno]} {
           # OK -- regexp matched
       } else {
           messagebox "Error" "Which run do you mean by '$run'?" info ok
           return;
       }
       opp_newrun $runno

       if {[opp_object_systemmodule] != [opp_object_nullpointer]} {
           opp_inspect [opp_object_systemmodule] (default)
       }
    }
}

proc edit_find {} {
   # implements Edit|Find...
   findDialog .main.text
}

proc edit_findnext {} {
   # implements Edit|Find next
   findNext .main.text
}

proc toggle_treeview {} {
   global config widgets

   if {$config(display-treeview)==1} {
       set config(display-treeview) 0
       pack forget $widgets(manager)
       .toolbar.tree config -relief raised
   } else {
       set config(display-treeview) 1
       pack $widgets(manager) -before .main.text -expand 0 -fill y  -side left
       .toolbar.tree config -relief sunken
       updateTreeManager
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

proc run_express {} {
    # implements Simulate|Express Run

    if [check_running] return

    if {[network_ready] == 1} {
        display_stopdialog with_update
        opp_run_express
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
       opp_run_express $time $event
       remove_stopdialog
    }
}

proc start_all {} {

    if [check_running] return

    if {[network_present] == 0} return
    opp_start_all
}

proc call_finish {} {

    # check state is not SIM_RUNNING
    if [check_running] return

    # check state is not SIM_NONET
    if {[network_present] == 0} return

    # check state is not SIM_FINISHCALLED
    if {[opp_getsimulationstate] == "SIM_FINISHCALLED"} {
       messagebox {Error} {finish() has been invoked already.} info ok
       return
    }

    # check state is not SIM_ERROR
    if {[opp_getsimulationstate] == "SIM_ERROR"} {
       set ans [messagebox {Warning} \
                  {Simulation was stopped with error, calling finish() might produce unexpected results. Proceed anyway?} \
                  question yesno]
       if {$ans == "no"} {
           return
       }
    } else {
       set ans [messagebox {Question} \
                  {Do you want to conclude this simulation run and invoke finish() on all modules?} \
                  question yesno]
       if {$ans == "no"} {
           return
       }
    }

    opp_finish_simulation
}

proc rebuild {} {
    # implements Simulate|Rebuild

    if [check_running] return

    if {[network_present] == 0} return
    opp_rebuild
}


proc stop_simulation {} {
    # implements Simulate|Stop

    if {[opp_getsimulationstate] == "SIM_RUNNING"} {
       opp_stopsimulation
    } else {
       #messagebox {Error} {Simulation is not running.} info ok
    }
}

proc module_windows {} {
    # implements Trace|Module output...
    if {[network_present] == 0} return
    inspectfromlistbox \
         "Open module windows" \
         "Open trace window for modules:" \
         {Module output} \
         "modules [opp_object_systemmodule] deep"
    # set w [opp_inspect [opp_object_systemmodule] {(default)} ds]
    # $w.buttons.type config -text modulewindow
}

proc message_windows {} {
    # implements Trace|Message sending...
    if {[network_present] == 0} return
    create_messagewindow .messagewindow
}

proc clear_windows {} {
    # implements Trace|Clear windows...
    # also called back from C++ code
    # FIXME: should delete the contents of all text windows!!!
    .main.text delete 1.0 end
}

proc inspect_filteredobjectlist {} {
    # implements Find/inspect objects...
    filteredobjectlist_dialog
}

# Obsoleted by inspect_filteredobjectlist:
# proc inspect_anyobject {} {
#    # implements Inspect|Any simulation object...
#    opp_inspect [opp_object_simulation] {(default)} d
# }

# Obsoleted by inspect_filteredobjectlist:
# proc inspect_matching {} {
#     # implements Inspect|By pattern matching...
#     set pattern ""
#     set ok [inputbox {Inspect by pattern matching...} \
#                      "Enter pattern to match object pathname.\n\n\
#                       Wildcards can be used: *=any string, ?=any char, {a-z}=any char from set,\n\
#                       {^a-z}=any char NOT from the set. Example: '*.node\[{5-8}].*.histogram'" \
#                      pattern]
#     if {$ok == 0} return
#
#     set count [opp_inspect_matching $pattern (default) countonly]
#     if {$count == 0} {
#         messagebox {Inspect...} "No match from pattern `$pattern'." info ok
#     } elseif {$count > 8} {
#         set ans [messagebox {Warning...} \
#                             "The pattern `$pattern' matches $count objects. \
#                              Do you really want to inspect them all?" \
#                             question yesno]
#         if {$ans == "yes"} {
#            set type [listboxSelectionDialog {Choose Type...} {Select inspector type.} \
#                          [opp_inspectortype all]]
#            if {$type == ""} return
#            opp_inspect_matching $pattern $type
#         }
#     } else {
#         set type [listboxSelectionDialog {Choose Type...} {Select inspector type.} \
#                          [opp_inspectortype all]]
#         if {$type == ""} return
#         opp_inspect_matching $pattern $type
#     }
# }

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
    opp_inspect [opp_object_networks] {(default)}
}

proc inspect_moduletypes {} {
    opp_inspect [opp_object_moduletypes] {(default)}
}

proc inspect_channeltypes {} {
    opp_inspect [opp_object_channeltypes] {(default)}
}

proc inspect_functions {} {
    opp_inspect [opp_object_functions] {(default)}
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
       inspectorlist_save $f
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
       inspectorlist_load $f
       close $f
    }
}

proc edit_textfile {} {
    # implements Options|File name...
    set types {
         {{Text files}             {*.txt}}
         {{Ini files}              {*.ini}}
         {{NED files}              {*.ned}}
         {{C++ files}              {*.cc *.cpp}}
         {{C++ headers}            {*.h}}
         {{Output vectors}         {*.vec}}
         {{Output scalars}         {*.sca}}
         {{Snapshot files}         {*.sna}}
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

proc view_inifile {} {
    view_file [opp_getfilename ini]
}

proc view_outputvectorfile {} {
    set fname [opp_getfilename outvector]
    if {$fname == ""} {
       messagebox {Info} "The current output vector manager doesn't use file output." info ok
       return
    }
    if {![file exists $fname]} {
       messagebox {Info} "Output vector file not yet created (no values recorded yet)." info ok
       return
    }
    view_file $fname
}

proc view_outputscalarfile {} {
    set fname [opp_getfilename outscalar]
    if {$fname == ""} {
       messagebox {Info} "The current output scalar manager doesn't use file output." info ok
       return
    }
    if {![file exists $fname]} {
       messagebox {Info} "Output scalar file not yet created (no output scalars written)." info ok
       return
    }
    view_file $fname
}

proc view_snapshotfile {} {
    set fname [opp_getfilename snapshot]
    if {$fname == ""} {
       messagebox {Info} "The current snapshot manager doesn't use file output." info ok
       return
    }
    if {![file exists $fname]} {
       messagebox {Info} "Snapshot file not yet created (no snapshots done yet)." info ok
       return
    }
    view_file $fname
}

proc view_file {filename} {
    if [catch {open $filename r} f] {
       messagebox {Error} "Error: $f" info ok
       return
    } else {
       create_fileviewer $filename
    }
}

proc load_tkenvrc {} {
  if [catch {
      if [catch {open ".tkenvrc" r} f] {
      } else {
          puts "Loading .tkenvrc"
          inspectorlist_load $f
          close $f
      }
  } err] {
      messagebox {Error} "Error: $err" error ok
      return
  }
}

proc save_tkenvrc {} {
  if [catch {
      set f [open ".tkenvrc" w]
      inspectorlist_save $f
      close $f
  } err] {
      messagebox {Error} "Error: $err" error ok
      return
  }
}

