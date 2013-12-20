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
set config(timeline-wantselfmsgs) 1
set config(timeline-wantnonselfmsgs) 1
set config(timeline-wantsilentmsgs) 0
set config(log-save-filename) "omnetpp.out"
set config(mainwin-state) "normal"
set config(mainwin-geom) ""
set config(concurrent-anim) 1
set config(logwindow-scrollbacklines) 100000
set config(zoomby-factor) 1.1
set config(layout-may-resize-window) 0
set config(layout-may-change-zoom) 1

set pluginlist {}

set contextmenurules(keys) {}


#
# simple debug-print procedure
#
proc debug {str} {
   # UNCOMMENT the next lines if you want debug messages
   #set procname_with_args [info level [expr [info level]-1]]
   #set procname [lindex $procname_with_args 0]
   #puts "dbg: \[$procname\]: $str"
}

#===================================================================
#    MAIN OMNET++ WINDOW
#===================================================================

proc createOmnetppWindow {} {

    global tcl_version tk_version tk_patchLevel tcl_platform
    global icons fonts help_tips widgets config priv
    global B2 B3

    wm focusmodel . passive
    wm geometry . 640x350
    #wm maxsize . 1009 738
    wm minsize . 1 1
    wm overrideredirect . 0
    wm resizable . 1 1
    wm deiconify .
    wm title . "OMNeT++/Tkenv"
    wm protocol . WM_DELETE_WINDOW {exitOmnetpp}

    # set the application icon
    set iconphoto_main $icons(logo128m)
    set iconphoto_other $icons(logo128w)
    if {$tcl_platform(platform) == "windows"} {
        if {![isTkAtLeast "8.5.6"]} {
            # Tk bug #2504402: "On Windows the wm iconphoto command only works with
            # 32-bit color displays. Other display settings produce a black icon."
            # This bug appears to have been fixed in Tk 8.5.6. For earlier versions,
            # only turn on icon for 32-bit displays.
            if {[winfo screendepth .] == 32} {
                # Bug #1467997: "the displayed icons have red and blue colors transposed."
                # This bug was was fixed in 8.4.16. For earlier versions, we manually swap
                # the R and B channels.
                if {![isTkAtLeast "8.4.16"]} {
                    opp_swapredandblue $iconphoto_other
                    opp_swapredandblue $iconphoto_main
                }
                # note: on win7, without the "after" command wm iconphoto causes startup glitches (empty window+delay)
                after 200 "wm iconphoto . -default $iconphoto_other; wm iconphoto . $iconphoto_main"
            }
        } else {
            # note: on win7, without the "after" command wm iconphoto causes startup glitches (empty window+delay)
            after 200 "wm iconphoto . -default $iconphoto_other; wm iconphoto . $iconphoto_main"
        }
    } elseif {[string equal [tk windowingsystem] aqua]}  {
        # do nothing: on Mac OS X 10.5, wm iconphoto is not implemented (results in error);
        # on 10.6, it does not cause an error but simply has no apparent effect
    } else {
        # On linux, 8.4.19 was tested and known to be working.
        if {[isTkAtLeast "8.4.19"]} {
            wm iconphoto . -default $iconphoto_other
            wm iconphoto . $iconphoto_main
        }
    }

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
      {command -command newRun -label {Set Up a Configuration...} -underline 7}
      {separator}
      {command -command loadNedFile -label {Load NED File...} -underline 0}
      {command -command newNetwork -label {Set Up an Unconfigured Network...} -underline 7}
      {separator}
      {command -command createSnapshot -label {Create Snapshot...} -underline 7}
      {separator}
      {command -command exitOmnetpp -label Exit -underline 1}
    } {
      eval .menubar.filemenu$m add $i
    }

    # Edit menu
    foreach i {
      {command -command editCopy -label {Copy} -accel {Ctrl+C} -underline 0}
      {separator}
      {command -command editFind -label {Find...} -accel {Ctrl+F} -underline 0}
      {command -command editFindNext -label {Find Next} -accel {Ctrl+N,F3} -underline 5}
      {separator}
      {command -command editFilterWindowContents -label {Filter Window Contents...} -accel {Ctrl+H} -underline 0}
    } {
      eval .menubar.editmenu$m add $i
    }

    # Simulate menu
    #  {command -command init_step -label {First real event} -underline 1}
    #  {separator}
    foreach i {
      {command -command oneStep -label {One Step} -accel {F4} -underline 4}
      {command -command runSlow -label {Slow Execution} -underline 1}
      {separator}
      {command -command runNormal -label {Run}  -accel {F5} -underline 0}
      {command -command runFast -label {Fast Run (rare display updates)} -accel {F6} -underline 0}
      {command -command runExpress -label {Express Run (tracing off)} -accel {F7} -underline 1}
      {command -command runUntil -label {Run Until...} -underline 4}
      {separator}
      {command -command debugNextEvent -label {Debug Next Event} -accel {Ctrl+F9} -underline 1}
      {separator}
      {command -command stopSimulation -label {Stop Execution} -accel {F8} -underline 0}
      {separator}
      {command -command callFinish -label {Call finish() for All Modules} -underline 0}
      {command -command rebuild -label {Rebuild Network} -underline 1}
    } {
      eval .menubar.simulatemenu$m add $i
    }

    # Trace menu
    foreach i {
      {command -command messageWindows -label {Message Sending...} -underline 8}
      {separator}
      {command -command clearWindows -label {Clear Main Window} -underline 0}
    } {
      eval .menubar.tracemenu$m add $i
    }

    # Inspect menu
    # The following two items have been replaced by the new Find/inspect objects... dialog:
    #  {command -command inspect_anyobject -label {From list of all objects...} -underline 0}
    #  {command -command inspect_matching -label {By pattern matching...} -underline 3}
    foreach i {
      {command -command inspectSystemModule -label {Network} -underline 0}
      {command -command inspectMessageQueue -label {Scheduled Events (FES)} -underline 0}
      {command -command inspectSimulation   -label {Simulation} -underline 1}
      {separator}
      {cascade -label {Available Components} -underline 10 -menu .menubar.inspectmenu$m.components}
      {separator}
      {command -command inspectFilteredObjectList -label {Show 'Find/Inspect Objects' Window} -accel {Ctrl+S} -underline 0}
      {command -command inspectBypointer -label {Inspect by Pointer...} -underline 4}
      {separator}
      {command -command opp_updateinspectors -label {Refresh Inspectors} -underline 0}
    } {
      eval .menubar.inspectmenu$m add $i
    }

    # Inspect|Components menu
    menu .menubar.inspectmenu$m.components -tearoff 0
    foreach i {
      {command -command inspectComponentTypes -label {NED Component Types} -underline 0}
      {command -command inspectClasses        -label {Registered Classes} -underline 0}
      {command -command inspectFunctions      -label {NED Functions} -underline 4}
      {command -command inspectEnums          -label {Registered Enums} -underline 11}
      {command -command inspectConfigEntries  -label {Supported Configuration Options} -underline 0}
    } {
      eval .menubar.inspectmenu$m.components add $i
    }

    # View menu
    foreach i {
      {command -label {Ini File} -underline 0 -command viewIniFile}
      {command -label {README} -underline 0 -command {viewFile README}}
      {separator}
      {command -label {Output Vector File} -underline 7 -command viewOutputVectorFile}
      {command -label {Output Scalar File} -underline 7 -command viewOutputScalarFile}
      {command -label {Snapshot File} -underline 0 -command viewSnapshotFile}
      {separator}
      {command -label {View Text File...} -underline 0 -command {editTextFile}}
    } {
      eval .menubar.viewmenu$m add $i
    }

    # Options menu
    foreach i {
      {command -command simulationOptions -label {Simulation Options...} -underline 0}
      {command -command toggleTimeline -label {Show/Hide Timeline} -underline 10}
      {command -command toggleTreeView -label {Show/Hide Object Tree} -underline 1}
      {command -command toggleRecordEventlog -label {Eventlog Recording} -underline 10}
      {separator}
      {command -label {Load Config...} -underline 0 -command loadTkenvConfig}
      {command -label {Save Config...} -underline 1 -command saveTkenvConfig}
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

    #TODO  {rebuild  -image $icons(rebuild) -command {rebuild}}

    foreach i {
      {sep00    -separator}
      {newrun   -image $icons(newrun)  -command {newRun}}
      {loadned  -image $icons(loadned) -command {loadNedFile}}
      {sep5     -separator}
      {copy     -image $icons(copy)    -command {editCopy}}
      {find     -image $icons(find)    -command {editFind}}
      {save     -image $icons(save)    -command {saveFile "."}}
      {sep0     -separator}
      {step     -image $icons(step)    -command {oneStep}}
      {run      -image $icons(run)     -command {runNormal}}
      {fastrun  -image $icons(fast)    -command {runFast}}
      {exprrun  -image $icons(express) -command {runExpress}}
      {until    -image $icons(until)   -command {runUntil}}
      {stop     -image $icons(stop)    -command {stopSimulation}}
      {sep4     -separator}
      {eventlog -image $icons(recordeventlog) -command {toggleRecordEventlog}}
      {finish   -image $icons(finish)  -command {callFinish}}
      {sep02     -separator}
      {network  -image $icons(network) -command {inspectSystemModule}}
      {objs     -image $icons(findobj) -command {inspectFilteredObjectList}}
      {filter   -image $icons(filter)  -command {editFilterWindowContents}}
      {sep6     -separator}
      {tline    -image $icons(fes)     -command {toggleTimeline}}
      {tree     -image $icons(tree)    -command {toggleTreeView}}
      {sep9     -separator}
      {options  -image $icons(config)  -command {simulationOptions}}
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
    set help_tips(.toolbar.eventlog) {Eventlog recording on/off}
    set help_tips(.toolbar.finish)  {Call finish()}
    set help_tips(.toolbar.network) {Inspect network}
    set help_tips(.toolbar.objs)    {Find and inspect modules, messages, queues and other objects (Ctrl+S)}
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
    bind .timeline <Configure> "redrawTimeline"
    .timeline bind msg <Double-1> "timeline:dblClick .timeline"
    .timeline bind msgname <Double-1> "timeline:dblClick .timeline"
    .timeline bind msg <$B3> "timeline:rightClick .timeline %X %Y %x %y"
    .timeline bind msgname <$B3> "timeline:rightClick .timeline %X %Y %x %y"
    bind .timeline <Button-$B3> {timeline:popup %x %y %X %Y}

    set widgets(timeline) .timeline

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
        .toolbar.tree config -relief flat
    } else {
        .toolbar.tree config -relief sunken
    }

    if {$config(display-timeline)==0} {
        .toolbar.tline config -relief flat
    } else {
        .toolbar.tline config -relief sunken
    }

    #
    # Create main text window
    #
    text .main.text -yscrollcommand ".main.sb set" -font $fonts(text)
    scrollbar .main.sb -command ".main.text yview"
    logTextWidget:configureTags .main.text

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
    bindCommandsToTextWidget .main.text mainwindow
    bindRunCommands .
    bindOtherCommands .
}

