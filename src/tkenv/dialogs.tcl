#=================================================================
#  DIALOGS.TCL - part of
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

#===================================================================
#    HELPER/GUI PROCEDURES
#===================================================================

proc inputbox {title prompt variable {checkboxlabel {}} {checkboxvar {}}} {
    # This procedure displays a dialog box, waits for a button in the dialog
    # to be invoked, then returns the index of the selected button.

    upvar $variable var

    set w .inputbox
    createOkCancelDialog $w $title

    label $w.f.l -text $prompt
    entry $w.f.e -highlightthickness 0
    pack $w.f.l -anchor w -expand 0 -fill none -padx 2 -pady 2 -side top
    pack $w.f.e -anchor w -expand 1 -fill x -padx 2 -pady 2 -side top
    $w.f.e insert 0 $var
    $w.f.e selection range 0 end

    if {$checkboxlabel != ""} {
        global tmp
        upvar $checkboxvar cbvar
        set tmp(check) $cbvar
        checkbutton $w.f.c -text $checkboxlabel -variable tmp(check)
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

    label $w.f.m -text $text -anchor w -justify left
    label-combo $w.f.c $label $list $var
    pack $w.f.m -fill x -padx 2 -pady 2 -side top
    pack $w.f.c -fill x -padx 2 -pady 2 -side top
    setInitialDialogFocus $w.f.c.e

    $w.f.c.e config -width 30

    if [execOkCancelDialog $w] {
        set var [$w.f.c.e cget -value]
        destroy $w
        return 1
    }
    destroy $w
    return 0
}

#
# For selecting config and run number.
#
proc runSelectionDialog {configname_var runnumber_var} {
    set w .runseldialog
    createOkCancelDialog $w "Set up an Inifile Configuration"

    upvar $configname_var configname
    upvar $runnumber_var  runnumber

    set ok 0

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

        if {($configname=="" || $configname=="General") && $configlist!={}} {
            set configname [lindex $configlist 0]
        }

        label $w.f.m -anchor w -justify left -text "Set up one of the configurations defined in omnetpp.ini."
        label-combo $w.f.c "Config name:" $configlist $configname
        label-combo $w.f.c2 "Run number:" {} $runnumber
        pack $w.f.m -fill x -padx 2 -pady 2 -side top
        pack $w.f.c -fill x -padx 2 -pady 2 -side top
        pack $w.f.c2 -fill x -padx 2 -pady 2 -side top
        setInitialDialogFocus $w.f.c.e

        $w.f.c.e config -width 30
        $w.f.c2.e config -width 10

        combo:onChange $w.f.c.e [list runSelectionDialog:update $w]

        runSelectionDialog:update $w

        if [execOkCancelDialog $w] {
            set configname [runSelectionDialog:extractConfigName [$w.f.c.e cget -value]]
            set runnumber  [$w.f.c2.e cget -value]
            if ![string is integer $runnumber] {
                messagebox "Error" "Run number must be numeric." info ok
                set runnumber 0
            } else {
                set ok 1
            }
        }

    } err]} {
        messagebox "Error" $err error ok
    }
    destroy $w
    return $ok
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
    set configname [runSelectionDialog:extractConfigName [$w.f.c.e cget -value]]
    set n 0
    catch {set n [opp_getnumrunsinconfig $configname]}

    if {$n>1} {
        $w.f.c2.e config -state normal
    }

    $w.f.c2.e list delete 0 end
    for {set i 0} {$i<$n} {incr i} {
        $w.f.c2.e list insert end $i
    }

    # ensure run number is in the valid range
    set runnumber  [$w.f.c2.e cget -value]
    if {$n<=1} {
        $w.f.c2.e config -value ""
    }
    if {![string is integer $runnumber] || $runnumber<0 || $runnumber>=$n} {
        $w.f.c2.e config -value "0"
    }

    if {$n<=1} {
        $w.f.c2.e config -state disabled
    }
}

