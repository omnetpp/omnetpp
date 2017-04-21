#=================================================================
#  DIALOGS.TCL - part of
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


proc inputbox {title prompt variable {checkboxlabel {}} {checkboxvar {}}} {
    # This procedure displays a dialog box, waits for a button in the dialog
    # to be invoked, then returns the index of the selected button.

    upvar $variable var

    set w .inputbox
    createOkCancelDialog $w $title

    ttk::label $w.f.l -text $prompt
    ttk::entry $w.f.e
    pack $w.f.l -anchor w -expand 0 -fill none -padx 2 -pady 2 -side top
    pack $w.f.e -anchor w -expand 1 -fill x -padx 2 -pady 2 -side top
    $w.f.e insert 0 $var
    $w.f.e selection range 0 end

    if {$checkboxlabel != ""} {
        global tmp
        upvar $checkboxvar cbvar
        set tmp(check) $cbvar
        ttk::checkbutton $w.f.c -text $checkboxlabel -variable tmp(check)
        pack $w.f.c -anchor w -expand 0 -fill x -padx 4 -pady 2 -side top
    }

    setInitialDialogFocus $w.f.e

    if [execOkCancelDialog $w] {
        set var [$w.f.e get]
        if {$checkboxlabel != ""} {
            set cbvar $tmp(check)
        }
        destroy $w
        return 1
    }
    destroy $w
    return 0
}

proc messagebox {title msg icon type} {
    return [tk_messageBox -title $title -message $msg -icon $icon -type $type]
}

proc comboSelectionDialog {title text label variable list} {
    set w .combodialog
    createOkCancelDialog $w $title

    upvar $variable var

    ttk::label $w.f.m -text $text -anchor w -justify left
    label-combo $w.f.c $label $list $var
    pack $w.f.m -fill x -padx 2 -pady 2 -side top
    pack $w.f.c -fill x -padx 2 -pady 2 -side top
    setInitialDialogFocus $w.f.c.e

    $w.f.c.e config -width 30

    if [execOkCancelDialog $w] {
        set var [$w.f.c.e get]
        destroy $w
        return 1
    }
    destroy $w
    return 0
}

set config(dontshow) {}

#
# "Did you know..." dialog. The text comes the hints() array;  state is saved
# into config(dontshow)
#
proc hintDialog {key} {
    global config tmp_once hints

    if {[lsearch -exact $config(dontshow) $key]!=-1} {
        # already shown
        return
    }

    if {![info exists hints($key)]} {
        debug "dbg: hintDialog: hint hints($key) not found"
        return
    }

    set text $hints($key)

    # create dialog with OK button
    set w .oncedialog
    createOkCancelDialog $w "Hint"
    destroy $w.buttons.cancelbutton

    text $w.f.text -relief solid -bd 1 -wrap word
    $w.f.text insert 1.0 $text
    $w.f.text config -state disabled
    ttk::checkbutton $w.f.x -text "Do not show this hint again" -variable tmp_once
    pack $w.f.x -expand 0 -fill x -side bottom
    pack $w.f.text -expand 1 -fill both -side top -padx 5 -pady 5

    set tmp_once 0

    # exec the dialog
    execOkCancelDialog $w
    if {$tmp_once} {
        lappend config(dontshow) $key
    }
    destroy $w
}


proc aboutDialog {} {
    global OMNETPP_RELEASE OMNETPP_EDITION OMNETPP_BUILDID

    set title "About OMNeT++"
    set text "
OMNeT++/OMNEST
Discrete Event Simulation Framework

(C) 1992-2017 OpenSim Ltd.
Release: $OMNETPP_RELEASE, build: $OMNETPP_BUILDID
$OMNETPP_EDITION

NO WARRANTY -- see license for details."

    set w .aboutdialog
    createOkCancelDialog $w $title

    $w.f config -border 2 -relief groove
    ttk::label $w.f.l -text "$text\n\n" -justify center -anchor c
    pack $w.f.l -expand 1 -fill both -side top -padx 30
    destroy $w.buttons.cancelbutton
    execOkCancelDialog $w
    destroy $w
}

#
# For selecting config and run number.
#
proc runSelectionDialog {} {
    global config

    set w .runseldialog
    createOkCancelDialog $w "Set Up Inifile Configuration"

    set result {}

    if {[catch {
        set sortedconfignames [runSelectionDialog:groupAndSortConfigNames]

        set configlist {}
        set isbase 0
        foreach name $sortedconfignames {
            if {$name == ""} {set isbase 1; continue}

            set desc [opp_getconfigdescription $name]
            set runs [opp_getnumrunsinconfig $name]
            # NOTE: if you change this, change proc runSelectionDialog:extractConfigName too
            #if {$isbase} {append name " -- base config"}
            if {$isbase} {set name "($name)"}
            if {$desc != ""} {append name " -- $desc"}
            if {$runs == 0}   {append name " (invalid config, generates 0 runs)"}
            if {$runs > 1}   {append name " (config with $runs runs)"}
            lappend configlist $name
        }

        set configname ""
        set runnumber 0

        # if last choice looks valid, use that as default
        if {[lcontains $sortedconfignames $config(default-configname)]} {
            #TODO also validate run number
            set configname $config(default-configname)
            set runnumber $config(default-runnumber)
        }
        if {$configname=="" && $configlist!={}} {
            set configname [lindex $configlist 0]
            set runnumber 0
        }

        ttk::label $w.f.m -anchor w -justify left -text "Set up one of the configurations defined in omnetpp.ini."
        label-combo $w.f.c "Config name:" $configlist $configname
        label-combo $w.f.c2 "Run number:" {} $runnumber
        pack $w.f.m -fill x -padx 2 -pady 2 -side top
        pack $w.f.c -fill x -padx 2 -pady 2 -side top
        pack $w.f.c2 -fill x -padx 2 -pady 2 -side top
        setInitialDialogFocus $w.f.c.e

        $w.f.c.e config -width 30
        $w.f.c2.e config -width 10

        bind $w.f.c.e <<ComboboxSelected>> [list runSelectionDialog:update $w]

        runSelectionDialog:update $w

        if [execOkCancelDialog $w] {
            set configname [runSelectionDialog:extractConfigName [$w.f.c.e get]]
            set runnumber  [$w.f.c2.e get]
            if ![string is integer $runnumber] {
                messagebox "Error" "Run number must be numeric." info ok
            } else {
                set config(default-configname) $configname
                set config(default-runnumber) $runnumber
                set result [list $configname $runnumber] ;# return is not accepted here
            }
        }

    } err]} {
        messagebox "Error" $err error ok
    }
    destroy $w
    return $result
}

proc runSelectionDialog:groupAndSortConfigNames {} {
    foreach c [opp_getconfignames] {
        foreach base [opp_getbaseconfigs $c] {
            set hasderivedconfig($base) 1
        }
    }

    set leaf {}
    set nonleaf {}
    foreach c [opp_getconfignames] {
        if [info exist hasderivedconfig($c)] {
            lappend nonleaf $c
        } else {
            lappend leaf $c
        }
    }

    return [concat $leaf {{}} $nonleaf]
}

proc runSelectionDialog:extractConfigName {s} {
    set s [regsub " -- .*\$" $s ""]
    set s [regsub " \\(.*\\)\$" $s ""]
    set s [regsub "^\\((.*)\\)\$" $s "\\1"]
    return $s
}

