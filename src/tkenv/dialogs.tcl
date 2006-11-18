#=================================================================
#  DIALOGS.TCL - part of
#
#                     OMNeT++/OMNEST
#            Discrete System Simulation in C++
#
#=================================================================

#----------------------------------------------------------------#
#  Copyright (C) 1992-2005 Andras Varga
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

proc inputbox {title msg variable} {
    # This procedure displays a dialog box, waits for a button in the dialog
    # to be invoked, then returns the index of the selected button.

    upvar $variable var

    set w .inputbox
    createOkCancelDialog $w $title

    label $w.f.l -text $msg
    entry $w.f.e -highlightthickness 0
    pack $w.f.l -anchor w -expand 0 -fill none -padx 2 -pady 2 -side top
    pack $w.f.e -anchor w -expand 1 -fill x -padx 2 -pady 2 -side top
    $w.f.e insert 0 $var
    $w.f.e selection range 0 end
    focus $w.f.e

    if [execOkCancelDialog $w] {
        set var [$w.f.e get]
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
    focus $w.f.c.e

    $w.f.c.e config -width 30

    if [execOkCancelDialog $w] {
        set var [$w.f.c.e cget -value]
        destroy $w
        return 1
    }
    destroy $w
    return 0
}

proc display_stopdialog {} {
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

    button $w.stopbutton  -text "STOP!" -background red -activebackground red \
          -borderwidth 6 -font $fonts(big) -command {opp_stopsimulation}
    checkbutton $w.autoupdate -text "auto-update inspectors" -variable opp(autoupdate) -command "stopdialog_autoupdate $w"
    button $w.updatebutton  -text "  Update now  " -borderwidth 1 -command {opp_updateinspectors}

    grid $w.stopbutton   -sticky news -padx 4 -pady 3
    grid $w.autoupdate   -sticky nes -padx 4 -pady 0
    grid $w.updatebutton -sticky nes -padx 4 -pady 3

    bind $w <Return> "opp_stopsimulation"
    bind $w <Escape> "opp_stopsimulation"
    bind $w <F8>     "opp_stopsimulation"

    set opp(autoupdate) [opp_getsimoption expressmode_autoupdate]
    stopdialog_autoupdate $w

    # 2. Center window
    center $w

    # 3. Set a grab and claim the focus too.

    set opp(oldFocus) [focus]
    set opp(oldGrab) [grab current $w]
    grab $w
    focus $w.stopbutton
}

proc stopdialog_autoupdate {w} {
    global opp
    if {$opp(autoupdate)} {
        opp_setsimoption expressmode_autoupdate 1
        $w.updatebutton config -state disabled
    } else {
        opp_setsimoption expressmode_autoupdate 0
        $w.updatebutton config -state normal
    }
}

proc remove_stopdialog {} {
    # Remove the dialog created by display_stopdialog

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

proc options_dialog {{defaultpage "g"}} {
    global opp config help_tips helptexts

    set w .optionsdialog

    createOkCancelDialog $w {Simulation options}

    notebook $w.f.nb bottom
    set nb $w.f.nb

    notebook_addpage $nb g General
    notebook_addpage $nb a Animation
    notebook_addpage $nb t Timeline
    pack $nb -expand 1 -fill both

    notebook_showpage $nb $defaultpage

    frame $nb.g.f1 -relief groove -borderwidth 2
    label-entry $nb.g.f1.updfreq_fast    {Update freq. for Fast Run (events):}
    label-entry $nb.g.f1.updfreq_express {Update freq. for Express Run (events):}
    label-entry $nb.g.f1.stepdelay       {Per-event delay for slow execution:}
    $nb.g.f1.updfreq_fast.l config -width 0
    $nb.g.f1.updfreq_express.l config -width 0
    $nb.g.f1.stepdelay.l config -width 0
    pack $nb.g.f1.updfreq_fast -anchor w -fill x
    pack $nb.g.f1.updfreq_express -anchor w -fill x
    pack $nb.g.f1.stepdelay -anchor w -fill x

    frame $nb.g.f2 -relief groove -borderwidth 2
    checkbutton $nb.g.f2.usemainwin -text {Use main window for module output} -variable opp(usemainwin)
    checkbutton $nb.g.f2.banners -text {Print event banners} -variable opp(banners)
    label-entry $nb.g.f2.numlines {Scrollback buffer (lines):}
    commentlabel $nb.g.f2.c1 {Applies to main window and module log windows. Leave blank for unlimited. Minimum value is 500 lines.}
    checkbutton $nb.g.f2.bkpts -text {Stop on breakpoint() calls} -variable opp(bkpts)
    checkbutton $nb.g.f2.layouting -text {Show layouting process} -variable opp(layouting)
    checkbutton $nb.g.f2.confirmexit -text {Confirm exit when simulation is in progress} -variable opp(confirmexit)
    $nb.g.f2.numlines.l config -width 0
    pack $nb.g.f2.usemainwin -anchor w
    pack $nb.g.f2.banners -anchor w
    pack $nb.g.f2.numlines -anchor w -fill x
    pack $nb.g.f2.c1 -anchor w
    pack $nb.g.f2.bkpts -anchor w
    pack $nb.g.f2.layouting -anchor w
    pack $nb.g.f2.confirmexit -anchor w

    #frame $nb.t -relief groove -borderwidth 2
    checkbutton $nb.t.tlwantself -text {Display self-messages in the timeline} -variable opp(timeline-wantselfmsgs)
    checkbutton $nb.t.tlwantnonself -text {Display non-self messages in the timeline} -variable opp(timeline-wantnonselfmsgs)
    label-entry-help $nb.t.tlnamepattern {Message name filter:} $helptexts(timeline-namepattern)
    label-entry-help $nb.t.tlclassnamepattern {Class name filter:} $helptexts(timeline-classnamepattern)
    commentlabel $nb.t.c1 {Wildcards, AND, OR, NOT, numeric ranges, field matchers like kind, length, etc. accepted. Click Help for more.}
    $nb.t.tlnamepattern.l config -width 20
    $nb.t.tlclassnamepattern.l config -width 20
    $nb.t.tlnamepattern.e config -width 40
    $nb.t.tlclassnamepattern.e config -width 40
    pack $nb.t.tlwantself -anchor w
    pack $nb.t.tlwantnonself -anchor w
    pack $nb.t.tlnamepattern -anchor w -fill x
    pack $nb.t.tlclassnamepattern -anchor w -fill x
    pack $nb.t.c1 -anchor w

    checkbutton $nb.a.anim -text {Animate messages} -variable opp(anim)
    label-scale $nb.a.speed {Animation speed:}
    $nb.a.speed.e config -length 200 -from 0 -to 3 -resolution 0.01 -variable opp(speed)
    checkbutton $nb.a.concanim -text {Broadcast animation} -variable opp(concanim)
    commentlabel $nb.a.ca "Animates send/sendDirect calls concurrently, after processing\neach event (i.e. out of sequence)"
    checkbutton $nb.a.nextev -text {Show next event markers} -variable opp(nextev)
    checkbutton $nb.a.sdarrows -text {Show arrows for sendDirect() animation} -variable opp(sdarrows)
    checkbutton $nb.a.animmeth -text {Animate method calls} -variable opp(animmeth)
    label-scale $nb.a.methdelay {Method call delay (ms):}
    $nb.a.methdelay.e config -length 200 -from 0 -to 3000 -resolution 1 -variable opp(methdelay)
    checkbutton $nb.a.msgnam -text {Display message names during animation} -variable opp(msgnam)
    checkbutton $nb.a.msgclass -text {Display message class during animation} -variable opp(msgclass)
    checkbutton $nb.a.msgcol -text {Color messages by message kind} -variable opp(msgcol)
    commentlabel $nb.a.c {Color code (message->kind() mod 8): 0=red 1=green 2=blue 3=white 4=yellow 5=cyan 6=magenta 7=black}
    checkbutton $nb.a.penguin -text {Penguin mode} -variable opp(penguin)
    checkbutton $nb.a.bubbles -text {Show bubbles (bubble() calls)} -variable opp(bubbles)
    pack $nb.a.anim -anchor w
    pack $nb.a.speed -anchor w -expand 0 -fill x
    pack $nb.a.concanim -anchor w
    pack $nb.a.ca -anchor w
    pack $nb.a.nextev -anchor w
    pack $nb.a.sdarrows -anchor w
    pack $nb.a.animmeth -anchor w
    pack $nb.a.methdelay -anchor w -expand 0 -fill x
    pack $nb.a.msgnam -anchor w
    pack $nb.a.msgclass -anchor w
    pack $nb.a.msgcol -anchor w
    pack $nb.a.c -anchor w
    pack $nb.a.penguin -anchor w
    pack $nb.a.bubbles -anchor w

    pack $nb.g.f2 -anchor center -expand 0 -fill x -ipadx 0 -ipady 0 -padx 10 -pady 5 -side top
    pack $nb.g.f1 -anchor center -expand 0 -fill x -ipadx 0 -ipady 0 -padx 10 -pady 5 -side top

    # Configure dialog
    $nb.g.f1.updfreq_fast.e insert 0 [opp_getsimoption updatefreq_fast]
    $nb.g.f1.updfreq_express.e insert 0 [opp_getsimoption updatefreq_express]
    $nb.g.f1.stepdelay.e insert 0 [opp_getsimoption stepdelay]
    $nb.g.f2.numlines.e insert 0 $config(logwindow-scrollbacklines)
    set opp(usemainwin) [opp_getsimoption use_mainwindow]
    set opp(banners)    [opp_getsimoption print_banners]
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
    set opp(bubbles)    [opp_getsimoption bubbles]
    set opp(speed)      [opp_getsimoption animation_speed]
    set opp(bkpts)      [opp_getsimoption bkpts_enabled]
    set opp(confirmexit) $config(confirm-exit)
    $nb.t.tlnamepattern.e insert 0      $config(timeline-msgnamepattern)
    $nb.t.tlclassnamepattern.e insert 0 $config(timeline-msgclassnamepattern)
    set opp(timeline-wantselfmsgs)      $config(timeline-wantselfmsgs)
    set opp(timeline-wantnonselfmsgs)   $config(timeline-wantnonselfmsgs)

    focus $nb.g.f2.usemainwin

    if [execOkCancelDialog $w] {
        opp_setsimoption stepdelay           [$nb.g.f1.stepdelay.e get]
        opp_setsimoption updatefreq_fast     [$nb.g.f1.updfreq_fast.e get]
        opp_setsimoption updatefreq_express  [$nb.g.f1.updfreq_express.e get]
        set n [$nb.g.f2.numlines.e get]
        if {$n=="" || [string is integer $n]} {
            if {$n!="" && $n<500} {set n 500}
            set config(logwindow-scrollbacklines) $n
        }
        opp_setsimoption use_mainwindow      $opp(usemainwin)
        opp_setsimoption print_banners       $opp(banners)
        opp_setsimoption animation_enabled   $opp(anim)
        set config(concurrent-anim)          $opp(concanim)
        opp_setsimoption nexteventmarkers    $opp(nextev)
        opp_setsimoption senddirect_arrows   $opp(sdarrows)
        opp_setsimoption anim_methodcalls    $opp(animmeth)
        opp_setsimoption methodcalls_delay   $opp(methdelay)
        opp_setsimoption animation_msgnames  $opp(msgnam)
        opp_setsimoption animation_msgclassnames $opp(msgclass)
        opp_setsimoption animation_msgcolors $opp(msgcol)
        opp_setsimoption penguin_mode        $opp(penguin)
        opp_setsimoption showlayouting       $opp(layouting)
        opp_setsimoption bubbles             $opp(bubbles)
        opp_setsimoption animation_speed     $opp(speed)
        opp_setsimoption bkpts_enabled       $opp(bkpts)
        set config(confirm-exit)             $opp(confirmexit)
        setIfPatternIsValid config(timeline-msgnamepattern)  [$nb.t.tlnamepattern.e get]
        setIfPatternIsValid config(timeline-msgclassnamepattern) [$nb.t.tlclassnamepattern.e get]
        set config(timeline-wantselfmsgs)    $opp(timeline-wantselfmsgs)
        set config(timeline-wantnonselfmsgs) $opp(timeline-wantnonselfmsgs)

        opp_updateinspectors
        redraw_timeline
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

proc rununtil_dialog {time_var event_var mode_var} {

    global opp config

    upvar $time_var time_var0
    upvar $event_var event_var0
    upvar $mode_var mode_var0

    set w .rununtil

    createOkCancelDialog $w {Run until}

    label-entry $w.f.time  {Simulation time to stop at:}
    label-entry $w.f.event {Event number to stop at:}
    label-combo $w.f.mode  {Running mode:} \
                 {{Normal} {Fast (rare updates)} {Express (tracing off)}}

    foreach i {time event mode} {
       $w.f.$i.l configure -width 24
       $w.f.$i.e configure -width 18
       pack $w.f.$i -anchor w
    }

    pack $w.f -anchor center -expand 1 -fill both -padx 10 -pady 10 -side top

    $w.f.time.e insert 0 $config(rununtil-time)
    $w.f.event.e insert 0 $config(rununtil-event)
    $w.f.mode.e configure -value $config(rununtil-mode)

    $w.f.time.e select range 0 end
    $w.f.event.e select range 0 end

    focus $w.f.time.e

    if [execOkCancelDialog $w] {
        set time_var0  [$w.f.time.e get]
        set event_var0 [$w.f.event.e get]
        set mode_var0  [lindex [$w.f.mode.e cget -value] 0]

        set config(rununtil-time)  $time_var0
        set config(rununtil-event) $event_var0
        set config(rununtil-mode) [$w.f.mode.e cget -value]

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

    # add entry fields
    label-entry $dlg.f.find "Find string:"
    checkbutton $dlg.f.regexp -text {regular expression} -underline 0 -variable tmp(regexp)
    checkbutton $dlg.f.case -text {case sensitive} -underline 0 -variable tmp(case-sensitive)
    checkbutton $dlg.f.words -text {whole words only} -underline 0 -variable tmp(whole-words)
    checkbutton $dlg.f.backwards -text {search backwards} -underline 7 -variable tmp(backwards)

    grid $dlg.f.find   -                -sticky nw
    grid $dlg.f.regexp $dlg.f.case      -sticky nw
    grid $dlg.f.words  $dlg.f.backwards -sticky nw

    bind $dlg <Alt-r> [list $dlg.f.regexp invoke]
    bind $dlg <Alt-R> [list $dlg.f.regexp invoke]
    bind $dlg <Alt-c> [list $dlg.f.case invoke]
    bind $dlg <Alt-C> [list $dlg.f.case invoke]
    bind $dlg <Alt-w> [list $dlg.f.words invoke]
    bind $dlg <Alt-W> [list $dlg.f.words invoke]
    bind $dlg <Alt-b> [list $dlg.f.backwards invoke]
    bind $dlg <Alt-B> [list $dlg.f.backwards invoke]

    $dlg.f.find.e insert 0 $config(editor-findstring)
    $dlg.f.find.e select range 0 end

    focus $dlg.f.find.e

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


# filteredobjectlist_window --
#
# Implements the "Find/inspect objects" dialog.
#
proc filteredobjectlist_window {{ptr ""}} {
    global config tmp icons help_tips helptexts
    global HAVE_BLT

    set w .objdlg

    if {$ptr==""} {set ptr [opp_object_simulation]}

    # if already exists, update the "search in" field and show it
    if {[winfo exists $w]} {
        $w.f.filter.searchin.e delete 0 end
        $w.f.filter.searchin.e insert 0 [opp_getobjectfullpath $ptr]
        show_window $w  ;# black magic to raise the window
        return
    }

    # otherwise create
    createCloseDialog $w "Find/inspect objects"

    # Create toolbar
    frame $w.toolbar -relief raised -borderwidth 1
    pack $w.toolbar -anchor center -expand 0 -fill x -side top -before $w.f
    foreach i {
      {sep01    -separator}
      {step     -image $icons(step)    -command {one_step}}
      {sep1     -separator}
      {run      -image $icons(run)     -command {run_normal}}
      {fastrun  -image $icons(fast)    -command {run_fast}}
      {exprrun  -image $icons(express) -command {run_express}}
      {sep2     -separator}
      {until    -image $icons(until)   -command {run_until}}
      {sep3     -separator}
      {stop     -image $icons(stop)    -command {stop_simulation}}
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

    labelframe $w.f.filter.pars -text "Search by class and object name:"
    pack $w.f.filter.pars -anchor center -expand 0 -fill x -side top
    set fp $w.f.filter.pars

    labelwithhelp $fp.classlabel "Class filter expression:" $helptexts(filterdialog-classnamepattern)
    labelwithhelp $fp.namelabel  "Object full path filter, e.g. \"*.queue\ AND not length(0)\":" $helptexts(filterdialog-namepattern)

    combo $fp.classentry [concat {{}} [getClassNames]]
    $fp.classentry.entry config -textvariable tmp(class)
    entry $fp.nameentry -textvariable tmp(name)

    set classhelptext "Wildcards accepted (*,?), try '*Packet'"
    set namehelptext "Use wildcards (*,?): '*.foo' for any object named foo; '*foo*' for any\n\
                     object whose full path contains foo; use '{a-z}' for character range"

    button $fp.refresh -text "Refresh" -width 10 -command "filteredobjectlist_refresh $w"

    grid $fp.classlabel $fp.namelabel x           -sticky nw   -padx 5
    grid $fp.classentry $fp.nameentry $fp.refresh -sticky news -padx 5 -pady 3
    grid columnconfig $fp 0 -weight 1
    grid columnconfig $fp 1 -weight 3

    # category filters
    labelframe $w.f.filter.cat -text "Object categories:"
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
        labelframe $w.f.filter.order -text "Sorting:"
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
    $w.buttons.closebutton config -command filteredobjectlist_window_close
    wm protocol $w WM_DELETE_WINDOW "$w.buttons.closebutton invoke"

    bind $fp.classentry.entry <Return> "$fp.refresh invoke"
    bind $fp.nameentry <Return> "$fp.refresh invoke"
    bind $lb <Double-Button-1> "filteredobjectlist_inspect $lb; after 500 \{raise $w; focus $lb\}"
    bind $lb <Key-Return> "filteredobjectlist_inspect $lb; after 500 \{raise $w; focus $lb\}"
    bind $lb <Button-3> "filteredobjectlist_popup %X %Y $w"
    bind $w <Escape> "$w.buttons.closebutton invoke"
    bind_runcommands $w

    focus $fp.nameentry

}

#
# Closes Filtered object dialog
#
proc filteredobjectlist_window_close {} {
    global config tmp
    set w .objdlg

    set config(filtobjlist-class)    $tmp(class)
    set config(filtobjlist-name)     $tmp(name)
    set config(filtobjlist-order)    $tmp(order)
    set config(filtobjlist-category) $tmp(category)

    destroy $w
}

# getClassNames --
#
# helper proc for filteredobjectlist_window
#
proc getClassNames {} {
    # modules and channels are not registered as classes, add them manually
    set classes [concat [opp_getchildobjects [opp_object_classes]] \
                        [opp_getchildobjects [opp_object_channeltypes]] \
                        [opp_getchildobjects [opp_object_moduletypes]] ]
    # get the names
    set classnames {}
    foreach classptr $classes {
        lappend classnames [opp_getobjectfullname $classptr]
    }
    # add classes that are not registered
    #lappend classnames ...

    return [lsort -dictionary $classnames]
}

# filteredobjectlist_refresh --
#
# helper proc for filteredobjectlist_window
#
proc filteredobjectlist_refresh {w} {
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
        if {$rootobject==[opp_null]} {
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
    multicolumnlistbox_deleteall $lb

    # insert into listbox
    if {$viewall=="ok"} {
        if {$num==$maxcount} {
            $w.f.numobj config -text "The first $num objects found:"
        } else {
            $w.f.numobj config -text "Found $num objects:"
        }
        foreach ptr $objlist {
            multicolumnlistbox_insert $lb $ptr [list ptr $ptr class [opp_getobjectclassname $ptr] name [opp_getobjectfullpath $ptr] info [opp_getobjectinfostring $ptr]] [get_icon_for_object $ptr]
        }
        set filtobjlist_state(outofdate) 0
        #$lb selection set 0
    }
}

set filtobjlist_state(outofdate) 0

#
# Called from inspectorupdate_callback whenever inspectors should be refereshed
#
proc filteredobjectlist_inspectorupdate {} {
    global filtobjlist_state
    set filtobjlist_state(outofdate) 1
}

proc filteredobjectlist_isnotsafetoinspect {} {
    global filtobjlist_state
    if {$filtobjlist_state(outofdate) || [is_running]} {
        return 1
    }
    return 0
}

# filteredobjectlist_popup --
#
# helper procedure for filteredobjectlist_window -- creates popup menu
#
proc filteredobjectlist_popup {X Y w} {
    set lb $w.f.main.list
    set ptr [lindex [multicolumnlistbox_curselection $lb] 0]
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

proc filteredobjectlist_inspect {lb} {
    set w .objdlg
    if {[filteredobjectlist_isnotsafetoinspect]} {
        if {[is_running]} {
            set advice "please stop the simulation and click Refresh first"
        } else {
            set advice "please click Refresh first"
        }
        tk_messageBox -parent $w -icon info -type ok -title {Filtered object list} \
                      -message "Dialog contents might be out of date -- $advice."
        return
    }

    inspect_item_in $lb
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