proc bindRunCommands {w} {
    # Note: the "after 100" in the commands below is a workaround on Mac OS X:
    # without them, a few seconds after hitting e.g. F5 (Run) the app will
    # stop responding to UI events (beach ball appears and never goes away).
    # It doesn't hurt on other platforms.
    bind $w <F4> {after 100 oneStep}
    bind $w <F5> {after 100 runNormal}
    bind $w <F6> {after 100 runFast}
    bind $w <F7> {after 100 runExpress}
    bind $w <F8> {stopSimulation}
    bind $w <Control-F9> {debugNextEvent}
}

proc bindOtherCommands {w} {
    bind $w <Control-s> [list inspectFilteredObjectList $w]
    bind $w <Control-S> [list inspectFilteredObjectList $w]
}

# note: modptr may be "systemmodule" or a pointer; "" means no module
proc bindCommandsToTextWidget {txt {wintype ""}} {
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
    if {$wintype=="modulewindow"} {
        # bind Ctrl+H ('break' is needed because originally ^H is bound to DEL)
        set w [winfo toplevel $txt]
        bind $txt <Control-h> "moduleWindow:openFilterDialog %W; break"
        bind $txt <Control-H> "moduleWindow:openFilterDialog %W; break"
    }
    if {$wintype=="mainwindow"} {
        # bind Ctrl+H ('break' is needed because originally ^H is bound to DEL)
        bind $txt <Control-h> "mainlogWindow:openFilterDialog; break"
        bind $txt <Control-H> "mainlogWindow:openFilterDialog; break"
    }

    # bind Ctrl+A "Select all" ('break' is needed below because ^A=Home)
    bind $txt <Control-a> {%W tag add sel 1.0 end; break}

    # bind a context menu as well
    catch {$txt config -wrap $config(editor-wrap)}
    bind $txt <Button-$B3> [list textwidget:contextMenu %W $wintype %X %Y]
}