proc runSelectionDialog:update {w} {
    # fill run number combo with runs of the selected config
    set configname [runSelectionDialog:extractConfigName [$w.f.c.e get]]
    set n 0
    catch {set n [opp_getnumrunsinconfig $configname]}

    if {$n>1} {
        $w.f.c2.e config -state normal
    }

    set runs {}
    for {set i 0} {$i<$n} {incr i} {lappend runs $i}
    $w.f.c2.e configure -values $runs

    # ensure run number is in the valid range
    set runnumber [$w.f.c2.e get]
    if {![string is integer $runnumber] || $runnumber<0 || $runnumber>=$n} {
        $w.f.c2.e set 0
    }

    if {$n<=1} {
        $w.f.c2.e config -state disabled
    }
}

proc displayStopDialog {} {
    # Create a dialog that can be used to stop a running simulation
    global opp tmp

    # 1. Create and configure dialog
    set topwindow "."
    catch {set topwindow [winfo toplevel [focus]]}
    if {$topwindow=="" || $topwindow=="."} {
        set w .stopdialog
    } else {
        set w $topwindow.stopdialog
    }
    set tmp(stopdialog) $w

    if {[winfo exists $w]} return

    toplevel $w
    wm title $w "Running..."
    wm transient $w [winfo toplevel [winfo parent $w]]
    wm protocol $w WM_DELETE_WINDOW {opp_stopsimulation}
    restoreDialogGeometry $w

    set red #f83030
    button $w.stopbutton  -text "STOP!" -background $red -activebackground $red \
          -borderwidth 6 -font BIGFont -command {opp_stopsimulation}
    ttk::checkbutton $w.autoupdate -text "auto-update inspectors" -variable opp(autoupdate) -command "stopDialogAutoupdate $w"
    ttk::button $w.updatebutton  -text "  Update now  " -command {opp_callrefreshdisplay; opp_refreshinspectors}

    grid $w.stopbutton   -sticky news -padx 4 -pady 3
    grid $w.autoupdate   -sticky nes -padx 4 -pady 0
    grid $w.updatebutton -sticky nes -padx 4 -pady 3

    bind $w <Return> "opp_stopsimulation"
    bind $w <Escape> "opp_stopsimulation"
    bind $w <F8>     "opp_stopsimulation"

    set opp(autoupdate) [opp_getsimoption expressmode_autoupdate]
    stopDialogAutoupdate $w


    # Set a grab and claim the focus too.
    set opp(oldFocus) [focus]
    set opp(oldGrab) [grab current $w]
    grab $w
    focus $w.stopbutton
}

proc stopDialogAutoupdate {w} {
    global opp
    if {$opp(autoupdate)} {
        opp_setsimoption expressmode_autoupdate 1
        $w.updatebutton config -state disabled
    } else {
        opp_setsimoption expressmode_autoupdate 0
        $w.updatebutton config -state normal
    }
}

proc removeStopDialog {} {
    # Remove the dialog created by displayStopDialog

    global opp tmp
    if ![info exist tmp(stopdialog)] {
        return
    }

    set w $tmp(stopdialog)
    if {![winfo exists $w]} return

    # Restore the focus before deleting the window, since otherwise the
    # window manager may take the focus away so we can't redirect it.
    # Finally, restore any grab that was in effect.

    saveDialogGeometry $w
    catch {focus $opp(oldFocus)}
    destroy $w
    if {$opp(oldGrab) != ""} {
        grab $opp(oldGrab)
    }
}

