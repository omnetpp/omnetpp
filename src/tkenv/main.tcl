#==========================================================================
#  MAIN.TCL -
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
#    GLOBAL VARIABLES
#===================================================================

# default config settings
set config(editor-findstring) ""
set config(editor-case-sensitive) 0
set config(editor-whole-words) 0
set config(editor-regexp) 0
set config(editor-backwards) 1
set config(display-treeview) 1
set config(filtobjlist-class)  ""
set config(filtobjlist-name)   ""
set config(filtobjlist-order)  "Class"
set config(filtobjlist-maxcount) 100000
set config(filtobjlist-category) "mqsgvo"
set config(last-nedfile) ""

set pluginlist {}

#===================================================================
#    MAIN OMNET++ WINDOW
#===================================================================

proc create_omnetpp_window {} {

    global icons fonts tcl_version help_tips widgets config

    wm focusmodel . passive
    wm geometry . 640x350
    #wm maxsize . 1009 738
    wm minsize . 1 1
    wm overrideredirect . 0
    wm resizable . 1 1
    wm deiconify .
    wm title . "OMNeT++/Tkenv"
    wm protocol . WM_DELETE_WINDOW {exit_omnetpp}

    #################################
    # Menu bar
    #################################

    menu .menubar
    . config -menu .menubar

    # Create menus
    foreach i {
       {filemenu     -$label_opt File -underline 0}
       {editmenu     -$label_opt Edit -underline 0}
       {simulatemenu -$label_opt Simulate -underline 0}
       {tracemenu    -$label_opt Trace -underline 0}
       {inspectmenu  -$label_opt Inspect -underline 0}
       {viewmenu     -$label_opt View -underline 0}
       {optionsmenu  -$label_opt Options -underline 0}
       {helpmenu     -$label_opt Help -underline 0}
    } {
       if {$tcl_version < 8.0} {
           set label_opt "text"; set m ".m"
           set mb [eval menubutton .menubar.$i -padx 4 -pady 3]
           menu $mb.m -tearoff 0
           $mb config -menu $mb.m
       } else {
           set label_opt "label"; set m ""
           eval .menubar add cascade -menu .menubar.$i
           menu ".menubar.[lindex $i 0]" -tearoff 0
       }
    }

    # File menu
    foreach i {
      {command -command new_run -label {Set up a run...} -underline 7}
      {command -command new_network -label {Set up a network...} -underline 7}
      {separator}
      {command -command load_nedfile -label {Load NED file...} -underline 0}
      {separator}
      {command -command create_snapshot -label {Create snapshot...} -underline 7}
      {separator}
      {command -command exit_omnetpp -label Exit -underline 1}
    } {
      eval .menubar.filemenu$m add $i
    }

    # Edit menu
    foreach i {
      {command -command edit_find -label {Find...} -accel {Ctrl-F} -underline 0}
      {command -command edit_findnext -label {Find next} -accel {Ctrl-N,F3} -underline 5}
    } {
      eval .menubar.editmenu$m add $i
    }

    # Simulate menu
    #  {command -command init_step -label {First real event} -underline 1}
    #  {separator}
    foreach i {
      {command -command one_step -label {One step} -accel {F4} -underline 4}
      {command -command slow_exec -label {Slow execution} -underline 1}
      {separator}
      {command -command run -label {Run}  -accel {F5} -underline 0}
      {command -command run_fast -label {Fast run (rare display updates)} -accel {F6} -underline 0}
      {command -command run_express -label {Express run (tracing off)} -accel {F7} -underline 0}
      {command -command run_until -label {Run until...} -underline 4}
      {separator}
      {command -command stop_simulation -label {Stop execution} -accel {F8} -underline 0}
      {separator}
      {command -command call_finish -label {Call finish() for all modules} -underline 0}
      {command -command rebuild -label {Rebuild network} -underline 1}
    } {
      eval .menubar.simulatemenu$m add $i
    }

    # Trace menu
    foreach i {
      {command -command module_windows -label {Module output...} -underline 0}
      {command -command message_windows -label {Message sending...} -underline 8}
      {separator}
      {command -command clear_windows -label {Clear main window} -underline 0}
    } {
      eval .menubar.tracemenu$m add $i
    }

    # Inspect menu
    # The following two items have been replaced by the new Find/inspect objects... dialog:
    #  {command -command inspect_anyobject -label {From list of all objects...} -underline 0}
    #  {command -command inspect_matching -label {By pattern matching...} -underline 3}
    foreach i {
      {command -command inspect_systemmodule -label {Network} -underline 0}
      {command -command inspect_messagequeue -label {Scheduled events (FES)} -underline 0}
      {separator}
      {cascade -label {Available components} -underline 10 -menu .menubar.inspectmenu$m.components}
      {separator}
      {command -command inspect_filteredobjectlist -label {Find/inspect objects...} -accel {Ctrl-S} -underline 0}
      {command -command inspect_bypointer -label {By pointer...} -underline 4}
      {separator}
      {command -command opp_updateinspectors -label {Refresh inspectors} -underline 0}
    } {
      eval .menubar.inspectmenu$m add $i
    }

    # Inspect|Components menu
    menu .menubar.inspectmenu$m.components -tearoff 0
    foreach i {
      {command -command inspect_networks -label {Available networks} -underline 10}
      {command -command inspect_moduletypes -label {Module types} -underline 0}
      {command -command inspect_channeltypes -label {Channel types} -underline 0}
      {command -command inspect_functions -label {Registered functions} -underline 11}
    } {
      eval .menubar.inspectmenu$m.components add $i
    }

    # View menu
    foreach i {
      {command -label {Ini file} -underline 0 -command view_inifile}
      {command -label {README} -underline 0 -command {view_file README}}
      {separator}
      {command -label {Output vector file} -underline 7 -command view_outputvectorfile}
      {command -label {Output scalar file} -underline 7 -command view_outputscalarfile}
      {command -label {Snapshot file} -underline 0 -command view_snapshotfile}
      {separator}
      {command -label {View/Edit file...} -underline 0 -command {edit_textfile}}
    } {
      eval .menubar.viewmenu$m add $i
    }

    # Options menu
    foreach i {
      {command -command simulation_options -label {Simulation options...} -underline 0}
      {command -command toggle_treeview -label {Show/hide object tree} -underline 1}
      {separator}
      {command -label {Load config...} -underline 0 -command load_tkenv_config}
      {command -label {Save config...} -underline 1 -command save_tkenv_config}
    } {
      eval .menubar.optionsmenu$m add $i
    }

    # Help menu
    foreach i {
      {command -command about -label {About OMNeT++/Tkenv} -underline 0}
    } {
      eval .menubar.helpmenu$m add $i
    }

    #################################
    # Create toolbar
    #################################

    frame .toolbar -relief raised -borderwidth 1

    #  {restart  -image $icons(restart) -command {rebuild}}

    foreach i {
      {sep01    -separator}
      {newrun   -image $icons(newrun)  -command {new_run}}
      {newnet   -image $icons(newnet)  -command {new_network}}
      {loadned  -image $icons(loadned) -command {load_nedfile}}

      {sep0     -separator}
      {step     -image $icons(step)    -command {one_step}}
      {sep1     -separator}
      {run      -image $icons(run)     -command {run}}
      {fastrun  -image $icons(fast)    -command {run_fast}}
      {exprrun  -image $icons(express) -command {run_express}}
      {sep2     -separator}
      {until    -image $icons(until)   -command {run_until}}
      {sep3     -separator}
      {stop     -image $icons(stop)    -command {stop_simulation}}
      {sep4     -separator}
      {finish   -image $icons(finish)  -command {call_finish}}
      {sep5     -separator}
      {network  -image $icons(network) -command {inspect_systemmodule}}
      {fes      -image $icons(fes)     -command {inspect_messagequeue}}
      {objs     -image $icons(findobj) -command {inspect_filteredobjectlist}}
      {sep6     -separator}
      {tree     -image $icons(tree)    -command {toggle_treeview}}
      {options  -image $icons(config)  -command {simulation_options}}
      {sep7     -separator}
      {find     -image $icons(find)    -command {edit_find}}
    } {
      set b [eval iconbutton .toolbar.$i]
      pack $b -anchor n -expand 0 -fill none -side left -padx 0 -pady 2
    }

    set help_tips(.toolbar.loadned) {Load NED file for compound module definitions}
    set help_tips(.toolbar.newrun)  {Set up a run}
    set help_tips(.toolbar.newnet)  {Set up a network}
    set help_tips(.toolbar.step)    {Execute one event (F4)}
    set help_tips(.toolbar.run)     {Run with full animation (F5)}
    set help_tips(.toolbar.fastrun) {Run faster: no animation and rare inspector updates (F6)}
    set help_tips(.toolbar.exprrun) {Run at full speed: no text output, animation or inspector updates (F7)}
    set help_tips(.toolbar.until)   {Run until time or event number}
    set help_tips(.toolbar.stop)    {Stop running simulation (F8)}
    set help_tips(.toolbar.restart) {Rebuild network}
    set help_tips(.toolbar.finish)  {Call finish()}
    set help_tips(.toolbar.network) {Inspect network}
    set help_tips(.toolbar.fes)     {Inspect scheduled events (Future Event Set)}
    set help_tips(.toolbar.objs)    {Find/inspect objects (Ctrl-S)}
    set help_tips(.toolbar.tree)    {Show/hide object tree}
    set help_tips(.toolbar.options) {Simulation options}
    set help_tips(.toolbar.find)    {Find string in main window (Ctrl-F)}

    #################################
    # Create status bars
    #################################

    frame .statusbar
    frame .statusbar2
    frame .statusbar3

    label .statusbar.networklabel \
        -relief groove -text {(no network set up)} -width 18 -anchor w
    label .statusbar.eventlabel \
        -relief groove -text {Event #0} -width 15  -anchor w
    label .statusbar.timelabel \
        -relief groove -text {T=0.0000000 (0.00s)} -width 20 -anchor w
    label .statusbar.nextlabel \
        -relief groove -text {Next:} -width 23 -anchor w

    label .statusbar2.feslength \
        -relief groove -text {Msgs scheduled: 0} -width 20 -anchor w
    label .statusbar2.totalmsgs \
        -relief groove -text {Msgs created: 0} -width 20 -anchor w
    label .statusbar2.livemsgs \
        -relief groove -text {Msgs present: 0} -width 20 -anchor w

    label .statusbar3.eventspersec \
        -relief groove -text {Ev/sec: n/a} -width 15 -anchor w
    label .statusbar3.simsecpersec \
        -relief groove -text {Simsec/sec: n/a} -width 22 -anchor w
    label .statusbar3.eventspersimsec \
        -relief groove -text {Ev/simsec: n/a} -width 18 -anchor w

    pack .statusbar.networklabel -anchor n -expand 1 -fill x -side left
    pack .statusbar.eventlabel -anchor n -expand 1 -fill x -side left
    pack .statusbar.timelabel -anchor n -expand 1 -fill x -side left
    pack .statusbar.nextlabel -anchor n -expand 1 -fill x -side right

    pack .statusbar2.feslength -anchor n -expand 1 -fill x -side left
    pack .statusbar2.totalmsgs -anchor n -expand 1 -fill x -side left
    pack .statusbar2.livemsgs -anchor n -expand 1 -fill x -side left

    pack .statusbar3.eventspersec -anchor n -expand 1 -fill x -side left
    pack .statusbar3.simsecpersec -anchor n -expand 1 -fill x -side left
    pack .statusbar3.eventspersimsec -anchor n -expand 1 -fill x -side left

    set help_tips(.statusbar.networklabel)  {Network currently set up for execution}
    set help_tips(.statusbar.eventlabel)    {Current event number}
    set help_tips(.statusbar.timelabel)     {Current simulation time}
    set help_tips(.statusbar.nextlabel)     {Module where next event will occur}

    set help_tips(.statusbar2.feslength)    {Number of events (messages) currently scheduled}
    set help_tips(.statusbar2.totalmsgs)    {Total number of messages created since start of the simulation}
    set help_tips(.statusbar2.livemsgs)     {Number of existing message objects, including scheduled ones}

    set help_tips(.statusbar3.eventspersec)    {Performance: events processed per second}
    set help_tips(.statusbar3.simsecpersec)    {Relative speed: simulated seconds processed per second}
    set help_tips(.statusbar3.eventspersimsec) {Event density: events per simulated second}


    #################################
    # Create main display area
    #################################
    frame .main -borderwidth 1 -height 30 -relief sunken -width 30

    #
    # Create manager (vert. tree view)
    #
    frame .main.mgr -relief flat -borderwidth 1

    vertResizeBar .main.mgr.resize .main.mgr.tree

    canvas .main.mgr.tree -width 140 -bg #ffffe0 -bd 0 -yscrollcommand ".main.mgr.sb set"
    scrollbar .main.mgr.sb -command ".main.mgr.tree yview"
    pack .main.mgr.resize -side right -fill y
    pack .main.mgr.sb -side right -fill y
    pack .main.mgr.tree -side left -fill y -padx 0 -pady 0 -ipadx 0 -ipady 0

    set widgets(manager) .main.mgr
    initTreeManager
    pack .main.mgr -anchor center -expand 0 -fill y  -side left

    if {$config(display-treeview)==0} {
        .toolbar.tree config -relief raised
    } else {
        .toolbar.tree config -relief sunken
    }

    #
    # Create main text window
    #
    text .main.text -yscrollcommand ".main.sb set"
    scrollbar .main.sb -command ".main.text yview"
    .main.text tag configure event -foreground blue
    .main.text tag configure SELECT -back #808080 -fore #ffffff

    pack .main.sb -anchor center -expand 0 -fill y  -side right
    pack .main.text -anchor center -expand 1 -fill both -side right

    #################################
    # Pack everything
    #################################
    pack .toolbar    -anchor center -expand 0 -fill x -side top
    pack .statusbar  -anchor center -expand 0 -fill x -side top
    pack .statusbar2 -anchor center -expand 0 -fill x -side top
    pack .statusbar3 -anchor center -expand 0 -fill x -side top
    pack .main       -anchor center -expand 1 -fill both -side top

    focus .main.text

    ###############################
    # Hotkeys
    ###############################
    bind_findcommands_to_textwidget .main.text
    bind_runcommands .
}

proc bind_runcommands {w} {
    bind $w <F4> {one_step}
    bind $w <F5> {run}
    bind $w <F6> {run_fast}
    bind $w <F7> {run_express}
    bind $w <F8> {stop_simulation}
    bind $w <Control-s> {inspect_filteredobjectlist}
    bind $w <Control-S> {inspect_filteredobjectlist}
}

proc bind_findcommands_to_textwidget {txt} {
    # bindings for find
    #   'break' is needed below because
    #      ^F is originally bound to 1 char right
    #      ^N is originally bound to 1 line down
    bind $txt <Key> {%W tag remove SELECT 0.0 end}
    bind $txt <Control-f> "findDialog $txt;break"
    bind $txt <Control-F> "findDialog $txt;break"
    bind $txt <Control-n> "findNext $txt;break"
    bind $txt <Control-N> "findNext $txt;break"
    bind $txt <F3> "findNext $txt"
}

#===================================================================
#    LOAD BITMAPS
#===================================================================

proc load_bitmaps {path} {
   global tcl_platform

   puts "Loading bitmaps from $path:"

   # On Windows, we use ";" to separate directories in $path. Using ":" (the
   # Unix separator) would cause trouble with dirs containing drive letter
   # (like "c:\bitmaps"). Using a space is also not an option (think of
   # "C:\Program Files\...").

   if {$tcl_platform(platform) == "unix"} {
       set sep {:;}
   } else {
       set sep {;}
   }

   set files {}
   foreach dir [split $path $sep] {
       set files [concat $files \
                     [glob -nocomplain -- [file join $dir {*.gif}]] \
                     [glob -nocomplain -- [file join $dir {*.xpm}]] \
                     [glob -nocomplain -- [file join $dir {*.xbm}]]]
   }
   if {[llength $files] == 0} {
       puts "*** no bitmaps (gif,xpm,xbm) in $path"
       return
   }

   foreach f $files {

      set type ""
      case [string tolower [file extension $f]] {
        {.xbm} {set type bitmap}
        {.xbm} {set type photo}
        {.gif} {set type photo}
      }
      if {$type==""} {error "load_bitmaps: internal error"}

      set name [string tolower [file tail [file rootname $f]]]
      if {$name=="proc"} {
         puts -nonewline "(*** $name -- Tk dislikes this name, skipping ***) "
      } elseif [catch {image type $name}] {
         if [catch {
            image create $type $name -file $f
            puts -nonewline "$name "
         } err] {
            puts -nonewline "(*** $name is bad: $err ***) "
         }
      } else {
         puts -nonewline "($name is duplicate) "
      }
   }
   puts ""
   puts ""
}


#===================================================================
#    LOAD PLUGINS
#===================================================================

proc load_plugins {path} {
   global tcl_platform

   # On Windows, we use ";" to separate directories in $path. Using ":" (the
   # Unix separator) would cause trouble with dirs containing drive letter
   # (like "c:\bitmaps"). Using a space is also not an option (think of
   # "C:\Program Files\...").

   if {$tcl_platform(platform) == "unix"} {
       set dllpattern "*.so*"
       set sep {:;}
   } else {
       set dllpattern "*.dll"
       set sep {;}
   }

   puts "Plugin path: $path"

   set tclfiles 0
   set dllfiles 0
   foreach dir [split $path $sep] {

      # load tcl files
      set files [lsort [glob -nocomplain -- [file join $dir {*.tcl}]]]
      incr tclfiles [llength $files]
      if {[llength $files]!=0} {
          puts -nonewline "Loading tcl files from $dir: "
          foreach file $files {
             if [catch {source $file} errmsg] {
                 puts ""
                 puts "*** error sourcing $file: $errmsg"
             } else {
                 set name [string tolower [file tail $file]]
                 puts -nonewline "$name "
             }
          }
         puts ""
      }

      # load dynamic libraries
      set files [lsort [glob -nocomplain -- [file join $dir $dllpattern]]]
      incr dllfiles [llength $files]
      if {[llength $files]!=0} {
          puts -nonewline "Loading so/dll files from $dir: "
          foreach file $files {
             if [catch {opp_loadlib $file} errmsg] {
                 puts ""
                 puts "*** error loading shared library $file: $errmsg"
             } else {
                 set name [string tolower [file tail $file]]
                 puts -nonewline "$name "
             }
          }
          puts ""
      }
   }
}

#===================================================================
#    GENERIC BINDINGS
#===================================================================

proc generic_bindings {} {
   global help_tips
   set help_tips(helptip_proc) get_help_tip
   set help_tips(width) 500

   bind Button <Return> {tkButtonInvoke %W}
}

proc startup_commands {} {

    set run [opp_getsimoption default-run]
    if {$run>0} {
        opp_newrun $run
        if {[opp_object_systemmodule] != [opp_object_nullpointer]} {
            opp_inspect [opp_object_systemmodule] (default)
            notifyPlugins newNetwork
        }
    } else {
        if {[lsearch -regexp [opp_getinisectionnames] {^Run [0-9]+$}]!=-1} {
            new_run
        } else {
            new_network
        }
    }
}

#------------------------------------------------------------------

#
# Plugin "API":
#
proc registerPlugin {pluginname} {
    global pluginlist
    lappend pluginlist $pluginname
    #puts "DBG: registered plugin \"$pluginname\""
}

proc notifyPlugins {command args} {
    global pluginlist
    foreach pluginname $pluginlist {
        #puts "DBG: invoking plugin callback $pluginname:$command"
        eval $pluginname:$command $args
    }
}

