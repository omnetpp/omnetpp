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
set config(keep-inspectors-on-top) 1
set config(reuse-inspectors) 1
set config(editor-findstring) ""
set config(editor-case-sensitive) 0
set config(editor-whole-words) 0
set config(editor-regexp) 0
set config(editor-backwards) 1
set config(editor-wrap) none
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
set config(display-statusdetails) 1
set config(display-timeline) 1
set config(timeline-maxnumevents) 1000
set config(timeline-wantselfmsgs) 1
set config(timeline-wantnonselfmsgs) 1
set config(timeline-wantsilentmsgs) 0
set config(log-save-filename) "omnetpp.out"
set config(mainwin-state) "normal"
set config(mainwin-geom) ""
set config(mainwin-main-sashpos) {}
set config(mainwin-left-sashpos) {}
set config(mainwin-right-sashpos) {}
set config(concurrent-anim) 1
set config(logwindow-scrollbacklines) 100000
set config(zoomby-factor) 1.1
set config(layout-may-resize-window) 1
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
    wm geometry . 1000x650
    wm minsize . 1 1
    wm overrideredirect . 0
    wm resizable . 1 1
    wm deiconify .
    wm title . "OMNeT++/Tkenv"
    wm protocol . WM_DELETE_WINDOW {exitOmnetpp}

    mainWindow:setApplicationIcon
    mainWindow:createMenu
    mainWindow:createToolbar
    mainWindow:createStatusbars
    mainWindow:createTimeline

    pack .toolbar -expand 0 -fill x -side top
    pack .statusbar -expand 0 -fill x -side top
    pack .statusbar2 -expand 0 -fill x -side top
    #pack .statusbar3 -expand 0 -fill x -side top
    pack .timeline -expand 0 -fill x -side top

    # Create main display area
    panedwindow .main -orient horizontal -sashrelief raised
    pack .main -expand 1 -fill both -side top

    panedwindow .main.left -orient vertical -sashrelief raised
    panedwindow .main.right -orient vertical -sashrelief raised
    .main add .main.left .main.right

    set treeview [mainWindow:createTreeView]
    set inspectorview [mainWindow:createInspectorView]
    .main.left add $treeview $inspectorview

    set networkview [mainWindow:createNetworkView]
    set logview [mainWindow:createLogView]
    .main.right add $networkview $logview

    mainWindow:refreshToolbar
    focus .log.main.text

    # Hotkeys
    bindRunCommands .
    bindOtherCommands .
}

proc mainWindow:setApplicationIcon {} {
    global tcl_platform icons

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
}