proc preferencesDialog {parent {defaultpage ""}} {
    global opp config help_tips helptexts

    set parent [winfo toplevel $parent]
    if {$parent == "."} {
        set w .preferencesdialog
    } else {
        set w $parent.preferencesdialog
    }

    createOkCancelDialog $w "Preferences"

    ttk::notebook $w.f.nb
    set nb $w.f.nb
    $nb add [ttk::frame $nb.g] -text "General"
    $nb add [ttk::frame $nb.l] -text "Layouting"
    $nb add [ttk::frame $nb.a] -text "Animation"
    $nb add [ttk::frame $nb.t] -text "Filtering"
    $nb add [ttk::frame $nb.f] -text "Fonts"
    pack $nb -expand 1 -fill both

    if {$defaultpage==""} {set defaultpage $config(preferences-dialog-page)}

    $nb select $nb.$defaultpage

    # "General" page
    ttk::labelframe $nb.g.f0 -text "User Interface"
    ttk::checkbutton $nb.g.f0.keepontop -text "Keep inspector windows on top" -variable opp(keepontop)
    ttk::checkbutton $nb.g.f0.reuseinsp -text "Reuse inspectors if possible" -variable opp(reuseinsp)
    ttk::checkbutton $nb.g.f0.confirmexit -text "Confirm exit when simulation is in progress" -variable opp(confirmexit)
    label-combo $nb.g.f0.namespace "Hide namespaces from C++ class names:" {none omnetpp all} ""
    $nb.g.f0.namespace.l config -width 40
    pack $nb.g.f0.keepontop -anchor w
    pack $nb.g.f0.reuseinsp -anchor w
    pack $nb.g.f0.confirmexit -anchor w
    pack $nb.g.f0.namespace -anchor w

    ttk::labelframe $nb.g.f1 -text "Simulation Execution"
    label-entry $nb.g.f1.updfreq_fast    "Display update frequency for Fast Run (ms):"
    label-entry $nb.g.f1.updfreq_express "Display update frequency for Express Run (ms):"
    $nb.g.f1.updfreq_fast.l config -width 40
    $nb.g.f1.updfreq_express.l config -width 40
    pack $nb.g.f1.updfreq_fast -anchor w -fill x
    pack $nb.g.f1.updfreq_express -anchor w -fill x

    ttk::labelframe $nb.g.f2 -text "Logs"
    ttk::checkbutton $nb.g.f2.initbanners -text "Print initialization banners" -variable opp(initbanners)
    ttk::checkbutton $nb.g.f2.eventbanners -text "Print event banners" -variable opp(eventbanners)
    ttk::checkbutton $nb.g.f2.shortbanners -text "Short event banners" -variable opp(shortbanners)
    label-entry-help $nb.g.f2.logformat "Log prefix format:" $helptexts(logformat)
    label-combo $nb.g.f2.loglevel "Log level:" {FATAL ERROR WARN INFO DETAIL DEBUG TRACE} ""
    commentlabel $nb.g.f2.c0 "The above settings apply to subsequent simulation events. Hover mouse to get help on log prefix syntax."
    label-entry $nb.g.f2.numevents "Overall history size (in events):"
    label-entry $nb.g.f2.numlines "Scrollback buffer (lines):"
    commentlabel $nb.g.f2.c1 "Applies to main window and module log windows. Leave blank for unlimited. Minimum value is 500 lines."

    $nb.g.f2.numevents.l config -width 30
    $nb.g.f2.numlines.l config -width 30
    pack $nb.g.f2.initbanners -anchor w
    pack $nb.g.f2.eventbanners -anchor w
    pack $nb.g.f2.shortbanners -anchor w -padx 10
    pack $nb.g.f2.logformat -anchor w -fill x
    pack $nb.g.f2.loglevel -anchor w
    pack $nb.g.f2.c0 -anchor w
    pack $nb.g.f2.numevents -anchor w -fill x
    pack $nb.g.f2.numlines -anchor w -fill x
    pack $nb.g.f2.c1 -anchor w -fill x

    pack $nb.g.f0 -anchor center -expand 0 -fill x -ipadx 0 -ipady 0 -padx 10 -pady 5 -side top
    pack $nb.g.f2 -anchor center -expand 0 -fill x -ipadx 0 -ipady 0 -padx 10 -pady 5 -side top
    pack $nb.g.f1 -anchor center -expand 0 -fill x -ipadx 0 -ipady 0 -padx 10 -pady 5 -side top

    # "Layouting" page
    ttk::labelframe $nb.l.f1 -text "Layouting"
    ttk::label $nb.l.f1.layouterlabel -text "Layouting algorithm:"
    ttk::frame $nb.l.f1.layouter
    ttk::radiobutton $nb.l.f1.layouter.fast -text "Fast" -variable opp(layouterchoice) -value "fast"
    ttk::radiobutton $nb.l.f1.layouter.advanced -text "Advanced" -variable opp(layouterchoice) -value "advanced"
    ttk::radiobutton $nb.l.f1.layouter.auto -text "Adaptive (Fast for large networks, Advanced for smaller ones)" -variable opp(layouterchoice) -value "auto"
    ttk::checkbutton $nb.l.f1.layouting -text "Show layouting process" -variable opp(layouting)
    ttk::labelframe $nb.l.f2 -text "Display"
    ttk::checkbutton $nb.l.f2.arrangevectorconnections -text "Arrange connections on vector gates parallel to each other" -variable opp(arrangevectorconnections)
    ttk::checkbutton $nb.l.f2.allowresize -text "Resize window to fit network with current zoom level first" -variable opp(allowresize)
    ttk::checkbutton $nb.l.f2.allowzoom -text "Zoom out if necessary to fit network into window" -variable opp(allowzoom)
    label-entry $nb.l.f2.iconminsize  "Minimum icon size when zoomed out (pixels):"
    $nb.l.f2.iconminsize.l config -width 0

    pack $nb.l.f1.layouterlabel -anchor w
    pack $nb.l.f1.layouter -anchor w
    pack $nb.l.f1.layouter.fast -anchor w -padx 10
    pack $nb.l.f1.layouter.advanced -anchor w -padx 10
    pack $nb.l.f1.layouter.auto -anchor w -padx 10
    pack $nb.l.f1.layouting -anchor w
    pack $nb.l.f2.arrangevectorconnections -anchor w
    pack $nb.l.f2.iconminsize -anchor w -fill x
    pack $nb.l.f2.allowresize -anchor w
    pack $nb.l.f2.allowzoom -anchor w
    pack $nb.l.f1 -anchor center -expand 0 -fill x -ipadx 0 -ipady 0 -padx 10 -pady 5 -side top
    pack $nb.l.f2 -anchor center -expand 0 -fill x -ipadx 0 -ipady 0 -padx 10 -pady 5 -side top

    # "Filtering" page
    ttk::labelframe $nb.t.f1 -text "Timeline"
    ttk::checkbutton $nb.t.f1.tlwantself -text "Show self-messages" -variable opp(timeline-wantselfmsgs)
    ttk::checkbutton $nb.t.f1.tlwantnonself -text "Show non-self messages" -variable opp(timeline-wantnonselfmsgs)
    ttk::checkbutton $nb.t.f1.tlwantsilent -text "Show silent (non-animated) messages, see below" -variable opp(timeline-wantsilentmsgs)

    ttk::labelframe $nb.t.f2 -text "Animation"
    ttk::label $nb.t.f2.filterslabel -text "Messages to exclude from animation:"
    text $nb.t.f2.filterstext -height 10 -width 20
    commentlabel $nb.t.f2.c1 "One expression per line. Wildcards, AND, OR, NOT, numeric ranges ({5..10}), field matchers (className(..), kind(..), byteLength(..), etc.) are accepted."

    pack $nb.t.f1.tlwantself -anchor w
    pack $nb.t.f1.tlwantnonself -anchor w
    pack $nb.t.f1.tlwantsilent -anchor w
    pack $nb.t.f2.filterslabel -anchor w
    pack $nb.t.f2.filterstext -fill x -padx 8 -pady 2
    pack $nb.t.f2.c1 -anchor w -fill x

    pack $nb.t.f1 -anchor center -expand 0 -fill x -ipadx 0 -ipady 0 -padx 10 -pady 5 -side top
    pack $nb.t.f2 -anchor center -expand 0 -fill x -ipadx 0 -ipady 0 -padx 10 -pady 5 -side top

    # "Animation" page
    ttk::labelframe $nb.a.f1 -text "General"
    ttk::checkbutton $nb.a.f1.anim -text "Animate messages" -variable opp(anim)
    label-scale $nb.a.f1.speed "    Animation speed:"
    $nb.a.f1.speed.e config -length 200 -from 0 -to 3 -variable opp(speed)
    ttk::checkbutton $nb.a.f1.concanim -text "Broadcast animation" -variable opp(concanim)
    commentlabel $nb.a.f1.ca "Animates send/sendDirect calls concurrently, after processing each event (i.e. out of sequence)"
    ttk::checkbutton $nb.a.f1.nextev -text "Show next event markers" -variable opp(nextev)
    ttk::checkbutton $nb.a.f1.sdarrows -text "Show arrows for sendDirect animation" -variable opp(sdarrows)
    ttk::checkbutton $nb.a.f1.bubbles -text "Show bubbles (bubble() calls)" -variable opp(bubbles)
    ttk::checkbutton $nb.a.f1.animmeth -text "Animate method calls" -variable opp(animmeth)
    label-entry $nb.a.f1.methdelay "    Method call delay (ms):"
    $nb.a.f1.methdelay.l config -width 25
    ttk::labelframe $nb.a.f2 -text "Messages"
    ttk::checkbutton $nb.a.f2.msgnam -text "Display message names during animation" -variable opp(msgnam)
    ttk::checkbutton $nb.a.f2.msgclass -text "Display message class during animation" -variable opp(msgclass)
    ttk::checkbutton $nb.a.f2.msgcol -text "Color messages by message kind" -variable opp(msgcol)
    commentlabel $nb.a.f2.c "Color code (message kind modulo 8): 0=red 1=green 2=blue 3=white 4=yellow 5=cyan 6=magenta 7=black"
    ttk::checkbutton $nb.a.f2.penguin -text "Penguin mode" -variable opp(penguin)

    pack $nb.a.f1.anim -anchor w
    pack $nb.a.f1.speed -anchor w -expand 0 -fill x
    pack $nb.a.f1.concanim -anchor w
    pack $nb.a.f1.ca -anchor w -fill x
    pack $nb.a.f1.nextev -anchor w
    pack $nb.a.f1.sdarrows -anchor w
    pack $nb.a.f1.bubbles -anchor w
    pack $nb.a.f1.animmeth -anchor w
    pack $nb.a.f1.methdelay -anchor w -fill x
    pack $nb.a.f2.msgnam -anchor w
    pack $nb.a.f2.msgclass -anchor w
    pack $nb.a.f2.msgcol -anchor w
    pack $nb.a.f2.c -anchor w -fill x
    pack $nb.a.f2.penguin -anchor w

    pack $nb.a.f1 -anchor center -expand 0 -fill x -ipadx 0 -ipady 0 -padx 10 -pady 5 -side top
    pack $nb.a.f2 -anchor center -expand 0 -fill x -ipadx 0 -ipady 0 -padx 10 -pady 5 -side top

    # "Fonts" page
    ttk::labelframe $nb.f.f1 -text "Fonts"
    label-fontcombo $nb.f.f1.normalfont "User interface:" TkDefaultFont
    label-fontcombo $nb.f.f1.timelinefont "Timeline:" TimelineFont
    label-fontcombo $nb.f.f1.canvasfont "Canvas:" CanvasFont
    label-fontcombo $nb.f.f1.textfont  "Log windows:" LogFont
    label-fontcombo $nb.f.f1.simtimefont  "Simulation time:" SimtimeFont
    pack $nb.f.f1.normalfont -anchor w -fill x
    pack $nb.f.f1.timelinefont -anchor w -fill x
    pack $nb.f.f1.canvasfont -anchor w -fill x
    pack $nb.f.f1.textfont -anchor w -fill x
    pack $nb.f.f1.simtimefont -anchor w -fill x

    pack $nb.f.f1 -anchor center -expand 0 -fill x -ipadx 50 -ipady 0 -padx 10 -pady 5 -side top

    # Configure dialog
    $nb.g.f0.namespace.e delete 0 end
    $nb.g.f0.namespace.e insert 0 [opp_getsimoption stripnamespace]
    $nb.g.f1.updfreq_fast.e insert 0 [opp_getsimoption updatefreq_fast_ms]
    $nb.g.f1.updfreq_express.e insert 0 [opp_getsimoption updatefreq_express_ms]
    $nb.g.f2.numevents.e insert 0 [opp_getsimoption logbuffer_maxnumevents]
    $nb.g.f2.numlines.e insert 0 [opp_getsimoption scrollbacklimit]
    $nb.l.f2.iconminsize.e insert 0 [opp_getsimoption iconminsize]
    $nb.t.f2.filterstext insert 1.0 [opp_getsimoption silent_event_filters]
    set opp(eventbanners) [opp_getsimoption event_banners]
    set opp(initbanners) [opp_getsimoption init_banners]
    set opp(shortbanners) [opp_getsimoption short_banners]
    $nb.g.f2.logformat.e insert 0 [opp_getsimoption logformat]
    $nb.g.f2.loglevel.e delete 0 end
    $nb.g.f2.loglevel.e insert 0 [opp_getsimoption loglevel]
    set opp(anim)       [opp_getsimoption animation_enabled]
    set opp(concanim)   $config(concurrent-anim)
    set opp(nextev)     [opp_getsimoption nexteventmarkers]
    set opp(sdarrows)   [opp_getsimoption senddirect_arrows]
    set opp(animmeth)   [opp_getsimoption anim_methodcalls]
    $nb.a.f1.methdelay.e insert 0 [opp_getsimoption methodcalls_delay]
    set opp(msgnam)     [opp_getsimoption animation_msgnames]
    set opp(msgclass)   [opp_getsimoption animation_msgclassnames]
    set opp(msgcol)     [opp_getsimoption animation_msgcolors]
    set opp(penguin)    [opp_getsimoption penguin_mode]
    set opp(layouting)  [opp_getsimoption showlayouting]
    set opp(layouterchoice) [opp_getsimoption layouterchoice]
    set opp(arrangevectorconnections) [opp_getsimoption arrangevectorconnections]
    set opp(bubbles)    [opp_getsimoption bubbles]
    set opp(speed)      [opp_getsimoption animation_speed]
    set opp(keepontop)   $config(keep-inspectors-on-top)
    set opp(reuseinsp)   $config(reuse-inspectors)
    set opp(confirmexit) $config(confirm-exit)
    set opp(allowresize) $config(layout-may-resize-window)
    set opp(allowzoom)   $config(layout-may-change-zoom)

    set opp(timeline-wantselfmsgs)      $config(timeline-wantselfmsgs)
    set opp(timeline-wantnonselfmsgs)   $config(timeline-wantnonselfmsgs)
    set opp(timeline-wantsilentmsgs)    $config(timeline-wantsilentmsgs)

    setInitialDialogFocus $nb.a.f1.anim

    if [execOkCancelDialog $w] {
        opp_setsimoption updatefreq_fast_ms    [$nb.g.f1.updfreq_fast.e get]
        opp_setsimoption updatefreq_express_ms [$nb.g.f1.updfreq_express.e get]
        opp_setsimoption silent_event_filters  [$nb.t.f2.filterstext get 1.0 end]
        set n [$nb.g.f2.numevents.e get]
        if {$n=="" || [string is integer $n]} {
            if {$n!="" && $n<100} {set n 100}
            opp_setsimoption logbuffer_maxnumevents $n
        }
        set n [$nb.g.f2.numlines.e get]
        if {$n=="" || [string is integer $n]} {
            if {$n!="" && $n<500} {set n 500}
            opp_setsimoption scrollbacklimit $n
        }

        catch {opp_setsimoption stripnamespace [$nb.g.f0.namespace.e get] }
        opp_setsimoption event_banners       $opp(eventbanners)
        opp_setsimoption init_banners        $opp(initbanners)
        opp_setsimoption short_banners       $opp(shortbanners)
        opp_setsimoption logformat           [$nb.g.f2.logformat.e get]
        catch {opp_setsimoption loglevel     [$nb.g.f2.loglevel.e get] }
        opp_setsimoption animation_enabled   $opp(anim)
        set config(concurrent-anim)          $opp(concanim)
        opp_setsimoption nexteventmarkers    $opp(nextev)
        opp_setsimoption senddirect_arrows   $opp(sdarrows)
        opp_setsimoption anim_methodcalls    $opp(animmeth)
        opp_setsimoption methodcalls_delay   [$nb.a.f1.methdelay.e get]
        opp_setsimoption animation_msgnames  $opp(msgnam)
        opp_setsimoption animation_msgclassnames $opp(msgclass)
        opp_setsimoption animation_msgcolors $opp(msgcol)
        set old_iconminsize [opp_getsimoption iconminsize]
        opp_setsimoption iconminsize         [$nb.l.f2.iconminsize.e get]
        opp_setsimoption penguin_mode        $opp(penguin)
        opp_setsimoption showlayouting       $opp(layouting)
        opp_setsimoption layouterchoice      $opp(layouterchoice)
        opp_setsimoption arrangevectorconnections  $opp(arrangevectorconnections)
        opp_setsimoption bubbles             $opp(bubbles)
        opp_setsimoption animation_speed     $opp(speed)
        set config(keep-inspectors-on-top)   $opp(keepontop)
        set config(reuse-inspectors)         $opp(reuseinsp)
        set config(confirm-exit)             $opp(confirmexit)
        set config(layout-may-resize-window) $opp(allowresize)
        set config(layout-may-change-zoom)   $opp(allowzoom)

        set config(timeline-wantselfmsgs)    $opp(timeline-wantselfmsgs)
        set config(timeline-wantnonselfmsgs) $opp(timeline-wantnonselfmsgs)
        set config(timeline-wantsilentmsgs)  $opp(timeline-wantsilentmsgs)

        fontcombo:updatefont $nb.f.f1.normalfont TkDefaultFont
        fontcombo:updatefont $nb.f.f1.normalfont TkTooltipFont
        fontcombo:updatefont $nb.f.f1.normalfont TkTextFont
        fontcombo:updatefont $nb.f.f1.normalfont BoldFont
        #TODO: fontcombo:updatefont $nb.f.f1.normalfont BIGFont ----but keep size!!!
        fontcombo:updatefont $nb.f.f1.textfont LogFont
        fontcombo:updatefont $nb.f.f1.textfont TkFixedFont
        fontcombo:updatefont $nb.f.f1.timelinefont TimelineFont
        fontcombo:updatefont $nb.f.f1.canvasfont CanvasFont
        fontcombo:updatefont $nb.f.f1.simtimefont SimtimeFont
        fontcombo:updatefont $nb.f.f1.simtimefont EventnumFont
        updateTkpFonts

        reflectSettingsInGui
    }

    set config(preferences-dialog-page) [winfo name [$nb select]]

    destroy $w
}