proc displayStopDialog {} {
    # Create a dialog that can be used to stop a running simulation
    global opp fonts tmp

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
    wm title $w {Running...}
    wm transient $w [winfo toplevel [winfo parent $w]]
    wm protocol $w WM_DELETE_WINDOW {opp_stopsimulation}
    # bind $w <Visibility> "raise $w"  ;# Keep modal window on top -- not good! (obscures error dialogs)

    set red #f83030
    button $w.stopbutton  -text "STOP!" -background $red -activebackground $red \
          -borderwidth 6 -font $fonts(big) -command {opp_stopsimulation}
    checkbutton $w.autoupdate -text "auto-update inspectors" -variable opp(autoupdate) -command "stopDialogAutoupdate $w"
    button $w.updatebutton  -text "  Update now  " -borderwidth 1 -command {opp_updateinspectors}

    grid $w.stopbutton   -sticky news -padx 4 -pady 3
    grid $w.autoupdate   -sticky nes -padx 4 -pady 0
    grid $w.updatebutton -sticky nes -padx 4 -pady 3

    bind $w <Return> "opp_stopsimulation"
    bind $w <Escape> "opp_stopsimulation"
    bind $w <F8>     "opp_stopsimulation"

    set opp(autoupdate) [opp_getsimoption expressmode_autoupdate]
    stopDialogAutoupdate $w

    # 2. Center window
    center $w

    # 3. Set a grab and claim the focus too.

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

    catch {focus $opp(oldFocus)}
    destroy $w
    if {$opp(oldGrab) != ""} {
        grab $opp(oldGrab)
    }
}