proc mainWindow:createMenu {} {
    global CTRL

    menu .menubar
    . config -menu .menubar

    # Create menus
    foreach i {
       {filemenu     -label "File" -underline 0}
       {simulatemenu -label "Simulate" -underline 0}
       {inspectmenu  -label "Inspect" -underline 0}
       {viewmenu     -label "View" -underline 0}
       {helpmenu     -label "Help" -underline 0}
    } {
       eval .menubar add cascade -menu .menubar.$i
       menu ".menubar.[lindex $i 0]" -tearoff 0
    }

    # File menu
    foreach i {
      {command -command newRun -label "Set Up a Configuration..." -underline 0}
      {separator}
      {command -command loadNedFile -label "Load NED File..." -underline 0}
      {command -command newNetwork -label "Set Up an Unconfigured Network..." -underline 10}
      {separator}
      {command -command viewIniFile -label "Open Primary Ini File" -underline 13}
      {command -command editTextFile -label "Open Text File..." -underline 0}
      {separator}
      {command -command createSnapshot -label "Create Snapshot..." -underline 0}
      {separator}
      {command -command preferences -label "Preferences..." -underline 0}
      {separator}
      {command -command exitOmnetpp -label "Exit" -accel "$CTRL+Q" -underline 1}
    } {
      eval .menubar.filemenu add $i
    }

    # Simulate menu
    #  {command -command init_step -label "First real event" -underline 1}
    #  {separator}
    foreach i {
      {command -command oneStep -label "One Step" -accel "F4" -underline 4}
      {separator}
      {command -command runNormal -label "Run"  -accel "F5" -underline 0}
      {command -command runFast -label "Fast Run (rare display updates)" -accel "F6" -underline 0}
      {command -command runExpress -label "Express Run (tracing off)" -accel "F7" -underline 1}
      {command -command runUntil -label "Run Until..." -underline 4}
      {separator}
      {command -command debugNextEvent -label "Debug Next Event" -accel "$CTRL+F9" -underline 1}
      {separator}
      {command -command stopSimulation -label "Stop Execution" -accel "F8" -underline 1}
      {separator}
      {command -command toggleRecordEventlog -label "Start/Stop Eventlog Recording" -underline 1}
      {separator}
      {command -command callFinish -label "Call finish() for All Modules" -underline 0}
      {command -command rebuild -label "Rebuild Network" -underline 2}
    } {
      eval .menubar.simulatemenu add $i
    }

    # Inspect menu
    # The following two items have been replaced by the new Find/inspect objects... dialog:
    #  {command -command inspect_anyobject -label "From list of all objects..." -underline 0}
    #  {command -command inspect_matching -label "By pattern matching..." -underline 3}
    foreach i {
      {command -command inspectSystemModule -label "Network" -underline 0}
      {command -command inspectMessageQueue -label "Scheduled Events (FES)" -underline 0}
      {command -command inspectSimulation   -label "Simulation" -underline 1}
      {separator}
      {cascade -label "Available Components" -underline 10 -menu .menubar.inspectmenu.components}
      {separator}
      {command -command inspectFilteredObjectList -label "Find/Inspect Objects..." -accel "$CTRL+S" -underline 0}
      {command -command inspectBypointer -label "Inspect by Pointer..." -underline 3}
    } {
      eval .menubar.inspectmenu add $i
    }

    # Inspect|Components menu
    menu .menubar.inspectmenu.components -tearoff 0
    foreach i {
      {command -command inspectComponentTypes -label "NED Component Types" -underline 0}
      {command -command inspectClasses        -label "Registered Classes" -underline 0}
      {command -command inspectFunctions      -label "NED Functions" -underline 4}
      {command -command inspectEnums          -label "Registered Enums" -underline 11}
      {command -command inspectConfigEntries  -label "Supported Configuration Options" -underline 0}
    } {
      eval .menubar.inspectmenu.components add $i
    }

    # View menu
    foreach i {
      {command -command toggleStatusDetails -label "Status Details" -accel "$CTRL+D" -underline 0}
      {command -command toggleTimeline -label "Timeline" -accel "$CTRL+T" -underline 0}
    } {
      eval .menubar.viewmenu add $i
    }

    # Help menu
    foreach i {
      {command -command aboutDialog -label "About OMNeT++/Tkenv" -underline 0}
    } {
      eval .menubar.helpmenu add $i
    }
}

