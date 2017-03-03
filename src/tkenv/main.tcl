#=================================================================
#  MAIN.TCL - part of
#
#                     OMNeT++/OMNEST
#            Discrete System Simulation in C++
#
#=================================================================

#----------------------------------------------------------------#
#  Copyright (C) 1992-2017 Andras Varga
#
#  This file is distributed WITHOUT ANY WARRANTY. See the file
#  `license' for details on this and other legal matters.
#----------------------------------------------------------------#


# default config settings
set config(keep-inspectors-on-top) 1
set config(reuse-inspectors) 1
set config(editor-findstring) ""
set config(editor-case-sensitive) 0
set config(editor-whole-words) 0
set config(editor-regexp) 0
set config(editor-backwards) 1
set config(editor-wrap) none
set config(editor-showprefix) 1
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
set config(display-statusdetails) 1
set config(display-timeline) 1
set config(timeline-maxnumevents) 1000
set config(timeline-wantevents) 1
set config(timeline-wantselfmsgs) 1
set config(timeline-wantnonselfmsgs) 1
set config(timeline-wantsilentmsgs) 0
set config(log-save-filename) "omnetpp.out"
set config(mainwin-state) "normal"
set config(mainwin-geom) ""
set config(mainwin-main-sashpos) {}
set config(mainwin-left-sashpos) {}
set config(mainwin-right-sashpos-horizontal) {}
set config(mainwin-right-sashpos-vertical) {}
set config(mainwin-sash-orient) vertical
set config(mainwin-log-mode) messages
set config(new-loginspector-mode) messages
set config(concurrent-anim) 1
set config(zoomby-factor) 1.3
set config(layout-may-resize-window) 1
set config(layout-may-change-zoom) 1
set config(preferences-dialog-page) "g"
set config(default-configname) {}
set config(default-runnumber) {}
set config(stopdialog:size) {188 148}
set config(finddialog:size) {316 132}
set config(findobjectsdialog:size) {925 578}
set config(preferencesdialog:size) {652 545}
set config(runseldialog:size) {421 146}
set config(treedialog:size) {353 446}
set config(layersdialog:size) {400 400}
set config(outvectoroptionsdialog:size) {300 300}
set config(rununtildialog:size) {455 206}
set config(inputbox:size) {400 200}
set config(combodialog:size) {400 200}
set config(oncedialog:size) {500 300}
set config(aboutdialog:size) {448 278}
set config(modelinfodialog:size) {400 400}

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
    global icons help_tips widgets config priv
    global B2 B3

    wm focusmodel . passive
    wm geometry . 1020x650
    wm minsize . 1 1
    wm overrideredirect . 0
    wm resizable . 1 1
    wm deiconify .
    wm title . "OMNeT++/Tkenv"
    wm protocol . WM_DELETE_WINDOW {exitOmnetpp}

    mainWindow:createMenu
    mainWindow:createToolbar
    mainWindow:createStatusbars
    mainWindow:createTimeline

    pack .toolbar -expand 0 -fill x -side top
    pack .nexteventbar -expand 0 -fill x -side top
    #pack .performancebar -expand 0 -fill x -side top
    pack .timeline -expand 0 -fill x -side top

    pack .statusbar -expand 0 -fill x -side bottom

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
      {command -command runUntil -label "Run Until..." -accel "$CTRL+F5" -underline 4}
      {separator}
      {command -command debugNextEvent -label "Debug Next Event" -accel "$CTRL+F9" -underline 1}
      {separator}
      {command -command stopSimulation -label "Stop Execution" -accel "F8" -underline 1}
      {separator}
      {command -command toggleRecordEventlog -label "Start/Stop Eventlog Recording" -accel "$CTRL+G" -underline 1}
      {separator}
      {command -command callFinish -label "Conclude Simulation" -underline 0}
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
      {separator}
      {command -command mainWindow:toggleLayout -label "Flip Window Layout" -underline 0}
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

    ttk::frame .toolbar

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
      {vert     -image $icons(vertical)   -command {mainWindow:setLayout vertical}}
      {horiz    -image $icons(horizontal) -command {mainWindow:setLayout horizontal}}
      {tline    -image $icons(fes)     -command {toggleTimeline}}
      {sep9     -separator}
      {options  -image $icons(config)  -command {preferences}}
      {sep10    -separator}
    } {
      eval packToolbutton .toolbar.$i
    }
    animControl .toolbar.animspeed
    pack .toolbar.animspeed -anchor c -expand 0 -fill none -side left -padx 5 -pady 0

    ttk::label .simtimelabel -relief groove -font SimtimeFont -text "n/a" -width 20  -padding {10 -1 10 -1} -background #ffffe0
    pack .simtimelabel -in .toolbar -anchor c -expand 0 -fill y -side right -padx 0 -pady 0

    ttk::label .eventnumlabel -relief groove -font EventnumFont -anchor e -text "   n/a" -padding {10 -1 10 -1} -background #ffffe0
    pack .eventnumlabel -in .toolbar -anchor c -expand 0 -fill y -side right -padx 0 -pady 0

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
    set help_tips(.toolbar.until)   "Run until time or event number (${CTRL_}F5)"
    set help_tips(.toolbar.stop)    "Stop running simulation (F8)"
    set help_tips(.toolbar.restart) "Rebuild network"
    set help_tips(.toolbar.eventlog) "Eventlog recording on/off (${CTRL_}G)"
    set help_tips(.toolbar.finish)  "Conclude simulation"
    set help_tips(.toolbar.objs)    "Find and inspect modules, messages, queues and other objects (${CTRL_}S)"
    set help_tips(.toolbar.vert)    "Vertical main window layout"
    set help_tips(.toolbar.horiz)   "Horizontal main window layout"
    set help_tips(.toolbar.tline)   "Show/hide timeline (${CTRL_}T)"
    set help_tips(.toolbar.options) "Preferences"
    set help_tips(.toolbar.animspeed) "Animation speed"
}