proc setIfPatternIsValid {var pattern} {
    if [catch {opp_checkpattern $pattern} errmsg] {
        tk_messageBox -type ok -icon warning -title "Tkenv" -message "Filter pattern \"$pattern\" has invalid syntax -- setting unchanged."
    } else {
        uplevel [list set $var $pattern]
    }
}

proc runUntilDialog {time_var event_var msg_var mode_var} {

    global opp config tmp

    upvar $time_var time_var0
    upvar $event_var event_var0
    upvar $msg_var msg_var0
    upvar $mode_var mode_var0

    set w .rununtildialog
    createOkCancelDialog $w "Run Until"

    # collect FES messages for combo
    set msglabels {""}
    set msgptrs [opp_fesevents 1000 0 1 1 1]  ;# exclude non-message events
    foreach ptr $msgptrs {
        set msglabel "[opp_getobjectfullname $ptr] ([opp_getobjectshorttypename $ptr]), [opp_getobjectinfostring $ptr] -- $ptr"
        lappend msglabels $msglabel
    }

    # create and pack controls
    label-entry $w.f.time  "Simulation time to stop at:"
    label-entry $w.f.event "Event number to stop at:"
    label-combo $w.f.msg   "Message to stop at:" $msglabels
    label-check $w.f.stop  "" "Stop when message is cancelled" tmp(stop)
    label-combo $w.f.mode  "Running mode:"  {"Normal" "Fast (rare updates)" "Express (tracing off)"}

    foreach i {time event msg stop mode} {
       $w.f.$i.l configure -width 24
       pack $w.f.$i -anchor w -fill x
    }

    pack $w.f -anchor center -expand 1 -fill both -padx 10 -pady 10 -side top

    # restore last values
    set lastmsg $config(rununtil-msg)
    if {[lsearch -exact $msgptrs $lastmsg]==-1} {
        set msglabel ""  ;# saved msg pointer not currently scheduled, forget it (object may not even exist any more)
    } else {
        set msglabel "[opp_getobjectfullname $lastmsg] ([opp_getobjectshorttypename $lastmsg]), [opp_getobjectinfostring $lastmsg] -- $lastmsg"
    }

    $w.f.time.e insert 0 $config(rununtil-time)
    $w.f.event.e insert 0 $config(rununtil-event)
    $w.f.msg.e set $msglabel
    $w.f.mode.e set $config(rununtil-mode)
    set tmp(stop) [opp_getsimoption stoponmsgcancel]

    $w.f.time.e select range 0 end
    $w.f.event.e select range 0 end

    setInitialDialogFocus $w.f.time.e

    if [execOkCancelDialog $w] {
        set time_var0  [$w.f.time.e get]
        set event_var0 [$w.f.event.e get]
        set msg_var0   [lindex [$w.f.msg.e get] end]
        set mode_var0  [lindex [$w.f.mode.e get] 0]

        set config(rununtil-time)  $time_var0
        set config(rununtil-event) $event_var0
        set config(rununtil-msg)   $msg_var0
        set config(rununtil-mode)  [$w.f.mode.e get]
        opp_setsimoption stoponmsgcancel $tmp(stop)

        destroy $w
        return 1
    }
    destroy $w
    return 0
}



