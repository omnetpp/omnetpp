#=================================================================
#  FILTCFG.TCL - part of
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

proc filtCfgDel {lb} {
    global filt g

    set name [lindex [$lb get active] end]
    if {$name == ""} return

    set ans [tk_messageBox -message "Remove filter \"$name\"?" -title "Confirm" -icon question -type yesno -parent [winfo parent $lb]]
    if {$ans == "no"} {return}

    set pos [lsearch $filt(names) $name]
    if {$pos == -1} return

    set filt(names) [lreplace $filt(names) $pos $pos]
    unset filt($name,descr)
    unset filt($name,prefix)
    unset filt($name,type)
    unset filt($name,expr)

    $lb delete active
}

proc filtCfgDup {lb} {
    global filt g

    set name [lindex [$lb get active] end]
    if {$name == ""} return

    set pos [lsearch $filt(names) $name]
    if {$pos == -1} return

    set oldname $name
    append name "'"
    while {[lsearch $filt(names) $name]!=-1} {
        append name "'"
    }

    lappend filt(names) $name
    set filt($name,descr) $filt($oldname,descr)
    set filt($name,prefix) $filt($oldname,prefix)
    set filt($name,type)  $filt($oldname,type)
    set filt($name,expr)  $filt($oldname,expr)

    $lb insert active "$name - $filt($name,descr) $g(spaces) $name"
}

proc filtCfgNew {lb} {
    global filt g

    set name "new"
    while {[lsearch $filt(names) $name]!=-1} {
        append name "'"
    }
    lappend filt(names)   $name
    set filt($name,descr) "new filter"
    set filt($name,prefix) $name
    set filt($name,type)  expr
    set filt($name,expr)  "x+0"

    $lb insert active "$name - $filt($name,descr) $g(spaces) $name"
}

proc editFilterConfig {} {

    global filt g

    set w .filtcfg

    toplevel $w -class Toplevel
    wm title $w "Filter management"
    wm iconname $w Dialog
    wm focusmodel $w passive
    wm overrideredirect $w 0
    wm resizable $w 1 1
    wm deiconify $w

    #
    # listbox
    #
    label $w.l -anchor w -justify left -text "Here you can view, modify or create filters that can be used with any vector.\n\nAvailable filters:"
    frame $w.f
    listbox $w.f.list -width 50 -yscrollcommand "$w.f.sby set"
    scrollbar $w.f.sby -borderwidth 1 -command "$w.f.list yview"

    set lb $w.f.list

    pack $w.l -anchor w -side top -pady 3
    pack $w.f -anchor center -expand 1 -fill both -side top -padx 10 -pady 5
    pack $w.f.sby -anchor s -expand 0 -fill y -side right
    pack $w.f.list  -anchor center -expand 1 -fill both -side left

    #
    # buttons
    #
    frame $w.b -width 40
    pack $w.b -expand 0 -anchor e -side bottom -padx 5 -pady 5
    button $w.b.new   -text New...  -command "filtCfgNew $lb"   -width 7
    button $w.b.dup   -text Clone   -command "filtCfgDup $lb"   -width 7
    button $w.b.edit  -text Edit... -command "filtCfgEdit $lb"  -width 7
    button $w.b.del   -text Remove  -command "filtCfgDel $lb"   -width 7
    button $w.b.close -text Close   -command "set tmp(butt) 1"  -width 7

    grid $w.b.new $w.b.dup $w.b.edit  $w.b.del  -padx 2 -pady 3
    grid x        x         x        $w.b.close -padx 2 -pady 3

    #
    # fill dialog
    #
    foreach i $filt(names) {
       $lb insert end "$i - $filt($i,descr) $g(spaces) $i"
    }

    focus $lb


    #
    # bindings
    #
    global tmp
    bind $lb <Double-1> "filtCfgEdit $lb"
    bind $lb <3> "filtCfgEdit $lb"
    bind $w <Escape> {set tmp(butt) 1}
    bind $w <Return> "filtCfgEdit $lb"
    bind $w <Delete> "filtCfgDel $lb"
    wm protocol $w WM_DELETE_WINDOW {set tmp(butt) 1}

    center $w

    set oldGrab [grab current $w]
    if {$oldGrab != ""} {
        set grabStatus [grab status $oldGrab]
    }
    grab $w

    # Wait for the user to respond, then restore the focus and
    # return the index of the selected button.  Restore the focus
    # before deleting the window, since otherwise the window manager
    # may take the focus away so we can't redirect it.  Finally,
    # restore any grab that was in effect.

    tkwait variable tmp(butt)

    if {$oldGrab != ""} {
        if {$grabStatus == "global"} {
            grab -global $oldGrab
        } else {
            grab $oldGrab
        }
    }
    destroy $w
}