proc mainWindow:createStatusbars {} {
    global help_tips
    ttk::frame .nexteventbar
    ttk::frame .performancebar
    ttk::frame .statusbar

    ttk::label .nexteventlabel -relief groove -text "Next: n/a" -anchor w -width 40
    ttk::label .nextmodulelabel -relief groove -text "In: n/a" -anchor w  -width 60
    ttk::label .nexttimelabel -relief groove -text "At: 0s" -width 50 -anchor w
    pack .nexteventlabel .nextmodulelabel -in .nexteventbar -anchor n -expand 1 -fill x -side left
    pack .nexttimelabel -in .nexteventbar -anchor n -expand 0 -fill x -side left

    ttk::label .eventspersec -relief groove -text "Ev/sec: n/a" -anchor w -width 20
    ttk::label .simsecpersec -relief groove -text "Simsec/sec: n/a" -anchor w -width 20
    ttk::label .eventspersimsec -relief groove -text "Ev/simsec: n/a" -anchor w -width 20
    pack .eventspersec .simsecpersec .eventspersimsec -in .performancebar -anchor n -expand 1 -fill x -side left

    ttk::label .networklabel -relief groove -text "(No network set up)" -width 40 -anchor w
    ttk::label .msgstatslabel -relief groove -text "Msg stats: 0 scheduled / 0 existing / 0 created" -width 40 -anchor w
    pack .networklabel .msgstatslabel -in .statusbar -anchor n -expand 1 -fill x -side left

    set help_tips(.eventnumlabel)   "Event number of last event"
    set help_tips(.simtimelabel)    "Simulation time of last event"

    set help_tips(.nexttimelabel)   "Simulation time of next event"
    set help_tips(.nexteventlabel)  "Next simulation event"
    set help_tips(.nextmodulelabel) "Module where next event will occur"

    set help_tips(.eventspersec)    "Performance: events processed per second"
    set help_tips(.simsecpersec)    "Relative speed: simulated seconds processed per second"
    set help_tips(.eventspersimsec) "Event density: events per simulated second"

    set help_tips(.networklabel)    "Current inifile configuration, run number, network name, and potential scheduler information"
    set help_tips(.msgstatslabel)   "Number of events (messages) currently scheduled /\nNumber of existing message objects, including scheduled ones /\nTotal number of messages created since start of the simulation"
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

    canvas $f.tree -bg #ffffe0 -bd 0
    addScrollbars $f.tree

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
    toolbutton:setsunken .toolbar.tline $config(display-timeline)
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

    scale $w -orient horizontal -length 100 -sliderlength 12 -showvalue 0 -bd 1
    $w config -from .5 -to 3 -resolution 0.1 -variable priv(animspeed)

    trace variable priv(animspeed) w animSpeedChanged
}