# findDialog --
#
proc findDialog {w} {

    global tmp config

    set tmp(case-sensitive)  $config(editor-case-sensitive)
    set tmp(whole-words)     $config(editor-whole-words)
    set tmp(regexp)          $config(editor-regexp)
    set tmp(backwards)       $config(editor-backwards)

    # dialog should be child of the window which contains the text widget
    set dlg [winfo toplevel $w].finddialog
    if {$dlg=="..finddialog"} {set dlg .finddialog}

    # create dialog
    createOkCancelDialog $dlg "Find" 1

    label-entry $dlg.f.find "Find string:"
    ttk::frame $dlg.f.opt
    pack $dlg.f.find -expand 0 -fill x
    pack $dlg.f.opt -expand 0 -fill none -anchor e

    # add entry fields
    ttk::checkbutton $dlg.f.opt.regexp -text "Regular expression" -underline 0 -variable tmp(regexp)
    ttk::checkbutton $dlg.f.opt.case -text "Case sensitive" -underline 0 -variable tmp(case-sensitive)
    ttk::checkbutton $dlg.f.opt.words -text "Whole words only" -underline 0 -variable tmp(whole-words)
    ttk::checkbutton $dlg.f.opt.backwards -text "Search backwards" -underline 7 -variable tmp(backwards)

    grid $dlg.f.opt.regexp $dlg.f.opt.case      -sticky nw
    grid $dlg.f.opt.words  $dlg.f.opt.backwards -sticky nw

    bind $dlg <Alt-r> [list $dlg.f.opt.regexp invoke]
    bind $dlg <Alt-R> [list $dlg.f.opt.regexp invoke]
    bind $dlg <Alt-c> [list $dlg.f.opt.case invoke]
    bind $dlg <Alt-C> [list $dlg.f.opt.case invoke]
    bind $dlg <Alt-w> [list $dlg.f.opt.words invoke]
    bind $dlg <Alt-W> [list $dlg.f.opt.words invoke]
    bind $dlg <Alt-b> [list $dlg.f.opt.backwards invoke]
    bind $dlg <Alt-B> [list $dlg.f.opt.backwards invoke]

    $dlg.f.find.e insert 0 $config(editor-findstring)
    $dlg.f.find.e select range 0 end

    setInitialDialogFocus $dlg.f.find.e

    # exec the dialog
    set result [execOkCancelDialog $dlg]

    set findstring [$dlg.f.find.e get]

    set case $tmp(case-sensitive)
    set words $tmp(whole-words)
    set regexp $tmp(regexp)
    set backwards $tmp(backwards)

    set config(editor-findstring) $findstring
    set config(editor-case-sensitive) $case
    set config(editor-whole-words) $words
    set config(editor-regexp) $regexp
    set config(editor-backwards) $backwards

    destroy $dlg

    if {$result == 1} {
        doFind $w $findstring $case $words $regexp $backwards
    }
}


# findNext --
#
# find next occurrence of the string in the editor window
#
proc findNext {w} {
    global config

    set findstring   $config(editor-findstring)
    set case         $config(editor-case-sensitive)
    set words        $config(editor-whole-words)
    set regexp       $config(editor-regexp)
    set backwards    $config(editor-backwards)

    doFind $w $findstring $case $words $regexp $backwards
}


# doFind --
#
#
proc doFind {w findstring case words regexp backwards} {
    if {[_doFind $w $findstring $case $words $regexp $backwards] == ""} {
        tk_messageBox -parent [winfo toplevel $w] -title "Find" -icon warning \
                      -type ok -message "'$findstring' not found."
    }
}

# _doFind --
#
# Internal proc for doFind and doReplace.
#
# Finds the given string, positions the cursor after its last char,
# and returns the length. Returns empty string if not found.
#
proc _doFind {w findstring case words regexp backwards} {

    # remove previous highlights
    $w tag remove SELECT 0.0 end

    # find the string
    set cur "insert"
    set initialcur $cur
    while 1 {
        # do search
        if {$backwards} {
            if {$case && $regexp} {
                set cur [$w search -count length -backwards -regexp -- $findstring $cur 1.0]
            } elseif {$case} {
                set cur [$w search -count length -backwards -- $findstring $cur 1.0]
            } elseif {$regexp} {
                set cur [$w search -count length -backwards -nocase -regexp -- $findstring $cur 1.0]
            } else {
                set cur [$w search -count length -backwards -nocase -- $findstring $cur 1.0]
            }
        } else {
            if {$case && $regexp} {
                set cur [$w search -count length -regexp -- $findstring $cur end]
            } elseif {$case} {
                set cur [$w search -count length -- $findstring $cur end]
            } elseif {$regexp} {
                set cur [$w search -count length -nocase -regexp -- $findstring $cur end]
            } else {
                set cur [$w search -count length -nocase -- $findstring $cur end]
            }
        }

        # exit if not found
        if {$cur == ""} {
            break
        }

        # allow exit loop only if we moved from initial cursor position
        if {![$w compare "$cur  + $length chars" == $initialcur]} {
            # if 'whole words' and we are not at beginning of a word, continue searching
            if {!$words} {
                break
            }
            if {[$w compare $cur == "$cur wordstart"] && \
                [$w compare "$cur + $length char" == "$cur wordend"]} {
                break
            }
        }

        # move cur so that we find next/prev occurrence
        if {$backwards} {
            set cur "$cur - 1 char"
        } else {
            set cur "$cur + 1 char"
        }
    }

    # check if found
    if {$cur == ""} {
        return ""
    }

    # highlight it and return length
    $w tag add SELECT $cur "$cur + $length chars"
    $w mark set insert "$cur + $length chars"
    $w see insert

    return $length
}