proc mainWindow:createToolbar {} {
    global icons help_tips CTRL_

    frame .toolbar -relief raised -borderwidth 1

    #TODO  {rebuild  -image $icons(rebuild) -command {rebuild}}

    foreach i {
      {sep00    -separator}
      {newrun   -image $icons(newrun)  -command {newRun}}
      {loadned  -image $icons(loadned) -command {loadNedFile}}
      {sep5     -separator}
      {step     -image $icons(step)    -command {oneStep}}
      {run      -image $icons(run)     -command {runNormal}}
      {fastrun  -image $icons(fast)    -command {runFast}}
      {exprrun  -image $icons(express) -command {runExpress}}
      {until    -image $icons(until)   -command {runUntil}}
      {stop     -image $icons(stop)    -command {stopSimulation}}
      {sep4     -separator}
      {finish   -image $icons(finish)  -command {callFinish}}
      {sep7     -separator}
      {eventlog -image $icons(recordeventlog) -command {toggleRecordEventlog}}
      {sep3     -separator}
      {objs     -image $icons(findobj) -command {inspectFilteredObjectList}}
      {filter   -image $icons(filter)  -command {editFilterWindowContents}}
      {sep6     -separator}
      {tline    -image $icons(fes)     -command {toggleTimeline}}
      {sep9     -separator}
      {options  -image $icons(config)  -command {preferences}}
      {sep10    -separator}
    } {
      set b [eval iconbutton .toolbar.$i]
      pack $b -anchor n -expand 0 -fill none -side left -padx 0 -pady 2
    }
    animControl .toolbar.animspeed
    pack .toolbar.animspeed -anchor c -expand 0 -fill none -side left -padx 5 -pady 0

    set help_tips(.toolbar.loadned) "Load NED file for compound module definitions"
    set help_tips(.toolbar.newrun)  "Set up an inifile configuration"
    set help_tips(.toolbar.copy)    "Copy selected text to clipboard (${CTRL_}C)"
    set help_tips(.toolbar.find)    "Find string in main window (${CTRL_}F)"
    set help_tips(.toolbar.save)    "Save main window contents to file"
    set help_tips(.toolbar.filter)  "Filter main window contents (${CTRL_}H)"
    set help_tips(.toolbar.newnet)  "Set up a network"
    set help_tips(.toolbar.step)    "Execute one event (F4)"
    set help_tips(.toolbar.run)     "Run with full animation (F5)"
    set help_tips(.toolbar.fastrun) "Run faster: no animation and rare inspector updates (F6)"
    set help_tips(.toolbar.exprrun) "Run at full speed: no text output, animation or inspector updates (F7)"
    set help_tips(.toolbar.until)   "Run until time or event number"
    set help_tips(.toolbar.stop)    "Stop running simulation (F8)"
    set help_tips(.toolbar.restart) "Rebuild network"
    set help_tips(.toolbar.eventlog) "Eventlog recording on/off"
    set help_tips(.toolbar.finish)  "Call finish()"
    set help_tips(.toolbar.objs)    "Find and inspect modules, messages, queues and other objects (${CTRL_}S)"
    set help_tips(.toolbar.tline)   "Show/hide timeline (${CTRL_}T)"
    set help_tips(.toolbar.options) "Preferences"
    set help_tips(.toolbar.animspeed) "Animation speed"
}

proc mainWindow:createStatusbars {} {
    global fonts help_tips
    frame .statusbar
    frame .statusbar2
    frame .statusbar3

    label .networklabel -relief groove -text "(No network set up)" -width 40 -anchor w
    label .eventnumlabel -relief groove -text "Event #0" -width 15  -anchor w
    label .timelabel -relief groove -text "t=0" -width 20 -anchor w -font $fonts(bold)
    label .msgstatslabel -relief groove -text "Msg stats: 0 scheduled / 0 existing / 0 created" -width 40 -anchor w

    label .nexteventlabel -relief groove -text "Next: n/a" -anchor w -width 20
    label .nextmodulelabel -relief groove -text "In: n/a" -anchor w -width 20
    label .timedelta -relief groove -text "At: last event + 0s" -anchor w -width 20

    label .eventspersec -relief groove -text "Ev/sec: n/a" -anchor w -width 20
    label .simsecpersec -relief groove -text "Simsec/sec: n/a" -anchor w -width 20
    label .eventspersimsec -relief groove -text "Ev/simsec: n/a" -anchor w -width 20

    pack .networklabel .eventnumlabel .timelabel .msgstatslabel -in .statusbar -anchor n -expand 1 -fill x -side left
    #pack .feslength .totalmsgs .livemsgs -in .statusbar2 -anchor n -expand 1 -fill x -side left
    pack .nexteventlabel .nextmodulelabel .timedelta -in .statusbar2 -anchor n -expand 1 -fill x -side left
    pack .eventspersec .simsecpersec .eventspersimsec -in .statusbar3 -anchor n -expand 1 -fill x -side left

    set help_tips(.networklabel)    "Current inifile configuration, run number, and network name"
    set help_tips(.eventnumlabel)   "Sequence number of next event"
    set help_tips(.timelabel)       "Simulation time of next event"
    set help_tips(.msgstatslabel)   "Number of events (messages) currently scheduled /\nNumber of existing message objects, including scheduled ones /\nTotal number of messages created since start of the simulation"

    set help_tips(.nexteventlabel)  "The next simulation event"
    set help_tips(.nextmodulelabel) "Module where next event will occur"
    set help_tips(.timedelta)       "Time delta between the last executed and the next event"

    set help_tips(.eventspersec)    "Performance: events processed per second"
    set help_tips(.simsecpersec)    "Relative speed: simulated seconds processed per second"
    set help_tips(.eventspersimsec) "Event density: events per simulated second"
}