proc mainWindow:setLayout {orient} {
    set oldOrient [.main.right cget -orient]
    if {$oldOrient!=$orient} {mainWindow:toggleLayout}
}

proc mainWindow:toggleLayout {} {
    global config

    set oldOrient [.main.right cget -orient]
    if {$oldOrient=="horizontal"} {set orient vertical} else {set orient horizontal}

    # store old sash position
    set config(mainwin-right-sashpos-$oldOrient) [panedwindow:getSashPosition .main.right]

    # switch to new $orient, and restore its sash position
    set config(mainwin-sash-orient) $orient
    .main.right config -orient $orient
    panedwindow:setSashPosition .main.right $config(mainwin-right-sashpos-$orient)

    toolbutton:setsunken .toolbar.vert  [expr {$orient=="vertical"}]
    toolbutton:setsunken .toolbar.horiz [expr {$orient!="vertical"}]
}

proc toggleStatusDetails {} {
    global config

    set config(display-statusdetails) [expr !$config(display-statusdetails)]
    if {!$config(display-statusdetails)} {
        pack forget .nexteventbar
        pack forget .performancebar
    }
    mainWindow:updateStatusDisplay
}

proc mainWindow:updateNetworkRunDisplay {} {
    set configname [opp_getstatusvar activeconfig]
    set runnumber [opp_getstatusvar activerunnumber]
    set network [opp_getstatusvar networktypename]
    set scheduler [opp_getobjectinfostring [opp_object_scheduler]]
    if {$configname==""} {set configName "n/a"}
    if {$network==""} {set network "(no network)"}
    if {$scheduler==""} {set sep ""} else {set sep " - "}
    .networklabel config -text "$configname #$runnumber: $network$sep$scheduler"
}

proc mainWindow:updateStatusDisplay {} {
    global config

    mainWindow:updateSimtimeDisplay

    if {$config(display-statusdetails)} {
        set state [opp_getsimulationstate]
        set runmode [opp_getrunmode]

        if {$state=="SIM_RUNNING" && ($runmode=="fast" || $runmode=="express")} {
            if {[winfo manager .performancebar]==""} {
                pack .performancebar -after .toolbar -expand 0 -fill x -side top
                pack forget .nexteventbar
            }
            mainWindow:updatePerformanceDisplay
        } else {
            if {[winfo manager .nexteventbar]==""} {
                pack .nexteventbar -after .toolbar -expand 0 -fill x -side top
                pack forget .performancebar
            }
            mainWindow:updateNextEventDisplay
        }
    }
}

proc strpadleft {str len} {
    set spaces [string repeat " " [expr max(0, $len-[string length $str])]]
    return "$spaces$str"
}

proc mainWindow:updateSimtimeDisplay {} {
    .eventnumlabel config -text [strpadleft "#[opp_getstatusvar eventnumber]" 6]
    .simtimelabel config -text "[opp_getstatusvar simtime]s"
    .msgstatslabel config -text "Msg stats: [opp_getstatusvar feslength] scheduled / [opp_getstatusvar livemsgcount] existing / [opp_getstatusvar totalmsgcount] created"
}

proc mainWindow:updateNextEventDisplay {} {
    set modptr [opp_null]
    set msgptr [opp_null]

    set state [opp_getsimulationstate]
    if {$state=="SIM_NEW" || $state=="SIM_READY" || $state=="SIM_RUNNING"} {
        set modptr [opp_getstatusvar guessnextmodule]
        set msgptr [opp_getstatusvar guessnextevent]
    }


    if [opp_isnotnull $msgptr] {
        .nexteventlabel config -text "Next: [opp_getobjectname $msgptr] ([opp_getobjectfield $msgptr className], id=[opp_getobjectid $msgptr])"
        .nexttimelabel config -text "At: [opp_getstatusvar guessnextsimtime]s (+[opp_getstatusvar timedelta]s)"
    } else {
        .nexteventlabel config -text "Next: n/a"
        .nexttimelabel config -text "At: n/a"
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
    bind $w <$Control-F5> {runUntil}
    bind $w <$Control-F9> {debugNextEvent}
    bind $w <$Control-g>  {toggleRecordEventlog}
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