#
# Dialog to show/hide events in log windows. Returns an updated
# excludedModuleIds list; or a single 0 on cancel (0 is not a valid
# module Id.)
#
proc moduleOutputFilterDialog {rootmodule excludedModuleIds} {
    global tmp tmpExcludedModuleIds

    if {[networkPresent] == 0} {return 0}

    set title "Filter Window Contents"
    set msg "Select modules to show log messages from:"

    set w .treedialog
    createOkCancelDialog $w $title

    ttk::label $w.f.m -text $msg -anchor w -justify left
    ttk::frame $w.f.f -relief sunken
    pack $w.f.m -fill x -padx 10 -pady 5 -side top
    pack $w.f.f -expand 1 -fill both -padx 10 -pady 5 -side top

    # don't use sunken border due to rendering bug (checkboxes may erase part of the tree's border)
    canvas $w.f.f.c -width 300 -height 350 -bd 0 -bg white
    addScrollbars $w.f.f.c

    set tree $w.f.f.c
    set tmp(moduletreeroot) $rootmodule
    array unset tmpExcludedModuleIds
    foreach i $excludedModuleIds {set tmpExcludedModuleIds($i) 1}

    Tree:init $tree moduleOutputFilterDialog:getModuleTreeInfo
    Tree:open $tree $rootmodule
    Tree:readsubtreecheckboxstate $tree $rootmodule

    setInitialDialogFocus $tree

    if [execOkCancelDialog $w] {
        set excludedModuleIds {}
        foreach ptr [Tree:getcheckvars $tree] {
            set varname [Tree:getcheckvar $tree $ptr]
            upvar #0 $varname checkboxvar
            set isExcluded [expr !$checkboxvar]
            if {$isExcluded} {
                set moduleId [opp_getobjectid $ptr]
                lappend excludedModuleIds $moduleId
            }
        }
        array unset tmpExcludedModuleIds
        destroy $w
        return $excludedModuleIds
    }
    array unset tmpExcludedModuleIds
    destroy $w
    return 0
}

proc moduleOutputFilterDialog:getModuleTreeInfo {w op {key {}}} {
    global icons tmp tmpExcludedModuleIds

    set ptr $key
    switch $op {

      text {
        set id [opp_getobjectid $ptr]
        if {$id!=""} {set id " (id=$id)"}
        return "[opp_getobjectfullname $ptr] ([opp_getobjectshorttypename $ptr])$id"
      }

      needcheckbox {
        # we're going to say "yes", but initialize checkbox state first
        set varname [Tree:getcheckvar $w $ptr]
        upvar #0 $varname checkboxvar
        if {![info exist checkboxvar]} {
            set moduleId [opp_getobjectid $ptr]
            set isExcluded [info exist tmpExcludedModuleIds($moduleId)]
            set checkboxvar [expr !$isExcluded]
        }
        return 1
      }

      options {
        return ""
      }

      icon {
        #return [opp_getobjecticon $ptr]
        return ""
      }

      haschildren {
        if {$ptr=="treeroot"} {return 1}
        return [opp_hassubmodules $ptr]
      }

      children {
        if {$ptr=="treeroot"} {return $tmp(moduletreeroot)}
        return [opp_getsubmodules $ptr]
      }

      root {
        return "treeroot"
      }

      selectionchanged {
      }
    }
}




# filteredObjectList:window --
#
# Implements the "Find/inspect objects" dialog.
#
proc filteredObjectList:window {{ptr ""}} {
    global config tmp icons help_tips helptexts
    global B2 B3

    set w .findobjectsdialog

    if {$ptr=="" || [opp_isnull $ptr]} {
        set ptr [opp_object_simulation]
    }

    # if already exists, update the "search in" field and show it
    if {[winfo exists $w]} {
        $w.f.filter.searchin.e delete 0 end
        $w.f.filter.searchin.e insert 0 [opp_getobjectfullpath $ptr]
        showWindow $w  ;# black magic to raise the window
        return
    }

    # otherwise create
    createCloseDialog $w "Find/Inspect Objects" 1

    # stay on top
    if {$config(keep-inspectors-on-top)} {
        makeTransient $w
    }

    # vars
    set tmp(class)    $config(filtobjlist-class)
    set tmp(name)     $config(filtobjlist-name)
    set tmp(order)    $config(filtobjlist-order)
    set tmp(category) $config(filtobjlist-category)

    # two panels: $w.f.filter is the upper panel for filters, and
    # $w.f.main is the lower one with the listbox.

    # panel for filters
    ttk::frame $w.f.filter
    pack $w.f.filter -anchor center -expand 0 -fill x -side top

    #label $w.f.filter.title -text "Filter list of all objects in the simulation:" -justify left -anchor w
    #pack $w.f.filter.title -anchor w -expand 1 -fill x -side top

    label-entry $w.f.filter.searchin "Search inside:" [opp_getobjectfullpath $ptr]
    pack $w.f.filter.searchin -anchor w -expand 0 -fill x -side top

    ttk::labelframe $w.f.filter.pars -text "Search by class and object name:"
    pack $w.f.filter.pars -anchor center -expand 0 -fill x -side top
    set fp $w.f.filter.pars

    labelwithhelp $fp.classlabel "Class filter expression:" $helptexts(filterdialog-classnamepattern)
    labelwithhelp $fp.namelabel  "Object full path filter, e.g. \"*.queue\ AND not length(0)\":" $helptexts(filterdialog-namepattern)

    ttk::combobox $fp.classentry -values [concat {{}} [filteredObjectList:getClassNames]]
    catch {$fp.classentry current 0}
    $fp.classentry config -textvariable tmp(class)
    ttk::entry $fp.nameentry -textvariable tmp(name)

    set help_tips($fp.classentry) $helptexts(filterdialog-classnamepattern)
    set help_tips($fp.nameentry) $helptexts(filterdialog-namepattern)

    ttk::button $fp.refresh -text "Refresh" -width 10 -command "filteredObjectList:refresh $w"

    grid $fp.classlabel $fp.namelabel x -sticky ws -padx 5
    grid $fp.classentry $fp.nameentry $fp.refresh -sticky ew -padx 5 -pady 3
    grid columnconfig $fp 0 -weight 1
    grid columnconfig $fp 1 -weight 3

    # category filters
    ttk::labelframe $w.f.filter.cat -text "Object categories:"
    set cf $w.f.filter.cat
    ttk::checkbutton $cf.modules -text "Modules" -variable tmp(cat-m)
    ttk::checkbutton $cf.modpars -text "Parameters" -variable tmp(cat-p)
    ttk::checkbutton $cf.queues -text "Queues" -variable tmp(cat-q)
    ttk::checkbutton $cf.statistics -text "Statistics" -variable tmp(cat-s)
    ttk::checkbutton $cf.messages -text "Messages"  -variable tmp(cat-g)
    ttk::checkbutton $cf.chansgates -text "Gates, channels" -variable tmp(cat-c)
    ttk::checkbutton $cf.variables -text "Watches, FSMs"  -variable tmp(cat-v)
    ttk::checkbutton $cf.figures -text "Canvases, figures"  -variable tmp(cat-f)
    ttk::checkbutton $cf.other -text "Other" -variable tmp(cat-o)
    grid $cf.modules   $cf.modpars     $cf.queues     $cf.statistics $cf.other -sticky nw
    grid $cf.messages  $cf.chansgates  $cf.variables  $cf.figures              -sticky nw
    grid columnconfigure $cf 4 -weight 1
    pack $cf -anchor center -expand 0 -fill x -side top

    foreach {c} {m q p c s g v f o} {
        set tmp(cat-$c) [string match "*$c*" $tmp(category)]
    }

    # number of objects
    ttk::label $w.f.numobj -text "Found 0 objects" -justify left -anchor w
    pack $w.f.numobj -anchor w -expand 0 -fill x -side top

    # Listbox
    set lb [createInspectorListbox $w.f ""]
    $lb column name  -stretch 0 -width 350
    $lb column info  -stretch 0 -width 200
    inspectorListbox:restoreColumnWidths $lb "objdialog:columnwidths"

    # Configure dialog
    $w.buttons.closebutton config -command filteredObjectList:windowClose
    wm protocol $w WM_DELETE_WINDOW "$w.buttons.closebutton invoke"

    bind $fp.classentry <Return> "$fp.refresh invoke"
    bind $fp.nameentry <Return> "$fp.refresh invoke"
    bind $lb <Double-Button-1> "filteredObjectList:inspect $lb; after 500 \{raise $w; focus $lb\}"
    bind $lb <Key-Return> "filteredObjectList:inspect $lb; after 500 \{raise $w; focus $lb\}"
    bind $lb <Button-$B3> "+filteredObjectList:popup %X %Y $w"  ;# Note "+"! it appends this code to binding in widgets.tcl
    bind $w <Escape> "$w.buttons.closebutton invoke"
    bindRunCommands $w

    setInitialDialogFocus $fp.nameentry
}

