#==========================================================================
#  DIALOGS.TCL -
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

proc selectfromlistbox {title text list} {

    set w .listdialog
    createOkCancelDialog $w $title

    label $w.f.label -text $text -justify left
    pack $w.f.label -anchor w -expand 0 -fill none -padx 3m -pady 3m -side top

    frame $w.f.main
    scrollbar $w.f.main.sb -command "$w.f.main.list yview"
    listbox $w.f.main.list  -height 6 -yscrollcommand "$w.f.main.sb set"
    pack $w.f.main.sb -anchor center -expand 0 -fill y -side right
    pack $w.f.main.list  -anchor center -expand 1 -fill both  -side left
    pack $w.f.main  -anchor center -expand 1 -fill both -side top

    set lb $w.f.main.list
    foreach i $list {
       $lb insert end $i
    }
    $lb selection set 0

    # Configure dialog
    bind $lb <Double-Button-1> "$w.buttons.okbutton invoke"
    bind $lb <Key-Return> "$w.buttons.okbutton invoke"

    focus $lb

    if [execOkCancelDialog $w] {
       if {[$lb curselection] != ""} {
           set selection [$lb get [$lb curselection]]
       } else {
          set selection ""
       }
       destroy $w
       return $selection
    }
    destroy $w
    return ""
}

proc inspectfromlistbox_insp {lb type} {
    set sel [$lb curselection]
    if {$sel != ""} {
        set ptr [lindex [$lb get $sel] 0]
        opp_inspect $ptr $type
    }
}

proc inspectfromlistbox {title text type fillistbox_args} {

    set w .listdialog
    createOkCancelDialog $w $title

    label $w.f.label -text $text -justify left
    pack $w.f.label -anchor w -expand 0 -fill none -padx 3m -pady 3m -side top

    frame $w.f.main
    scrollbar $w.f.main.sb -command "$w.f.main.list yview"
    listbox $w.f.main.list  -height 10 -yscrollcommand "$w.f.main.sb set" -width 60
    pack $w.f.main.sb -anchor center -expand 0 -fill y -side right
    pack $w.f.main.list  -anchor center -expand 1 -fill both  -side left
    pack $w.f.main  -anchor center -expand 1 -fill both -side top

    set lb $w.f.main.list
    eval opp_fill_listbox $lb $fillistbox_args
    $lb selection set 0

    button $w.buttons.inspect -text "Select" -command "inspectfromlistbox_insp $lb \{$type\}; after 500 \{raise $w; focus $lb\}"
    pack $w.buttons.inspect -side top -anchor e -padx 2

    # Configure dialog
    bind $lb <Double-Button-1> "$w.buttons.inspect invoke"
    bind $lb <Key-Return> "$w.buttons.inspect invoke"

    focus $lb

    execOkCancelDialog $w
    destroy $w
}

proc display_stopdialog {mode} {
    # Create a dialog that can be used to stop a running simulation
    global opp fonts

    # 1. Create and configure dialog
    set w .stopdialog
    toplevel $w
    wm title $w {Running...}
    wm transient $w [winfo toplevel [winfo parent $w]]
    wm protocol $w WM_DELETE_WINDOW {opp_stopsimulation}
    # bind $w <Visibility> "raise $w"  ;# Keep modal window on top -- not good! (obscures error dialogs)

    button $w.stopbutton  -text {STOP!} -background red -activebackground red \
          -borderwidth 6 \
          -font $fonts(big)  \
          -command {opp_stopsimulation}
    if {$mode == "with_update"} {
       button $w.updatebutton  -text {Update object inspectors} \
          -borderwidth 3 \
          -command {opp_updateinspectors}
       pack $w.stopbutton -fill both -expand 1 -side top
       pack $w.updatebutton -fill x -side bottom
    } else {
       pack $w.stopbutton -fill both -expand 1
    }

    bind $w <Return> "opp_stopsimulation"
    bind $w <Escape> "opp_stopsimulation"

    # 2. Center window
    center $w

    # 3. Set a grab and claim the focus too.

    set opp(oldFocus) [focus]
#    set opp(oldGrab) [grab current $w]
#    if {$opp(oldGrab) != ""} {
#        set opp(grabStatus) [grab status $opp(oldGrab)]
#    }
#    grab $w
    focus $w.stopbutton
}

proc remove_stopdialog {} {
    # Remove the dialog created by display_stopdialog

    global opp
    set w .stopdialog

    # Restore the focus before deleting the window, since otherwise the
    # window manager may take the focus away so we can't redirect it.
    # Finally, restore any grab that was in effect.

    catch {focus $opp(oldFocus)}
    destroy $w
#    if {$opp(oldGrab) != ""} {
#        if {$opp(grabStatus) == "global"} {
#            grab -global $opp(oldGrab)
#        } else {
#            grab $opp(oldGrab)
#        }
#    }
}