#===================================================================
#    LOAD BITMAPS
#===================================================================

set bitmap_ctr 0

proc loadBitmaps {path} {
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
           doLoadBitmaps $dir ""
           puts ""
       }
   }

   if {$bitmap_ctr==0} {
       puts "*** no images (gif) in $path"
   }

   puts ""
}

proc doLoadBitmaps {dir prefix} {
   global bitmaps bitmap_ctr

   #debug "entering $dir"
   set files [concat [glob -nocomplain -- [file join $dir {*.gif}]] \
                     [glob -nocomplain -- [file join $dir {*.png}]]]

   # load bitmaps from this directory
   set numTotal 0
   set numLoaded 0
   foreach f $files {
      set name [file tail [file rootname $f]]
      set img "i[incr bitmap_ctr]$name"
      if [catch {
         image create photo $img -file $f
         fixupImageIfNeeded $img
         set size "n" ;#default
         regexp -- {^(.*)_(vl|xl|l|n|s|vs|xs)$} $name dummy name size
         set loaded [doAddBitmap $img $prefix $name $size]
         if {$loaded} {incr numLoaded}
         incr numTotal
      } err] {
         puts -nonewline "(*** cannot load $f: $err ***) "
      }
   }
   if {$numLoaded==$numTotal} {
      puts -nonewline "$prefix*: $numTotal  "
   } else {
      puts -nonewline "$prefix*: $numLoaded (skipped [expr $numTotal-$numLoaded])  "
   }

   # recurse into subdirs
   foreach f [glob -nocomplain -- [file join $dir {*}]] {
      if {[file isdirectory $f] && [file tail $f]!="CVS" && [file tail $f]!=".svn"} {
         doLoadBitmaps "$f" "$prefix[file tail $f]/"
      }
   }
}