#
# Closes Filtered object dialog
#
proc filteredObjectList:windowClose {} {
    global config tmp
    set w .findobjectsdialog

    set config(filtobjlist-class)    $tmp(class)
    set config(filtobjlist-name)     $tmp(name)
    set config(filtobjlist-order)    $tmp(order)
    set config(filtobjlist-category) $tmp(category)

    saveDialogGeometry $w

    set lb $w.f.main.list
    inspectorListbox:storeColumnWidths $lb "objdialog:columnwidths"

    destroy $w
}

# filteredObjectList:getClassNames --
#
# helper proc for filteredObjectList:window
#
proc filteredObjectList:getClassNames {} {
    set classes [opp_getchildobjects [opp_object_classes]]

    # get the names
    set classnames {}
    foreach classptr $classes {
        lappend classnames [opp_getobjectfullname $classptr]
    }
    # add classes that are not registered
    #lappend classnames ...

    return [lsort -dictionary $classnames]
}

# filteredObjectList:refresh --
#
# helper proc for filteredObjectList:window
#
proc filteredObjectList:refresh {w} {
    global config tmp
    global filtobjlist_state

    # resolve root object
    set rootobjectname [$w.f.filter.searchin.e get]
    set rootobject [opp_findobjectbyfullpath $rootobjectname]
    if [opp_isnull $rootobject] {
        tk_messageBox -title "Error" -icon error -type ok -parent $w \
            -message "Object to search in (\"$rootobjectname\") could not be resolved."
        return
    }

    set tmp(category) ""
    set categories {m q p c s g v f o}
    foreach {c} $categories {
        if {$tmp(cat-$c)} {set tmp(category) "$tmp(category)$c"}
    }
    if {[string length $tmp(category)]==[llength $categories]} {
        set tmp(category) "a$tmp(category)"
    }

    set class $tmp(class)
    if {$class==""} {set class "*"}
    if [catch {opp_checkpattern $class} errmsg] {
        tk_messageBox -parent $w -type ok -icon warning -title "Tkenv" -message "Class filter pattern \"$class\" has invalid syntax -- using \"*\" instead."
        set class "*"
    }

    set name $tmp(name)
    if {$name==""} {set name "*"}
    if [catch {opp_checkpattern $name} errmsg] {
        tk_messageBox -parent $w -type ok -icon warning -title "Tkenv" -message "Name filter pattern \"$name\" has invalid syntax -- using \"*\" instead."
        set name "*"
    }

    set order ""

    # get list
    set maxcount $config(filtobjlist-maxcount)
    if [catch {
        set objlist [opp_getsubobjectsfilt $rootobject $tmp(category) $class $name $maxcount $order]
    } err] {
        tk_messageBox -title "Error" -icon error -type ok -parent $w -message "Error: $err."
        set objlist {}
    }
    set num [llength $objlist]

    # ask user if too many...
    set viewall "ok"
    if {$num==$maxcount} {
        set viewall [tk_messageBox -title "Too many objects" -icon warning -type okcancel -parent $w \
        -message "Your query matched at least $num objects, click OK to display them."]
    }

    # clear listbox
    set lb $w.f.main.list
    ttkTreeview:deleteAll $lb

    # insert into listbox
    if {$viewall=="ok"} {
        if {$num==$maxcount} {
            $w.f.numobj config -text "The first $num objects found:"
        } else {
            $w.f.numobj config -text "Found $num objects:"
        }
        foreach ptr $objlist {
            set type [opp_getobjectshorttypename $ptr]
            set fullpath [opp_getobjectfullpath $ptr]
            set info [opp_getobjectinfostring $ptr]
            set icon [opp_getobjecticon $ptr]
            $lb insert {} end -image $icon -text "  $type" -values [list $fullpath $info $ptr]
        }
        set filtobjlist_state(outofdate) 0
    }
}

set filtobjlist_state(outofdate) 0

#
# Called from inspectorUpdateCallback whenever inspectors should be refereshed
#
proc filteredObjectList:inspectorUpdate {} {
    global filtobjlist_state
    set filtobjlist_state(outofdate) 1
}

proc filteredobjectlist_isnotsafetoinspect {} {
    global filtobjlist_state
    if {$filtobjlist_state(outofdate) || [isRunning]} {
        return 1
    }
    return 0
}

# filteredObjectList:popup --
#
# helper procedure for filteredObjectList:window -- creates popup menu
#
proc filteredObjectList:popup {X Y w} {
    set lb $w.f.main.list
    set ptr [inspectorListbox:getCurrent $lb]
    if [opp_isnull $ptr] return
    set insptypes [opp_supported_insp_types $ptr]

    set p $w.popup
    catch {destroy $p}
    menu $p -tearoff 0
    if {[filteredobjectlist_isnotsafetoinspect]} {set state "disabled"} else {set state "normal"}
    foreach type $insptypes {
       set label [getInspectMenuLabel $type]
       $p add command -label $label -state $state -command "opp_inspect $ptr \{$type\}; after 500 \{catch \{raise $w; focus $lb\}\}"
    }
    $p post $X $Y
}

proc filteredObjectList:inspect {lb} {
    set w .findobjectsdialog
    if {[filteredobjectlist_isnotsafetoinspect]} {
        if {[isRunning]} {
            set advice "please stop the simulation and click Refresh first"
        } else {
            set advice "please click Refresh first"
        }
        tk_messageBox -parent $w -icon info -type ok -title "Filtered object list" \
                      -message "Dialog contents might be out of date -- $advice."
        return
    }

    set ptr [inspectorListbox:getCurrent $lb]
    if [opp_isnotnull $ptr] {
        opp_inspect $ptr
    }
}