proc options_dialog {} {

    global opp

    set w .optionsdialog

    createOkCancelDialog $w {Simulation options}

    frame $w.f.f1 -relief groove -borderwidth 2
    label-entry $w.f.f1.stepdelay       {Delay for slow execution}
    label-entry $w.f.f1.updfreq_fast    {Update freq. for Fast Run (events)}
    label-entry $w.f.f1.updfreq_express {Update freq. for Express Run (events)}
    $w.f.f1.stepdelay.l config -width 0
    $w.f.f1.updfreq_fast.l config -width 0
    $w.f.f1.updfreq_express.l config -width 0
    $w.f.f1.stepdelay.e config -width 8
    $w.f.f1.updfreq_fast.e config -width 8
    $w.f.f1.updfreq_express.e config -width 8
    pack $w.f.f1.stepdelay -anchor w -expand 0 -fill x
    pack $w.f.f1.updfreq_fast -anchor w -expand 0 -fill x
    pack $w.f.f1.updfreq_express -anchor w -expand 0 -fill x

    frame $w.f.f2 -relief groove -borderwidth 2
    checkbutton $w.f.f2.usemainwin -text {Use main window for module output} -variable opp(usemainwin)
    checkbutton $w.f.f2.banners -text {Print event banners} -variable opp(banners)
    checkbutton $w.f.f2.bkpts -text {Stop on breakpoint() calls} -variable opp(bkpts)
    pack $w.f.f2.usemainwin -anchor w
    pack $w.f.f2.banners -anchor w
    pack $w.f.f2.bkpts -anchor w

    frame $w.f.f3 -relief groove -borderwidth 2
    checkbutton $w.f.f3.anim -text {Animate messages} -variable opp(anim)
    checkbutton $w.f.f3.msgnam -text {Message names during animation} -variable opp(msgnam)
    checkbutton $w.f.f3.msgcol -text {Coloring by message kind} -variable opp(msgcol)
    commentlabel $w.f.f3.c {Color code (message->kind() mod 7):
     0=red 1=green 2=blue 3=white
     4=yellow 5=cyan 6=magenta 7=black}
    label-scale $w.f.f3.speed {Animation speed:}
    $w.f.f3.speed.e config -length 200 -from 0 -to 3 \
                           -resolution 0.01 -variable opp(speed)
    pack $w.f.f3.anim -anchor w
    pack $w.f.f3.msgnam -anchor w
    pack $w.f.f3.msgcol -anchor w
    pack $w.f.f3.c -anchor w
    pack $w.f.f3.speed -anchor c

    pack $w.f.f2 -anchor center -expand 1 -fill both -ipadx 0 -ipady 0 -padx 10 -pady 10 -side top
    pack $w.f.f3 -anchor center -expand 1 -fill both -ipadx 0 -ipady 0 -padx 10 -pady 10 -side top
    pack $w.f.f1 -anchor center -expand 1 -fill both -ipadx 0 -ipady 0 -padx 10 -pady 10 -side top

    # Configure dialog
    $w.f.f1.updfreq_fast.e insert 0 [opp_getsimoption updatefreq_fast]
    $w.f.f1.updfreq_express.e insert 0 [opp_getsimoption updatefreq_express]
    $w.f.f1.stepdelay.e insert 0 [opp_getsimoption stepdelay]
    set opp(usemainwin) [opp_getsimoption use_mainwindow]
    set opp(banners)    [opp_getsimoption print_banners]
    set opp(anim)       [opp_getsimoption animation_enabled]
    set opp(msgnam)     [opp_getsimoption animation_msgnames]
    set opp(msgcol)     [opp_getsimoption animation_msgcolors]
    set opp(speed)      [opp_getsimoption animation_speed]
    set opp(bkpts)      [opp_getsimoption bkpts_enabled]

    focus $w.f.f2.usemainwin

    if [execOkCancelDialog $w] {
        opp_setsimoption stepdelay [$w.f.f1.stepdelay.e get]
        opp_setsimoption updatefreq_fast [$w.f.f1.updfreq_fast.e get]
        opp_setsimoption updatefreq_express [$w.f.f1.updfreq_express.e get]
        opp_setsimoption use_mainwindow      $opp(usemainwin)
        opp_setsimoption print_banners       $opp(banners)
        opp_setsimoption animation_enabled   $opp(anim)
        opp_setsimoption animation_msgnames  $opp(msgnam)
        opp_setsimoption animation_msgcolors $opp(msgcol)
        opp_setsimoption animation_speed     $opp(speed)
        opp_setsimoption bkpts_enabled       $opp(bkpts)
    }
    destroy $w
}

proc rununtil_dialog {time_var event_var mode_var} {

    global opp
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
       $w.f.$i.l config -width 24
       $w.f.$i.e config -width 18
       pack $w.f.$i -anchor w
    }

    pack $w.f -anchor center -expand 1 -fill both -padx 10 -pady 10 -side top

    focus $w.f.time.e

    if [execOkCancelDialog $w] {
        set time_var0  [$w.f.time.e get]
        set event_var0 [$w.f.event.e get]
        set mode_var0  [lindex [$w.f.mode.e cget -text] 0]
        destroy $w
        return 1
    }
    destroy $w
    return 0
}