proc filtCfgEdit {lb} {

    global filt g

    set name [lindex [$lb get active] end]
    if {$name == ""} return

    set pos [lsearch $filt(names) $name]
    if {$pos == -1} return

    createOkCancelDialog .fedit "Filter Options"

    # add entry fields and focus on first one
    frame .fedit.f.f0
    label-entry .fedit.f.f0.name "Name:" $name
    label-entry .fedit.f.f0.descr "Description:" $filt($name,descr)
    label-entry .fedit.f.f0.pref "Vector title prefix:" $filt($name,prefix)
    pack .fedit.f.f0.name -anchor center -expand 0 -fill x -side top
    pack .fedit.f.f0.descr -anchor center -expand 0 -fill x -side top
    pack .fedit.f.f0.pref -anchor center -expand 0 -fill x -side top
    pack .fedit.f.f0 -anchor center -expand 0 -fill x -side top

    set t $filt($name,type)
    set e $filt($name,expr)
    if {$t == "expr"} {set ex $e} else {set ex ""}
    if {$t == "awk"}  {set aw $e} else {set aw ""}
    if {$t == "prog"} {set pr $e} else {set pr ""}

    global tmp
    set tmp(radio) $t

    frame .fedit.f.f1
    radiobutton .fedit.f.f1.r -text {Awk expression} -value expr \
          -variable tmp(radio) -command "focus .fedit.f.f1.expr.e"
    label-entry .fedit.f.f1.expr "Expression:  x=" $ex
    pack .fedit.f.f1.r -anchor w -expand 0 -side top
    pack .fedit.f.f1.expr -anchor w -expand 0 -fill x -side top
    pack .fedit.f.f1 -anchor center -expand 0 -fill x -side top

    frame .fedit.f.f2
    radiobutton .fedit.f.f2.r -text {Awk program} -value awk \
          -variable tmp(radio) -command "focus .fedit.f.f2.expr.t"
    label-text .fedit.f.f2.expr "Awk program:" 4 $aw
    pack .fedit.f.f2.r -anchor w -expand 0 -side top
    pack .fedit.f.f2.expr -anchor w -expand 1 -fill both -side top
    pack .fedit.f.f2 -anchor center -expand 1 -fill both -side top

    frame .fedit.f.f3
    radiobutton .fedit.f.f3.r -text {External program} -value prog \
          -variable tmp(radio) -command "focus .fedit.f.f3.expr.e"
    label-entry .fedit.f.f3.expr "Program w/ pars:" $pr
    pack .fedit.f.f3.r -anchor w -expand 1 -side top
    pack .fedit.f.f3.expr -anchor w -expand 0 -fill x -side top
    pack .fedit.f.f3 -anchor center -expand 0 -fill x -side top

    frame .fedit.f.f4
    commentlabel .fedit.f.f4.c \
{Awk expressions and programs can use the t,x variables which will be
replaced with $2 and $3 when the filter is executed.  All three types
of filters can contain parameters of the form $(par); their values can
be entered when the filter is selected for a vector. The $(par) strings
will be replaced by the actual numbers when the filter is executed.}
    pack .fedit.f.f4 -anchor center -expand 0 -fill x -side top
    pack .fedit.f.f4.c -anchor w -expand 0 -fill x -side top

    focus .fedit.f.f0.name.e

    bind .fedit.f.f1.expr.e <Key> ".fedit.f.f1.r select"
    bind .fedit.f.f2.expr.t <Key> ".fedit.f.f2.r select"
    bind .fedit.f.f3.expr.e <Key> ".fedit.f.f3.r select"

    # exec the dialog, extract its contents if OK was pressed, then delete it
    if {[execOkCancelDialog .fedit] == 1} {
        set oldname $name
        set name [.fedit.f.f0.name.e get]

        set pos [lsearch $filt(names) $oldname]
        set filt(names) [lreplace $filt(names) $pos $pos $name]

        unset filt($oldname,descr)
        unset filt($oldname,prefix)
        unset filt($oldname,type)
        unset filt($oldname,expr)

        set filt($name,descr) [.fedit.f.f0.descr.e get]
        set filt($name,prefix) [.fedit.f.f0.pref.e get]
        if {$filt($name,prefix) == ""} {set filt($name,prefix) $name}
        set filt($name,type)  $tmp(radio)

        if {$filt($name,type)=="expr"} {
            set filt($name,expr)  [.fedit.f.f1.expr.e get]
        } elseif {$filt($name,type)=="awk"} {
            set filt($name,expr)  [.fedit.f.f2.expr.t get 1.0 end]
        } elseif {$filt($name,type)=="prog"} {
            set filt($name,expr)  [.fedit.f.f3.expr.e get]
        } else {
            error "Unknown type $filt($name,type)"
        }

        $lb delete active
        $lb insert active "$name - $filt($name,descr) $g(spaces) $name"
    }

    destroy .fedit
}