proc mainWindow:createTimeline {} {
    global widgets B1 B2 B3

    frame .timelineframe -borderwidth 2 -relief groove
    canvas .timeline -borderwidth 0
    pack .timeline -in .timelineframe -expand 1 -fill both

    bind .timeline <Configure> "redrawTimeline"
    .timeline bind msg <1> "timeline:click .timeline"
    .timeline bind msg <Double-1> "timeline:dblClick .timeline"
    .timeline bind msgname <Double-1> "timeline:dblClick .timeline"
    .timeline bind msg <$B3> "timeline:rightClick .timeline %X %Y %x %y"
    .timeline bind msgname <$B3> "timeline:rightClick .timeline %X %Y %x %y"
    bind .timeline <Button-$B3> {timeline:popup %x %y %X %Y}

    set widgets(timeline) .timeline
}

proc mainWindow:createTreeView {} {
    global widgets

    set f .main.mgr
    set widgets(manager) $f
    ttk::frame $f -relief flat -borderwidth 1

    canvas $f.tree -bg #ffffe0 -bd 0 -yscrollcommand "$f.sb set"
    ttk::scrollbar $f.sb -command "$f.tree yview"
    pack $f.sb -side right -expand 0 -fill y
    pack $f.tree -side left -expand 1 -fill both -padx 0 -pady 0 -ipadx 0 -ipady 0

    initTreeManager
    return $f
}

proc mainWindow:createInspectorView {} {
    set f .inspector
    ttk::frame $f -borderwidth 0
    createEmbeddedGenericObjectInspector $f
    return $f
}

proc mainWindow:createNetworkView {} {
    set insp .network
    ttk::frame $insp -borderwidth 0
    createEmbeddedModuleInspector $insp
    return $insp
}

proc mainWindow:createLogView {} {
    set insp .log
    ttk::frame $insp -borderwidth 0
    createEmbeddedLogInspector $insp
    return $insp
}

proc mainWindow:refreshToolbar {} {
    global config

    if {$config(display-timeline)==0} {set relief flat} else {set relief sunken}
    .toolbar.tline config -relief $relief
}

proc mainWindow:selectionChanged {insp obj} {
    if {$insp!=".inspector"} { # source is different from .inspector itself
        opp_inspector_setobject .inspector $obj
    }
}

proc mainWindow:networkViewInputChanged {obj} {
    opp_inspector_setobject .log $obj
}

proc animControl {w} {
    global priv

    scale $w -orient horizontal -length 50 -sliderlength 8 -showvalue 0 -bd 1
    $w config -from .5 -to 3 -resolution 0.01 -variable priv(animspeed)

    trace variable priv(animspeed) w animSpeedChanged
}

proc toggleStatusDetails {} {
    global config

    set config(display-statusdetails) [expr !$config(display-statusdetails)]
    if {!$config(display-statusdetails)} {
        pack forget .statusbar2
        pack forget .statusbar3
    }
    mainWindow:updateStatusDisplay
}

proc mainWindow:updateNetworkRunDisplay {} {
    set configname [opp_getstatusvar activeconfig]
    set runnumber [opp_getstatusvar activerunnumber]
    set network [opp_getstatusvar networktypename]
    if {$configname==""} {set configName "n/a"}
    if {$network==""} {set network "(no network)"}
    .networklabel config -text "$configname #$runnumber: $network"
}

proc mainWindow:updateStatusDisplay {} {
    global config

    mainWindow:updateSimtimeDisplay

    if {$config(display-statusdetails)} {
        set state [opp_getsimulationstate]
        set runmode [opp_getrunmode]

        if {$state=="SIM_RUNNING" && ($runmode=="fast" || $runmode=="express")} {
            if {[winfo manager .statusbar3]==""} {
                pack .statusbar3 -after .statusbar -expand 0 -fill x -side top
                pack forget .statusbar2
            }
            mainWindow:updatePerformanceDisplay
        } else {
            if {[winfo manager .statusbar2]==""} {
                pack .statusbar2 -after .statusbar -expand 0 -fill x -side top
                pack forget .statusbar3
            }
            mainWindow:updateNextModuleDisplay
        }
    }
}

