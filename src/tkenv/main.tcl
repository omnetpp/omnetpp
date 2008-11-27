#=================================================================
#  MAIN.TCL - part of
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
set config(editor-wrap) none
set config(display-treeview) 1
set config(filtobjlist-class)  ""
set config(filtobjlist-name)   ""
set config(filtobjlist-order)  "Class"
set config(filtobjlist-maxcount) 100000
set config(filtobjlist-category) "mqsgvo"
set config(last-nedfile) ""
set config(confirm-exit) 1
set config(rununtil-mode) "Express (tracing off)"
set config(rununtil-time) ""
set config(rununtil-event) ""
set config(rununtil-msg) ""
set config(display-timeline) 1
set config(timeline-maxnumevents) 1000
#set config(timeline-includemsgkinds) {*}
#set config(timeline-excludemsgkinds) {}
set config(timeline-wantselfmsgs) 1
set config(timeline-wantnonselfmsgs) 1
set config(timeline-msgnamepattern) "*"
set config(timeline-msgclassnamepattern) "*"
set config(log-save-filename) "omnetpp.out"
set config(mainwin-state) "normal"
set config(mainwin-geom) ""
set config(concurrent-anim) 1
set config(logwindow-scrollbacklines) 100000

set pluginlist {}

set contextmenurules(keys) {}


#
# simple debug-print procedure
#
proc debug {str} {
   # uncomment the next line if you want debug messages
   #puts "dbg: $str"
}

#===================================================================
#    MAIN OMNET++ WINDOW
#===================================================================