# On recent versions of OS X, images with partial transparency (alpha not 0 or 255)
# are not displayed at all. Workaround: replace all alpha values with 0 or 255.
proc fixupImageIfNeeded {img} {
   if {[string equal [tk windowingsystem] aqua]} {
       opp_reducealpha $img 128
   }
}

# register loaded image
proc doAddBitmap {img prefix name size} {
   global bitmaps

   # skip if already exists
   if [info exists bitmaps($prefix$name,$size)] {
       image delete $img
       return 0
   }

   # access via the s= display string option
   set bitmaps($prefix$name,$size) $img

   # access by the legacy way
   if {$size=="n"} {
       set bitmaps($prefix$name) $img
   } else {
       set bitmaps($prefix${name}_$size) $img
   }
   return 1
}

#===================================================================
#    LOAD PLUGINS
#===================================================================

proc loadPlugins {path} {
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
                 set name [file tail $file]
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
                 set name [file tail $file]
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

proc genericBindings {} {
    global help_tips
    set help_tips(helptip_proc) getHelpTip
    set help_tips(width) 500

    bind Button <Return> {%W invoke}
}

proc startupCommands {} {
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
        reflectRecordEventlog
        if [opp_isnotnull [opp_object_systemmodule]] {
            opp_inspect [opp_object_systemmodule] "(default)"
            notifyPlugins newNetwork
        }
    } else {
        # ask the user to select a network or a config
        if {$confignames=={} || $confignames=={General}} {
            newNetwork
        } else {
            newRun
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

proc isTkAtLeast {version} {
    global tk_patchLevel

    # must use dictionary comparison due to the embedded numbers
    if {[strdictcmp $tk_patchLevel $version] < 0} {
        return 0
    } else {
        return 1
    }
}

proc strdictcmp {s1 s2} {
    if {$s1 eq $s2} {
        return 0
    } else {
        # there's no dictionary comparison in Tcl, only dictionary sort...
        set ordered [lsort -dictionary [list $s1 $s2]]
        if {[lindex $ordered 0] eq $s1} {
            return -1
        } else {
            return 1
        }
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