proc mainWindow:updateSimtimeDisplay {} {
    .eventnumlabel config -text "Event #[opp_getstatusvar eventnumber]"
    .timelabel config -text "t=[opp_getstatusvar guessnextsimtime]s"
    .msgstatslabel config -text "Msg stats: [opp_getstatusvar feslength] scheduled / [opp_getstatusvar livemsgcount] existing / [opp_getstatusvar totalmsgcount] created"
}

proc mainWindow:updateNextModuleDisplay {} {
    set modptr [opp_null]
    set msgptr [opp_null]

    set state [opp_getsimulationstate]
    if {$state=="SIM_NEW" || $state=="SIM_READY" || $state=="SIM_RUNNING"} {
        set modptr [opp_getstatusvar guessnextmodule]
        set msgptr [opp_getstatusvar guessnextevent]
    }

    if [opp_isnotnull $msgptr] {
        .nexteventlabel config -text "Next: [opp_getobjectname $msgptr] ([opp_getobjectfield $msgptr className], id=[opp_getobjectid $msgptr])"
        .timedelta config -text "At: last event + [opp_getstatusvar timedelta]s"
    } else {
        .nexteventlabel config -text "Next: n/a"
        .timedelta config -text "At: n/a"
    }

    if [opp_isnotnull $modptr] {
        .nextmodulelabel config -text "In: [opp_getobjectfullpath $modptr] ([opp_getobjectshorttypename $modptr], id=[opp_getobjectid $modptr])"
    } else {
        .nextmodulelabel config -text "In: n/a"
    }
}

proc mainWindow:updatePerformanceDisplay {} {
    .simsecpersec config -text "Simsec/sec: [opp_getstatusvar simsecpersec]"
    .eventspersec config -text "Ev/sec: [opp_getstatusvar eventspersec]"
    .eventspersimsec config -text "Ev/simsec: [opp_getstatusvar eventspersimsec]"
}


#
# Called when simulation speed slider on toolbar changes
#
proc animSpeedChanged {arr name op} {
    if {($op!="w" && $op!="write") || $arr!="priv" || $name!="animspeed"} {error "wrong callback"}

    global priv
    opp_setsimoption "animation_speed" $priv(animspeed)
}

proc bindRunCommands {w} {
    global Control

    # Note: the "after 100" in the commands below is a workaround on Mac OS X:
    # without them, a few seconds after hitting e.g. F5 (Run) the app will
    # stop responding to UI events (beach ball appears and never goes away).
    # It doesn't hurt on other platforms.
    bind $w <F4> {after 100 oneStep}
    bind $w <F5> {after 100 runNormal}
    bind $w <F6> {after 100 runFast}
    bind $w <F7> {after 100 runExpress}
    bind $w <F8> {stopSimulation}
    bind $w <$Control-F9> {debugNextEvent}
    bind $w <$Control-d>  {toggleStatusDetails}
    bind $w <$Control-t>  {toggleTimeline}
    bind $w <$Control-q>  {exitOmnetpp}
}

proc bindOtherCommands {w {insp ""}} {
    global Control
    bind $w <$Control-s> [list inspectFilteredObjectList $insp]
}

proc bindCommandsToTextWidget {txt} {
    global config B2 B3 Control

    # bindings for find; 'break' is needed below because Ctrl+F is already bound
    bind $txt <Key> {%W tag remove SELECT 0.0 end}
    bind $txt <$Control-f> {findDialog %W; break}
    bind $txt <F3> {findNext %W}

    # bind Ctrl+A "Select all" ('break' is needed below because ^A=Home)
    bind $txt <$Control-a> {%W tag add sel 1.0 end; break}
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
        if {($confignames=="General" && $cfgnetworkname!="")} {
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
        if {$confignames=={} || $confignames=="General"} {
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