proc create_omnetpp_window {} {

    global icons fonts tcl_version help_tips widgets config priv
    global B2 B3

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
      {command -command new_run -label {Set up a configuration...} -underline 7}
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
      {command -command edit_copy -label {Copy} -accel {Ctrl+C} -underline 0}
      {separator}
      {command -command edit_find -label {Find...} -accel {Ctrl+F} -underline 0}
      {command -command edit_findnext -label {Find next} -accel {Ctrl+N,F3} -underline 5}
      {separator}
      {command -command edit_filterwindowcontents -label {Filter window contents...} -accel {Ctrl+H} -underline 0}
    } {
      eval .menubar.editmenu$m add $i
    }

    # Simulate menu
    #  {command -command init_step -label {First real event} -underline 1}
    #  {separator}
    foreach i {
      {command -command one_step -label {One step} -accel {F4} -underline 4}
      {command -command run_slow -label {Slow execution} -underline 1}
      {separator}
      {command -command run_normal -label {Run}  -accel {F5} -underline 0}
      {command -command run_fast -label {Fast run (rare display updates)} -accel {F6} -underline 0}
      {command -command run_express -label {Express run (tracing off)} -accel {F7} -underline 1}
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
      {command -command inspect_simulation   -label {Simulation} -underline 1}
      {separator}
      {cascade -label {Available components} -underline 10 -menu .menubar.inspectmenu$m.components}
      {separator}
      {command -command inspect_filteredobjectlist -label {Show 'Find/inspect objects' window} -accel {Ctrl+S} -underline 0}
      {command -command inspect_bypointer -label {Inspect by pointer...} -underline 4}
      {separator}
      {command -command opp_updateinspectors -label {Refresh inspectors} -underline 0}
    } {
      eval .menubar.inspectmenu$m add $i
    }

    # Inspect|Components menu
    menu .menubar.inspectmenu$m.components -tearoff 0
    foreach i {
      {command -command inspect_componenttypes -label {NED component types} -underline 0}
      {command -command inspect_classes        -label {Registered classes} -underline 0}
      {command -command inspect_functions      -label {NED functions} -underline 4}
      {command -command inspect_enums          -label {Registered Enums} -underline 11}
      {command -command inspect_configentries  -label {Accepted config entries} -underline 0}
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
      {command -label {View text file...} -underline 0 -command {edit_textfile}}
    } {
      eval .menubar.viewmenu$m add $i
    }

    # Options menu
    foreach i {
      {command -command simulation_options -label {Simulation options...} -underline 0}
      {command -command toggle_timeline -label {Show/hide timeline} -underline 10}
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
      {sep00    -separator}
      {newrun   -image $icons(newrun)  -command {new_run}}
      {newnet   -image $icons(newnet)  -command {new_network}}
      {loadned  -image $icons(loadned) -command {load_nedfile}}
      {sep5     -separator}
      {copy     -image $icons(copy)    -command {edit_copy}}
      {find     -image $icons(find)    -command {edit_find}}
      {save     -image $icons(save)    -command {savefile "."}}
      {filter   -image $icons(filter)  -command {edit_filterwindowcontents}}
      {sep0     -separator}
      {step     -image $icons(step)    -command {one_step}}
      {run      -image $icons(run)     -command {run_normal}}
      {fastrun  -image $icons(fast)    -command {run_fast}}
      {exprrun  -image $icons(express) -command {run_express}}
      {until    -image $icons(until)   -command {run_until}}
      {stop     -image $icons(stop)    -command {stop_simulation}}
      {sep4     -separator}
      {finish   -image $icons(finish)  -command {call_finish}}
      {sep02     -separator}
      {network  -image $icons(network) -command {inspect_systemmodule}}
      {objs     -image $icons(findobj) -command {inspect_filteredobjectlist}}
      {sep6     -separator}
      {tline    -image $icons(fes)     -command {toggle_timeline}}
      {tree     -image $icons(tree)    -command {toggle_treeview}}
      {sep9     -separator}
      {options  -image $icons(config)  -command {simulation_options}}
    } {
      set b [eval iconbutton .toolbar.$i]
      pack $b -anchor n -expand 0 -fill none -side left -padx 0 -pady 2
    }

    set help_tips(.toolbar.loadned) {Load NED file for compound module definitions}
    set help_tips(.toolbar.newrun)  {Set up an inifile configuration}
    set help_tips(.toolbar.copy)    {Copy selected text to clipboard (Ctrl+C)}
    set help_tips(.toolbar.find)    {Find string in main window (Ctrl+F)}
    set help_tips(.toolbar.save)    {Save main window contents to file}
    set help_tips(.toolbar.filter)  {Filter main window contents (Ctrl+H)}
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
    set help_tips(.toolbar.objs)    {Find and inspect messages, queues, watched variables, statistics, etc (Ctrl+S)}
    set help_tips(.toolbar.tline)   {Show/hide timeline}
    set help_tips(.toolbar.tree)    {Show/hide object tree}
    set help_tips(.toolbar.options) {Simulation options}

    #################################
    # Create status bars
    #################################

    frame .statusbar
    frame .statusbar2
    frame .statusbar3

    canvas .timeline -borderwidth 2 -relief groove -height 40
    bind .timeline <Configure> "redraw_timeline"
    .timeline bind msg <Double-1> "timeline_dblclick .timeline"
    .timeline bind msgname <Double-1> "timeline_dblclick .timeline"
    .timeline bind msg <$B3> "timeline_rightclick .timeline %X %Y"
    .timeline bind msgname <$B3> "timeline_rightclick .timeline %X %Y"
    bind .timeline <Button-$B3> {timeline_popup %x %y %X %Y}

    set widgets(timeline) .timeline

    label .statusbar.networklabel \
        -relief groove -text {(no network set up)} -width 18 -anchor w -font $fonts(normal)
    label .statusbar.eventlabel \
        -relief groove -text {Event #0} -width 15  -anchor w -font $fonts(normal)
    label .statusbar.timelabel \
        -relief groove -text {T=0.0000000 (0.00s)} -width 20 -anchor w -font $fonts(normal)
    label .statusbar.nextlabel \
        -relief groove -text {Next:} -width 23 -anchor w -font $fonts(normal)

    label .statusbar2.feslength \
        -relief groove -text {Msgs scheduled: 0} -width 20 -anchor w -font $fonts(normal)
    label .statusbar2.totalmsgs \
        -relief groove -text {Msgs created: 0} -width 20 -anchor w -font $fonts(normal)
    label .statusbar2.livemsgs \
        -relief groove -text {Msgs present: 0} -width 20 -anchor w -font $fonts(normal)

    label .statusbar3.eventspersec \
        -relief groove -text {Ev/sec: n/a} -width 15 -anchor w -font $fonts(normal)
    label .statusbar3.simsecpersec \
        -relief groove -text {Simsec/sec: n/a} -width 22 -anchor w -font $fonts(normal)
    label .statusbar3.eventspersimsec \
        -relief groove -text {Ev/simsec: n/a} -width 18 -anchor w -font $fonts(normal)

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
    set help_tips(.statusbar.eventlabel)    {Sequence number of next event}
    set help_tips(.statusbar.timelabel)     {Simulation time of next event)}
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

    if {$config(display-timeline)==0} {
        .toolbar.tline config -relief raised
    } else {
        .toolbar.tline config -relief sunken
    }

    #
    # Create main text window
    #
    text .main.text -yscrollcommand ".main.sb set" -font $fonts(fixed)
    scrollbar .main.sb -command ".main.text yview"
    .main.text tag configure event -foreground blue
    .main.text tag configure log -foreground #006000
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
    pack .timeline   -anchor center -expand 0 -fill x -side top
    pack .main       -anchor center -expand 1 -fill both -side top

    focus .main.text

    ###############################
    # Hotkeys
    ###############################
    bind_commands_to_textwidget .main.text "systemmodule"
    bind_runcommands .
    bind_othercommands .
}