proc optionsDialog {parent {defaultpage "g"}} {
    global opp config fonts help_tips helptexts

    set parent [winfo toplevel $parent]

    if {$parent == "."} {
        set w .optionsdialog
    } else {
        set w $parent.optionsdialog
    }

    createOkCancelDialog $w {Simulation Options}

    notebook $w.f.nb
    set nb $w.f.nb

    notebook:addPage $nb g General
    notebook:addPage $nb l Layouting
    notebook:addPage $nb a Animation
    notebook:addPage $nb t Filtering
    notebook:addPage $nb f Fonts
    pack $nb -expand 1 -fill both

    notebook:showPage $nb $defaultpage

    # "General" page
    labelframe $nb.g.f1 -text "Simulation Execution" -relief groove -borderwidth 2 -font $fonts(normal)
    label-entry $nb.g.f1.updfreq_fast    {Display update frequency for Fast Run (ms):}
    label-entry $nb.g.f1.updfreq_express {Display update frequency for Express Run (ms):}
    label-entry $nb.g.f1.stepdelay       {Per-event delay for slow execution (ms):}
    checkbutton $nb.g.f1.confirmexit -text {Confirm exit when simulation is in progress} -variable opp(confirmexit)
    $nb.g.f1.updfreq_fast.l config -width 0
    $nb.g.f1.updfreq_express.l config -width 0
    $nb.g.f1.stepdelay.l config -width 0
    pack $nb.g.f1.updfreq_fast -anchor w -fill x
    pack $nb.g.f1.updfreq_express -anchor w -fill x
    pack $nb.g.f1.stepdelay -anchor w -fill x
    pack $nb.g.f1.confirmexit -anchor w

    labelframe $nb.g.f2 -text "Logs" -relief groove -borderwidth 2 -font $fonts(normal)
    checkbutton $nb.g.f2.usemainwin -text {Use main window for module output} -variable opp(usemainwin)
    checkbutton $nb.g.f2.initbanners -text {Print initialization banners} -variable opp(initbanners)
    checkbutton $nb.g.f2.eventbanners -text {Print event banners} -variable opp(eventbanners)
    checkbutton $nb.g.f2.shortbanners -text {Short event banners} -variable opp(shortbanners)
    label-entry $nb.g.f2.numlines {Scrollback buffer (lines):}
    commentlabel $nb.g.f2.c1 {Applies to main window and module log windows. Leave blank for unlimited. Minimum value is 500 lines.}

    $nb.g.f2.numlines.l config -width 0
    pack $nb.g.f2.usemainwin -anchor w
    pack $nb.g.f2.initbanners -anchor w
    pack $nb.g.f2.eventbanners -anchor w
    pack $nb.g.f2.shortbanners -anchor w -padx 10
    pack $nb.g.f2.numlines -anchor w -fill x
    pack $nb.g.f2.c1 -anchor w

    pack $nb.g.f2 -anchor center -expand 0 -fill x -ipadx 0 -ipady 0 -padx 10 -pady 5 -side top
    pack $nb.g.f1 -anchor center -expand 0 -fill x -ipadx 0 -ipady 0 -padx 10 -pady 5 -side top

    # "Layouting" page
    labelframe $nb.l.f1 -text "Layouting" -relief groove -borderwidth 2 -font $fonts(normal)
    label $nb.l.f1.layouterlabel -text "Layouting algorithm:"
    frame $nb.l.f1.layouter
    radiobutton  $nb.l.f1.layouter.fast -text "Fast" -variable opp(layouterchoice) -value "fast"
    radiobutton  $nb.l.f1.layouter.advanced -text "Advanced" -variable opp(layouterchoice) -value "advanced"
    radiobutton  $nb.l.f1.layouter.auto -text "Adaptive (Fast for large networks, Advanced for smaller ones)" -variable opp(layouterchoice) -value "auto"
    checkbutton $nb.l.f1.layouting -text {Show layouting process} -variable opp(layouting)
    labelframe $nb.l.f2 -text "Display" -relief groove -borderwidth 2 -font $fonts(normal)
    checkbutton $nb.l.f2.arrangevectorconnections -text {Arrange connections on vector gates parallel to each other} -variable opp(arrangevectorconnections)
    checkbutton $nb.l.f2.allowresize -text {Resize window to fit network with current zoom level first} -variable opp(allowresize)
    checkbutton $nb.l.f2.allowzoom -text {Zoom out if necessary to fit network into window} -variable opp(allowzoom)
    label-entry $nb.l.f2.iconminsize  {Minimum icon size when zoomed out (pixels):}
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
    labelframe $nb.t.f1 -text "Timeline" -relief groove -borderwidth 2 -font $fonts(normal)
    checkbutton $nb.t.f1.tlwantself -text {Show self-messages} -variable opp(timeline-wantselfmsgs)
    checkbutton $nb.t.f1.tlwantnonself -text {Show non-self messages} -variable opp(timeline-wantnonselfmsgs)
    checkbutton $nb.t.f1.tlwantsilent -text {Show silent (non-animated) messages, see below} -variable opp(timeline-wantsilentmsgs)

    labelframe $nb.t.f2 -text "Animation" -relief groove -borderwidth 2 -font $fonts(normal)
    label $nb.t.f2.filterslabel -text "Messages to exclude from animation:"
    text $nb.t.f2.filterstext -highlightthickness 0 -height 10 -width 20
    commentlabel $nb.t.f2.c1 {One expression per line. Wildcards, AND, OR, NOT, numeric ranges ({5..10}), field matchers (className(..), kind(..), byteLength(..), etc.) are accepted.}

    pack $nb.t.f1.tlwantself -anchor w
    pack $nb.t.f1.tlwantnonself -anchor w
    pack $nb.t.f1.tlwantsilent -anchor w
    pack $nb.t.f2.filterslabel -anchor w
    pack $nb.t.f2.filterstext -fill x -padx 8 -pady 2
    pack $nb.t.f2.c1 -anchor w -fill x

    pack $nb.t.f1 -anchor center -expand 0 -fill x -ipadx 0 -ipady 0 -padx 10 -pady 5 -side top
    pack $nb.t.f2 -anchor center -expand 0 -fill x -ipadx 0 -ipady 0 -padx 10 -pady 5 -side top

    # "Animation" page
    labelframe $nb.a.f1 -text "General" -relief groove -borderwidth 2 -font $fonts(normal)
    checkbutton $nb.a.f1.anim -text {Animate messages} -variable opp(anim)
    label-scale $nb.a.f1.speed {Animation speed:}
    $nb.a.f1.speed.e config -length 200 -from 0 -to 3 -resolution 0.01 -variable opp(speed)
    checkbutton $nb.a.f1.concanim -text {Broadcast animation} -variable opp(concanim)
    commentlabel $nb.a.f1.ca "Animates send/sendDirect calls concurrently, after processing\neach event (i.e. out of sequence)"
    checkbutton $nb.a.f1.nextev -text {Show next event markers} -variable opp(nextev)
    checkbutton $nb.a.f1.sdarrows -text {Show arrows for sendDirect animation} -variable opp(sdarrows)
    checkbutton $nb.a.f1.bubbles -text {Show bubbles (bubble() calls)} -variable opp(bubbles)
    checkbutton $nb.a.f1.animmeth -text {Animate method calls} -variable opp(animmeth)
    label-scale $nb.a.f1.methdelay {Method call delay (ms):}
    $nb.a.f1.methdelay.e config -length 200 -from 0 -to 3000 -resolution 1 -variable opp(methdelay)
    labelframe $nb.a.f2 -text "Messages" -relief groove -borderwidth 2 -font $fonts(normal)
    checkbutton $nb.a.f2.msgnam -text {Display message names during animation} -variable opp(msgnam)
    checkbutton $nb.a.f2.msgclass -text {Display message class during animation} -variable opp(msgclass)
    checkbutton $nb.a.f2.msgcol -text {Color messages by message kind} -variable opp(msgcol)
    commentlabel $nb.a.f2.c {Color code (message kind modulo 8): 0=red 1=green 2=blue 3=white 4=yellow 5=cyan 6=magenta 7=black}
    checkbutton $nb.a.f2.penguin -text {Penguin mode} -variable opp(penguin)

    pack $nb.a.f1.anim -anchor w
    pack $nb.a.f1.speed -anchor w -expand 0 -fill x
    pack $nb.a.f1.concanim -anchor w
    pack $nb.a.f1.ca -anchor w
    pack $nb.a.f1.nextev -anchor w
    pack $nb.a.f1.sdarrows -anchor w
    pack $nb.a.f1.bubbles -anchor w
    pack $nb.a.f1.animmeth -anchor w
    pack $nb.a.f1.methdelay -anchor w -expand 0 -fill x
    pack $nb.a.f2.msgnam -anchor w
    pack $nb.a.f2.msgclass -anchor w
    pack $nb.a.f2.msgcol -anchor w
    pack $nb.a.f2.c -anchor w
    pack $nb.a.f2.penguin -anchor w

    pack $nb.a.f1 -anchor center -expand 0 -fill x -ipadx 0 -ipady 0 -padx 10 -pady 5 -side top
    pack $nb.a.f2 -anchor center -expand 0 -fill x -ipadx 0 -ipady 0 -padx 10 -pady 5 -side top

    # "Fonts" page
    labelframe $nb.f.f1 -text "Fonts" -relief groove -borderwidth 2 -font $fonts(normal)
    label-fontcombo $nb.f.f1.normalfont {User interface:} {}
    label-fontcombo $nb.f.f1.textfont  {Log windows:} {}
    label-fontcombo $nb.f.f1.canvasfont {Graphics:} {}
    commentlabel $nb.f.f1.note {Examples: Arial, Arial 10, Arial 10 bold. The system may silently use another font if the given font is not available.}
    pack $nb.f.f1.normalfont -anchor w -fill x
    pack $nb.f.f1.textfont -anchor w -fill x
    pack $nb.f.f1.canvasfont -anchor w -fill x
    pack $nb.f.f1.note -anchor w -fill x

    pack $nb.f.f1 -anchor center -expand 0 -fill x -ipadx 0 -ipady 0 -padx 10 -pady 5 -side top

    # Configure dialog
    $nb.g.f1.updfreq_fast.e insert 0 [opp_getsimoption updatefreq_fast_ms]
    $nb.g.f1.updfreq_express.e insert 0 [opp_getsimoption updatefreq_express_ms]
    $nb.g.f1.stepdelay.e insert 0 [opp_getsimoption stepdelay]
    $nb.g.f2.numlines.e insert 0 $config(logwindow-scrollbacklines)
    $nb.l.f2.iconminsize.e insert 0 [opp_getsimoption iconminsize]
    $nb.t.f2.filterstext insert 1.0 [opp_getsimoption silent_event_filters]
    set opp(usemainwin) [opp_getsimoption use_mainwindow]
    set opp(eventbanners) [opp_getsimoption event_banners]
    set opp(initbanners) [opp_getsimoption init_banners]
    set opp(shortbanners) [opp_getsimoption short_banners]
    set opp(anim)       [opp_getsimoption animation_enabled]
    set opp(concanim)   $config(concurrent-anim)
    set opp(nextev)     [opp_getsimoption nexteventmarkers]
    set opp(sdarrows)   [opp_getsimoption senddirect_arrows]
    set opp(animmeth)   [opp_getsimoption anim_methodcalls]
    set opp(methdelay)  [opp_getsimoption methodcalls_delay]
    set opp(msgnam)     [opp_getsimoption animation_msgnames]
    set opp(msgclass)   [opp_getsimoption animation_msgclassnames]
    set opp(msgcol)     [opp_getsimoption animation_msgcolors]
    set opp(penguin)    [opp_getsimoption penguin_mode]
    set opp(layouting)  [opp_getsimoption showlayouting]
    set opp(layouterchoice) [opp_getsimoption layouterchoice]
    set opp(arrangevectorconnections) [opp_getsimoption arrangevectorconnections]
    set opp(bubbles)    [opp_getsimoption bubbles]
    set opp(speed)      [opp_getsimoption animation_speed]
    set opp(confirmexit) $config(confirm-exit)
    set opp(allowresize) $config(layout-may-resize-window)
    set opp(allowzoom)   $config(layout-may-change-zoom)

    set opp(timeline-wantselfmsgs)      $config(timeline-wantselfmsgs)
    set opp(timeline-wantnonselfmsgs)   $config(timeline-wantnonselfmsgs)
    set opp(timeline-wantsilentmsgs)    $config(timeline-wantsilentmsgs)

    fontcombo:set $nb.f.f1.normalfont.e $fonts(normal)
    fontcombo:set $nb.f.f1.textfont.e $fonts(text)
    fontcombo:set $nb.f.f1.canvasfont.e $fonts(canvas)

    setInitialDialogFocus $nb.g.f2.usemainwin

    if [execOkCancelDialog $w] {
        opp_setsimoption stepdelay             [$nb.g.f1.stepdelay.e get]
        opp_setsimoption updatefreq_fast_ms    [$nb.g.f1.updfreq_fast.e get]
        opp_setsimoption updatefreq_express_ms [$nb.g.f1.updfreq_express.e get]
        opp_setsimoption silent_event_filters  [$nb.t.f2.filterstext get 1.0 end]
        set n [$nb.g.f2.numlines.e get]
        if {$n=="" || [string is integer $n]} {
            if {$n!="" && $n<500} {set n 500}
            set config(logwindow-scrollbacklines) $n
        }
        opp_setsimoption use_mainwindow      $opp(usemainwin)
        opp_setsimoption event_banners       $opp(eventbanners)
        opp_setsimoption init_banners        $opp(initbanners)
        opp_setsimoption short_banners       $opp(shortbanners)
        opp_setsimoption animation_enabled   $opp(anim)
        set config(concurrent-anim)          $opp(concanim)
        opp_setsimoption nexteventmarkers    $opp(nextev)
        opp_setsimoption senddirect_arrows   $opp(sdarrows)
        opp_setsimoption anim_methodcalls    $opp(animmeth)
        opp_setsimoption methodcalls_delay   $opp(methdelay)
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
        set config(confirm-exit)             $opp(confirmexit)
        set config(layout-may-resize-window) $opp(allowresize)
        set config(layout-may-change-zoom)   $opp(allowzoom)

        set config(timeline-wantselfmsgs)    $opp(timeline-wantselfmsgs)
        set config(timeline-wantnonselfmsgs) $opp(timeline-wantnonselfmsgs)
        set config(timeline-wantsilentmsgs)  $opp(timeline-wantsilentmsgs)

        set font [actualFont [fixupFontName [$nb.f.f1.normalfont.e get]]]
        if {$font != ""} {
            set fonts(normal)  $font
            set fonts(bold)    $font
            set fonts(balloon) $font
        }

        set font [actualFont [fixupFontName [$nb.f.f1.textfont.e get]]]
        if {$font != ""} {
            set fonts(text) $font
        }

        set font [actualFont [fixupFontName [$nb.f.f1.canvasfont.e get]]]
        if {$font != ""} {
            set fonts(canvas) $font
            set fonts(timeline) $font
        }

        reflectSettingsInGui
    }
    destroy $w
}