#----

set helptexts(logformat) {
Here are some of the format characters that you can use in the log prefix.
See the manual or the LogFormatter class for the complete list.

General:
  - %l, %c: log level (INFO, DEBUG, etc) and category string
  - %e, %t, %g: event number, simulation time, and fingerprint if enabled
  - %f, %i, %u: source file, line number, and function

Objects:
  - %E event object (class name, name)
  - %U module of current event (NED type, full path)
  - %C context component (NED type, full path)
  - %K context component, if different from event module (NED type, full path)
  - %J source component or object (NED type or class, full path or pointer)
  - %L source component or object, if different from context (NED type or class, full path or pointer)

Padding with spaces:
  - %[0-9]+ add spaces until specified column
  - %| adaptive tabstop: add padding until longest prefix seen so far
  - %> function call depth times 2-space indentation (see Enter_Method, Enter_Method_Silent)
}


set helptexts(timeline-namepattern) {
Generic filter expression which matches the object name by default.

Wildcards ("?", "*") are allowed. "{a-exz}" matches any character in the
range "a".."e", plus "x" and "z". You can match numbers: "job{128..191}"
will match "job128", "job129", ..., "job191". "job{128..}" and "job{..191}"
are also understood. You can combine patterns with AND, OR and NOT and
parentheses (lowercase and, or, not are also OK). You can match against
other object fields such as message length, message kind, etc., with the
syntax "fieldname(pattern)". Put quotation marks around a pattern if it
contains parentheses.

Examples:
 m*
            matches any object whose name begins with "m"
 m* AND *-{0..250}
            matches any object whose name begins with "m" and ends with dash
            and a number between 0 and 250
 not *timer*
            matches any object whose name doesn't contain the substring "timer"
 not (*timer* or *timeout*)
            matches any object whose name doesn't contain either "timer" or
            "timeout"
 kind(3) or kind({7..9})
            matches messages with message kind equal to 3, 7, 8 or 9
 className(IP*) and data-*
            matches objects whose class name begins with "IP" and name begins
            with "data-"
 not className(cMessage) and byteLength({1500..})
            matches objects whose class is not cMessage, and byteLength is
            at least 1500
 "or" or "and" or "not" or "*(*" or "msg(ACK)"
            quotation marks needed when pattern is a reserved word or contains
            parentheses. (Note: msg(ACK) without parens would be interpreted
            as some object having a "msg" attribute with the value "ACK"!)
}

set helptexts(timeline-classnamepattern) {
Generic filter expression which matches the class name by default.

Wildcards ("?", "*"), AND, OR, NOT and field matchers are accepted;
see Name Filter help for a more complete list.

Examples:
  PPPFrame
            matches objects whose class name is PPPFrame
  Ethernet*Frame or PPPFrame
            matches objects whose class name is PPPFrame or
            Ethernet(something)Frame
  not cMessage
            matches objects whose class name is not cMessage (so PPPFrame
            etc. are accepted)
  cMessage and kind(3)
            matches objects of class cMessage and message kind 3.
}

set helptexts(filterdialog-namepattern) {
Generic filter expression which matches the object full path by default.

Wildcards ("?", "*") are allowed. "{a-exz}" matches any character in the
range "a".."e", plus "x" and "z". You can match numbers: "*.job{128..191}"
will match objects named "job128", "job129", ..., "job191". "job{128..}"
and "job{..191}" are also understood. You can combine patterns with AND, OR
and NOT and parentheses (lowercase and, or, not are also OK). You can match
against other object fields such as queue length, message kind, etc., with
the syntax "fieldname(pattern)". Put quotation marks around a pattern if it
contains parentheses.

HINT: You'll want to start the pattern with "*." in most cases, to match
objects anywhere in the network!

Examples:
 *.destAddr
            matches all objects whose name is "destAddr" (likely module
            parameters)
 *.subnet2.*.destAddr
            matches objects named "destAddr" inside "subnet2"
 *.node[8..10].*
            matches anything inside module node[8], node[9] and node[10]
 className(cQueue) and not length(0)
            matches non-empty queue objects
 className(cQueue) and length({10..})
            matches queue objects with length>=10
 kind(3) or kind({7..9})
            matches messages with message kind equal to 3, 7, 8 or 9
            (Only messages have a "kind" attribute.)
 className(IP*) and *.data-*
            matches objects whose class name begins with "IP" and
            name begins with "data-"
 not className(cMessage) and byteLength({1500..})
            matches messages whose class is not cMessage, and byteLength is
            at least 1500. (Only messages have a "byteLength" attribute.)
 "*(*" or "*.msg(ACK)"
            quotation marks needed when pattern is a reserved word or contains
            parentheses. (Note: *.msg(ACK) without parens would be interpreted
            as some object having a "*.msg" attribute with the value "ACK"!)
}

set helptexts(filterdialog-classnamepattern) {
Generic filter expression which matches class name by default.

Wildcards ("?", "*"), AND, OR, NOT and field matchers are accepted;
see Object Filter help for a more complete list.

Examples:
  cQueue
            matches cQueue objects
  TCP* or (IP* and not IPDatagram)
            matches objects whose class name begins with TCP or IP,
            excluding IPDatagrams
  cMessage and kind(3)
            matches objects of class cMessage and message kind 3.
}

proc modelInfoDialog {{insp ""}} {
    if {[networkPresent] == 0} {return 0}

    if {$insp==""} {
        set modptr [opp_object_systemmodule]
    } else {
        set modptr [opp_inspector_getobject $insp]
    }
    if {$modptr==[opp_object_systemmodule]} {
        set what "Network"
    } else {
        set what "Module"
    }
    set modname [opp_getobjectfullpath $modptr]
    set typeptrs [opp_getcomponenttypes $modptr]

    set msg "$what \"$modname\" uses the following simple modules:\n\n"

    set unspec 0
    set inval 0
    set isapl [opp_isapl]
    foreach typeptr $typeptrs {
        set typename [opp_getobjectfullname $typeptr]
        set lc [opp_getobjectfield $typeptr lcprop]
        if {$lc=="omnetpp"} {
            append msg "  $typename   (part of OMNeT++)\n"
        } else {
            if {$isapl} {
                if {$lc==""} {
                    set lc "UNSPECIFIED*"; set unspec 1
                } elseif {[string first $lc "GPL LGPL BSD"]==-1} {
                    set lc "$lc - INVALID LICENSE*"; set inval 1
                }
            } else {
                if {$lc==""} {
                    set lc "unspecified*"; set unspec 1
                }
            }
            append msg "  $typename   (license: $lc)\n"
        }
    }
    if {$isapl && $unspec} {
        append msg "\nModule licenses may be declared in the package.ned file, with @license(<license>). OMNeT++ recognizes the following licenses: GPL, LGPL, BSD.\n"
    } elseif {$unspec} {
        append msg "\nModule licenses may be optionally declared in the package.ned file, with @license(<license>). OMNeT++ recognizes the following open-source licenses: GPL, LGPL, BSD. Other license codes (e.g. proprietary ones) are also accepted.\n"
    } elseif {$inval} {
        append msg "\nOMNeT++ recognizes the following license codes in @license(): GPL, LGPL, BSD. The commercial version OMNEST accepts any license declaration.\n"
    } else {
        append msg "\nModule licenses are declared in package.ned, with @license(<license>).\n"
    }

    set dlg $insp.modelinfodialog
    createOkCancelDialog $dlg "Model Information"
    message $dlg.f.txt -text $msg -width 400
    pack $dlg.f.txt -expand 1 -fill both
    destroy $dlg.buttons.cancelbutton
    execOkCancelDialog $dlg
    destroy $dlg
}