proc bind_runcommands {w} {
    bind $w <F4> {one_step}
    bind $w <F5> {run_normal}
    bind $w <F6> {run_fast}
    bind $w <F7> {run_express}
    bind $w <F8> {stop_simulation}
}

proc bind_othercommands {w} {
    bind $w <Control-s> [list inspect_filteredobjectlist $w]
    bind $w <Control-S> [list inspect_filteredobjectlist $w]
}

# note: modptr may be "systemmodule" or a pointer; "" means no module
proc bind_commands_to_textwidget {txt {modptr ""}} {
    global config B2 B3

    # bindings for find
    #   'break' is needed below because
    #      ^F is originally bound to 1 char right
    #      ^N is originally bound to 1 line down
    bind $txt <Key> {%W tag remove SELECT 0.0 end}
    bind $txt <Control-f> {findDialog %W; break}
    bind $txt <Control-F> {findDialog %W; break}
    bind $txt <Control-n> {findNext %W; break}
    bind $txt <Control-N> {findNext %W; break}
    bind $txt <F3> {findNext %W}
    if {$modptr!=""} {
        # bind Ctrl+H ('break' is needed because originally ^H is bound to DEL)
        bind $txt <Control-h> "moduleOutputFilterDialog %W $modptr; break"
        bind $txt <Control-H> "moduleOutputFilterDialog %W $modptr; break"
    }

    # bind Ctrl+A "Select all" ('break' is needed below because ^A=Home)
    bind $txt <Control-a> {%W tag add sel 1.0 end; break}

    # bind a context menu as well
    catch {$txt config -wrap $config(editor-wrap)}
    bind $txt <Button-$B3> [list textwidget_contextmenu %W $modptr %X %Y]
}

#===================================================================
#    LOAD BITMAPS
#===================================================================

set bitmap_ctr 0