proc setIfPatternIsValid {var pattern} {
    if [catch {opp_checkpattern $pattern} errmsg] {
        tk_messageBox -type ok -icon warning -title Tkenv -message "Filter pattern \"$pattern\" has invalid syntax -- setting unchanged."
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

    set w .rununtil
    createOkCancelDialog $w {Run until}

    # collect FES messages for combo
    set msglabels {""}
    set msgptrs [opp_fesmsgs 1000 1 1 1]
    foreach ptr $msgptrs {
        set msglabel "[opp_getobjectfullname $ptr] ([opp_getobjectshorttypename $ptr]), [opp_getobjectinfostring $ptr] -- $ptr"
        lappend msglabels $msglabel
    }

    # create and pack controls
    label-entry $w.f.time  {Simulation time to stop at:}
    label-entry $w.f.event {Event number to stop at:}
    label-combo $w.f.msg   {Message to stop at:} $msglabels
    label-check $w.f.stop  {} {stop if message gets cancelled} tmp(stop)
    label-combo $w.f.mode  {Running mode:}  {{Normal} {Fast (rare updates)} {Express (tracing off)}}

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
    $w.f.msg.e configure -value $msglabel
    $w.f.mode.e configure -value $config(rununtil-mode)
    set tmp(stop) [opp_getsimoption stoponmsgcancel]

    $w.f.time.e select range 0 end
    $w.f.event.e select range 0 end

    setInitialDialogFocus $w.f.time.e

    if [execOkCancelDialog $w] {
        set time_var0  [$w.f.time.e get]
        set event_var0 [$w.f.event.e get]
        set msg_var0   [lindex [$w.f.msg.e get] end]
        set mode_var0  [lindex [$w.f.mode.e cget -value] 0]

        set config(rununtil-time)  $time_var0
        set config(rununtil-event) $event_var0
        set config(rununtil-msg)   $msg_var0
        set config(rununtil-mode)  [$w.f.mode.e cget -value]
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
    set dlg [winfo toplevel $w].dlg
    if {$dlg=="..dlg"} {set dlg .dlg}

    # create dialog with OK and Cancel buttons
    set title "Find"
    createOkCancelDialog $dlg $title

    label-entry $dlg.f.find "Find string:"
    frame $dlg.f.opt
    pack $dlg.f.find -expand 0 -fill x
    pack $dlg.f.opt -expand 0 -fill none -anchor e

    # add entry fields
    checkbutton $dlg.f.opt.regexp -text {regular expression} -underline 0 -variable tmp(regexp)
    checkbutton $dlg.f.opt.case -text {case sensitive} -underline 0 -variable tmp(case-sensitive)
    checkbutton $dlg.f.opt.words -text {whole words only} -underline 0 -variable tmp(whole-words)
    checkbutton $dlg.f.opt.backwards -text {search backwards} -underline 7 -variable tmp(backwards)

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

    # exec the dialog, extract its contents if OK was pressed, then delete dialog
    if {[execOkCancelDialog $dlg] == 1} {
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
        doFind $w $findstring $case $words $regexp $backwards
   }
   catch {destroy $dlg}
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

    set title "Filter window contents"
    set msg "Select modules to show log messages from:"

    set w .treedialog
    createOkCancelDialog $w $title

    label $w.f.m -text $msg -anchor w -justify left
    frame $w.f.f -bd 2 -relief sunken
    pack $w.f.m -fill x -padx 10 -pady 5 -side top
    pack $w.f.f -expand 1 -fill both -padx 10 -pady 5 -side top

    canvas $w.f.f.c -width 300 -height 350 -bd 0 -relief flat -yscrollcommand "$w.f.f.vsb set"
    #   -xscrollcommand "$w.f.f.hsb set"
    #scrollbar $w.f.f.hsb -command "$w.f.f.c xview" -orient horiz
    scrollbar $w.f.f.vsb -command "$w.f.f.c yview"
    grid $w.f.f.c   $w.f.f.vsb  -sticky news
    #grid $w.f.f.hsb x           -sticky news
    grid rowconfig $w.f.f 0 -weight 1
    grid columnconfig $w.f.f 0 -weight 1

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
        #return [inspector:getIconForObject $ptr]
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
    }
}




# filteredObjectList:window --
#
# Implements the "Find/inspect objects" dialog.
#
proc filteredObjectList:window {{ptr ""}} {
    global config tmp icons help_tips helptexts fonts
    global HAVE_BLT B2 B3

    set w .objdlg

    if {$ptr==""} {set ptr [opp_object_simulation]}

    # if already exists, update the "search in" field and show it
    if {[winfo exists $w]} {
        $w.f.filter.searchin.e delete 0 end
        $w.f.filter.searchin.e insert 0 [opp_getobjectfullpath $ptr]
        showWindow $w  ;# black magic to raise the window
        return
    }

    # otherwise create
    createCloseDialog $w "Find/inspect objects"

    # Create toolbar
    frame $w.toolbar -relief raised -borderwidth 1
    pack $w.toolbar -anchor center -expand 0 -fill x -side top -before $w.f
    foreach i {
      {sep1     -separator}
      {step     -image $icons(step)    -command {oneStep}}
      {run      -image $icons(run)     -command {runNormal}}
      {fastrun  -image $icons(fast)    -command {runFast}}
      {exprrun  -image $icons(express) -command {runExpress}}
      {until    -image $icons(until)   -command {runUntil}}
      {sep2     -separator}
      {stop     -image $icons(stop)    -command {stopSimulation}}
    } {
      set b [eval iconbutton $w.toolbar.$i]
      pack $b -anchor n -expand 0 -fill none -side left -padx 0 -pady 2
    }

    set help_tips($w.toolbar.step)    {Execute one event (F4)}
    set help_tips($w.toolbar.run)     {Run with full animation (F5)}
    set help_tips($w.toolbar.fastrun) {Run faster: no animation and rare inspector updates (F6)}
    set help_tips($w.toolbar.exprrun) {Run at full speed: no text output, animation or inspector updates (F7)}
    set help_tips($w.toolbar.until)   {Run until time or event number}
    set help_tips($w.toolbar.stop)    {Stop running simulation (F8)}


    # vars
    set tmp(class)    $config(filtobjlist-class)
    set tmp(name)     $config(filtobjlist-name)
    set tmp(order)    $config(filtobjlist-order)
    set tmp(category) $config(filtobjlist-category)

    # two panels: $w.f.filter is the upper panel for filters, and
    # $w.f.main is the lower one with the listbox.

    # panel for filters
    frame $w.f.filter
    pack $w.f.filter -anchor center -expand 0 -fill x -side top

    #label $w.f.filter.title -text "Filter list of all objects in the simulation:" -justify left -anchor w
    #pack $w.f.filter.title -anchor w -expand 1 -fill x -side top

    label-entry $w.f.filter.searchin "Search inside:" [opp_getobjectfullpath $ptr]
    pack $w.f.filter.searchin -anchor w -expand 0 -fill x -side top

    labelframe $w.f.filter.pars -text "Search by class and object name:" -font $fonts(normal)
    pack $w.f.filter.pars -anchor center -expand 0 -fill x -side top
    set fp $w.f.filter.pars

    labelwithhelp $fp.classlabel "Class filter expression:" $helptexts(filterdialog-classnamepattern)
    labelwithhelp $fp.namelabel  "Object full path filter, e.g. \"*.queue\ AND not length(0)\":" $helptexts(filterdialog-namepattern)

    combo $fp.classentry [concat {{}} [filteredObjectList:getClassNames]]
    $fp.classentry.entry config -textvariable tmp(class)
    entry $fp.nameentry -textvariable tmp(name)

    set help_tips($fp.classentry.entry) $helptexts(filterdialog-classnamepattern)
    set help_tips($fp.nameentry) $helptexts(filterdialog-namepattern)

    button $fp.refresh -text "Refresh" -width 10 -command "filteredObjectList:refresh $w"

    grid $fp.classlabel $fp.namelabel x           -sticky nw   -padx 5
    grid $fp.classentry $fp.nameentry $fp.refresh -sticky news -padx 5 -pady 3
    grid columnconfig $fp 0 -weight 1
    grid columnconfig $fp 1 -weight 3

    # category filters
    labelframe $w.f.filter.cat -text "Object categories:" -font $fonts(normal)
    set cf $w.f.filter.cat
    checkbutton $cf.modules -text "modules" -variable tmp(cat-m)
    checkbutton $cf.modpars -text "module parameters" -variable tmp(cat-p)
    checkbutton $cf.queues -text "queues" -variable tmp(cat-q)
    checkbutton $cf.statistics -text "outvectors, statistics, variables" -variable tmp(cat-s)
    checkbutton $cf.messages -text "messages"  -variable tmp(cat-g)
    checkbutton $cf.chansgates -text "gates, channels" -variable tmp(cat-c)
    checkbutton $cf.variables -text "FSM states, variables"  -variable tmp(cat-v)
    checkbutton $cf.other -text "other" -variable tmp(cat-o)
    grid $cf.modules   $cf.modpars     $cf.queues     $cf.statistics  -sticky nw
    grid $cf.messages  $cf.chansgates  $cf.variables  $cf.other       -sticky nw
    grid columnconfigure $cf 3 -weight 1
    pack $cf -anchor center -expand 0 -fill x -side top

    foreach {c} {m q p c s g v o} {
        set tmp(cat-$c) [string match "*$c*" $tmp(category)]
    }

    # Sorting
    if {!$HAVE_BLT} {
        labelframe $w.f.filter.order -text "Sorting:" -font $fonts(normal)
        label-combo $w.f.filter.order.entry "Sort by:" {{Class} {Full name} {Name}}
        $w.f.filter.order.entry.e configure -textvariable tmp(order)
        pack $w.f.filter.order.entry -expand 0 -fill none -side top -anchor w
        pack $w.f.filter.order -expand 0 -fill x -side top
    }


    # number of objects
    label $w.f.numobj -text "Found 0 objects" -justify left -anchor w
    pack $w.f.numobj -anchor w -expand 0 -fill x -side top

    # panel for listbox
    frame $w.f.main
    scrollbar $w.f.main.vsb -command "$w.f.main.list yview"
    scrollbar $w.f.main.hsb -command "$w.f.main.list xview" -orient horiz
    multicolumnlistbox $w.f.main.list {
        {class   Class  80}
        {name    Name  180}
        {info    Info}
        {ptr     Pointer}
    } -height 200 -yscrollcommand "$w.f.main.vsb set" -xscrollcommand "$w.f.main.hsb set"

    grid $w.f.main.list $w.f.main.vsb -sticky news
    grid $w.f.main.hsb  x             -sticky news
    grid rowconfig $w.f.main 0 -weight 1
    grid columnconfig $w.f.main 0 -weight 1

    pack $w.f.main  -anchor center -expand 1 -fill both -side top

    set lb $w.f.main.list

    set type "(default)"

    # leave listbox empty -- filling it with all objects might take too long

    # Configure dialog
    $w.buttons.closebutton config -command filteredObjectList:windowClose
    wm protocol $w WM_DELETE_WINDOW "$w.buttons.closebutton invoke"

    bind $fp.classentry.entry <Return> "$fp.refresh invoke"
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
    set w .objdlg

    set config(filtobjlist-class)    $tmp(class)
    set config(filtobjlist-name)     $tmp(name)
    set config(filtobjlist-order)    $tmp(order)
    set config(filtobjlist-category) $tmp(category)

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
    global config tmp HAVE_BLT
    global filtobjlist_state

    # resolve root object
    set rootobjectname [$w.f.filter.searchin.e get]
    if {$rootobjectname=="simulation"} {
        set rootobject [opp_object_simulation]
    } else {
        if [catch {
            set rootobject [opp_modulebypath $rootobjectname]
        } err] {
            tk_messageBox -title "Error" -icon error -type ok -parent $w -message "Error: $err."
            return
        }
        if [opp_isnull $rootobject] {
            tk_messageBox -title "Error" -icon error -type ok -parent $w \
                -message "Please enter a module name or 'simulation' in the 'Search inside' field -- '$rootobjectname' could not be resolved."
            return
        }
    }

    set tmp(category) ""
    set categories {m q p c s g v o}
    foreach {c} $categories {
        if {$tmp(cat-$c)} {set tmp(category) "$tmp(category)$c"}
    }
    if {[string length $tmp(category)]==[llength $categories]} {
        set tmp(category) "a$tmp(category)"
    }

    set class $tmp(class)
    if {$class==""} {set class "*"}
    if [catch {opp_checkpattern $class} errmsg] {
        tk_messageBox -parent $w -type ok -icon warning -title Tkenv -message "Class filter pattern \"$class\" has invalid syntax -- using \"*\" instead."
        set class "*"
    }

    set name $tmp(name)
    if {$name==""} {set name "*"}
    if [catch {opp_checkpattern $name} errmsg] {
        tk_messageBox -parent $w -type ok -icon warning -title Tkenv -message "Name filter pattern \"$name\" has invalid syntax -- using \"*\" instead."
        set name "*"
    }

    if {!$HAVE_BLT} {
        set order $tmp(order)
    } else {
        set order ""
    }

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
    multicolumnlistbox:deleteAll $lb

    # insert into listbox
    if {$viewall=="ok"} {
        if {$num==$maxcount} {
            $w.f.numobj config -text "The first $num objects found:"
        } else {
            $w.f.numobj config -text "Found $num objects:"
        }
        foreach ptr $objlist {
            multicolumnlistbox:insert $lb $ptr [list ptr $ptr class [opp_getobjectshorttypename $ptr] name [opp_getobjectfullpath $ptr] info [opp_getobjectinfostring $ptr]] [inspector:getIconForObject $ptr]
        }
        set filtobjlist_state(outofdate) 0
        #$lb selection set 0
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
    set ptr [lindex [multicolumnlistbox:curSelection $lb] 0]
    if {$ptr==""} return
    set insptypes [opp_supported_insp_types $ptr]

    set p $w.popup
    catch {destroy $p}
    menu $p -tearoff 0
    if {[filteredobjectlist_isnotsafetoinspect]} {set state "disabled"} else {set state "normal"}
    foreach type $insptypes {
       $p add command -label "$type..." -state $state -command "opp_inspect $ptr \{$type\}; after 500 \{catch \{raise $w; focus $lb\}\}"
    }
    $p post $X $Y
}

proc filteredObjectList:inspect {lb} {
    set w .objdlg
    if {[filteredobjectlist_isnotsafetoinspect]} {
        if {[isRunning]} {
            set advice "please stop the simulation and click Refresh first"
        } else {
            set advice "please click Refresh first"
        }
        tk_messageBox -parent $w -icon info -type ok -title {Filtered object list} \
                      -message "Dialog contents might be out of date -- $advice."
        return
    }

    inspectItemIn $lb
}

#----

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

proc modelInfoDialog {{w ""}} {
    if {[networkPresent] == 0} {return 0}

    if {$w==""} {
        set modptr [opp_object_systemmodule]
    } else {
        regexp {\.(ptr.*)-[0-9]+} $w match modptr
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

    global fonts
    set dlg $w.dlg
    catch {destroy $dlg}
    createOkCancelDialog $dlg "Model Information"
    $dlg.f config -border 0
    message $dlg.f.txt -text $msg -width 400
    pack $dlg.f.txt -expand 1 -fill both
    destroy $dlg.buttons.cancelbutton
    execOkCancelDialog $dlg
    destroy $dlg
}