proc load_bitmaps {path} {
   global tcl_platform bitmaps bitmap_ctr

   # On Windows, we use ";" to separate directories in $path. Using ":" (the
   # Unix separator) would cause trouble with dirs containing drive letter
   # (like "c:\bitmaps"). Using a space is also not an option (think of
   # "C:\Program Files\...").

   if {$tcl_platform(platform) == "unix"} {
       set sep {:;}
   } else {
       set sep {;}
   }

   foreach dir [split $path $sep] {
       if {$dir!=""} {
           puts -nonewline "Loading images from $dir: "
           do_load_bitmaps $dir ""
           puts ""
       }
   }

   if {$bitmap_ctr==0} {
       puts "*** no images (gif) in $path"
   }

   puts ""
}

proc do_load_bitmaps {dir prefix} {
   global bitmaps bitmap_ctr

   #debug "entering $dir"
   set files [concat [glob -nocomplain -- [file join $dir {*.gif}]] \
                     [glob -nocomplain -- [file join $dir {*.png}]]]

   # load bitmaps from this directory
   set n 0
   foreach f $files {
      set name [string tolower [file tail [file rootname $f]]]
      set img "i[incr bitmap_ctr]$name"
      if [catch {
         image create photo $img -file $f
         set size "n" ;#default
         regexp -- {^(.*)_(vl|xl|l|n|s|vs|xs)$} $name dummy name size
         do_add_bitmap $img $prefix $name $size
         incr n
      } err] {
         puts -nonewline "(*** cannot load $f: $err ***) "
      }
   }
   puts -nonewline "$prefix*: $n  "

   # recurse into subdirs
   foreach f [glob -nocomplain -- [file join $dir {*}]] {
      if {[file isdirectory $f] && [file tail $f]!="CVS"} {
         do_load_bitmaps "$f" "$prefix[file tail $f]/"
      }
   }
}


# register loaded image
proc do_add_bitmap {img prefix name size} {
   global bitmaps

   # access via the s= display string option
   set bitmaps($prefix$name,$size) $img

   # access by the legacy way
   if {$size=="n"} {
       set bitmaps($prefix$name) $img
   } else {
       set bitmaps($prefix${name}_$size) $img
   }
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
   #
   # We'd ever wanted to read plugins relative to the executable,
   # its path is available via [info nameofexecutable].

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

    bind Button <Return> {%W invoke}
}

proc startup_commands {} {
    set configname [opp_getsimoption default_config]
    set runnumber [opp_getsimoption default_run]

    # if no configname is specified but there's only one ("General"),
    # and it also contains a network, use that without a question.
    if {$configname==""} {
        set confignames [opp_getconfignames]
        set cfgnetworkname [opp_getvaluefromconfig "network"]  ;# CFGID_NETWORK
        if {($confignames=={General} && $cfgnetworkname!="")} {
            set configname "General"
        }
    }

    if {$configname != ""} {
        # set up the selected config and run number
        opp_newrun $configname $runnumber
        if [opp_isnotnull [opp_object_systemmodule]] {
            opp_inspect [opp_object_systemmodule] "(default)"
            notifyPlugins newNetwork
        }
    } else {
        # ask the user to select a network or a config
        if {$confignames=={} || $confignames=={General}} {
            new_network
        } else {
            new_run
        }
    }
}

#===================================================================
#  Utility functions
#===================================================================
proc busy {{msg {}}} {
    if {$msg != ""} {
        #$statusbar.mode config -text $msg
        # note: next 2 lines are useless (on Windows at least)
        . config -cursor watch
        update idletasks
    } else {
        #$statusbar.mode config -text "Ready"
        . config -cursor ""
        update idletasks
    }
}

#===================================================================
#
# Plugin "API":
#
#===================================================================
proc registerPlugin {pluginname} {
    global pluginlist
    lappend pluginlist $pluginname
    #debug "registered plugin \"$pluginname\""
}

proc notifyPlugins {command args} {
    global pluginlist
    foreach pluginname $pluginlist {
        #debug "invoking plugin callback $pluginname:$command"
        eval $pluginname:$command $args
    }
}

