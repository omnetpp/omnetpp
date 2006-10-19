#=================================================================
#  WINDOWS.TCL - part of
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
#    MESSAGE WINDOW - IT IS NOT INSPECTOR!
#===================================================================

proc create_messagewindow {name} {
    set w $name
    if {[winfo exists $w]} {
        wm deiconify $w; return
    }

    global icons

    # creating widgets
    toplevel $w -class Toplevel
    wm focusmodel $w passive
    #wm maxsize $w 1009 738
    wm minsize $w 1 1
    wm overrideredirect $w 0
    wm resizable $w 1 1
    wm title $w "Sent/Delivered Messages"

    frame $w.toolbar
    textwindow_add_icons $w

    frame $w.main
    text $w.main.text -yscrollcommand "$w.main.sb set" -width 88 -height 15
    scrollbar $w.main.sb -command "$w.main.text yview"
    $w.main.text tag configure event -foreground blue

    # setting geometry
    pack $w.toolbar  -anchor center -expand 0 -fill x -side top
    pack $w.main -anchor center -expand 1 -fill both -side top
    pack $w.main.sb -anchor center -expand 0 -fill y -ipadx 0 -ipady 0 -padx 0 -pady 0 -side right
    pack $w.main.text -anchor center -expand 1 -fill both -side left

    $w.main.text tag configure SELECT -back #808080 -fore #ffffff

    # keyboard bindings
    bind_runcommands $w
    bind_findcommands_to_textwidget $w.main.text

}

#===================================================================
#    CONSOLE WINDOW
#===================================================================

proc show_console {} {
    set w .con
    if {[winfo exists .con]} {
        wm deiconify .con; return
    }
    toplevel .con -class vTcl
    wm minsize .con 375 160
    wm title .con "Tcl Console"
    frame .con.fra5 \
        -height 30 -width 30
    pack .con.fra5 \
        -anchor center -expand 1 -fill both -ipadx 0 -ipady 0 -padx 2 -pady 2 \
        -side top
    text .con.fra5.tex7 \
        -highlightthickness 0 -state disabled -width 50 -height 6 \
        -yscrollcommand {.con.fra5.scr8 set}
    .con.fra5.tex7 tag configure command -underline 1
    .con.fra5.tex7 tag configure error -foreground red
    .con.fra5.tex7 tag configure output
    pack .con.fra5.tex7 \
        -anchor center -expand 1 -fill both -ipadx 0 -ipady 0 -padx 0 -pady 0 \
        -side left
    scrollbar .con.fra5.scr8 \
        -command {.con.fra5.tex7 yview} -highlightthickness 0
    pack .con.fra5.scr8 \
        -anchor center -expand 0 -fill y -ipadx 0 -ipady 0 -padx 0 -pady 0 \
        -side right
    frame .con.fra6 \
        -height 30 -width 30
    pack .con.fra6 \
        -anchor center -expand 0 -fill both -ipadx 0 -ipady 0 -padx 0 -pady 0 \
        -side top
    entry .con.fra6.ent10 \
        -highlightthickness 0
    pack .con.fra6.ent10 \
        -anchor center -expand 0 -fill x -ipadx 0 -ipady 0 -padx 2 -pady 2 \
        -side top
    bind .con.fra6.ent10 <Key-Return> {
        .con.fra5.tex7 conf -state normal
        .con.fra5.tex7 insert end \n[.con.fra6.ent10 get] command
        if { [catch [.con.fra6.ent10 get] output] == 1 } {
            .con.fra5.tex7 insert end "\n$output" error
        } else {
            .con.fra5.tex7 insert end "\n$output" output
        }
        .con.fra5.tex7 conf -state disabled
        .con.fra5.tex7 yview end
        .con.fra6.ent10 delete 0 end
    }
    focus .con.fra6.ent10
}

#===================================================================
#    FILE VIEWER/EDITOR PROCEDURES
#===================================================================

proc loadfile {win filename} {
    if [catch {open $filename r} f] {
       messagebox {Error} "Error: $f" info ok
       return
    }
    set contents [read $f]
    close $f

    set t $win.main.text
    set curpos [$t index insert]
    $win.main.text delete 1.0 end
    $win.main.text insert end $contents
    catch {$t mark set insert $curpos}
    $t see insert
}

proc savefile {win {filename ""}} {
    global config

    if {$filename == ""} {
        set filename $config(log-save-filename)
        set filename [tk_getSaveFile -title {Save Log Window Contents} -parent $win \
                      -defaultextension "out" -initialfile $filename \
                      -filetypes {{{Log files} {*.out}} {{All files} {*}}}]
        if {$filename == ""} return
        set config(log-save-filename) $filename
    }

    if [catch {
       set f [open $filename w]
       set txt $win.main.text
       if {$txt == "..main.text"} {set txt .main.text}
       puts -nonewline $f [$txt get 1.0 end]
       close $f
    } err] {
       messagebox {Error} "Error: $err" info ok
    }
}

#
# Open file viewer window
#
proc create_fileviewer {filename} {
    global icons help_tips

    if {$filename == ""} return

    # create a widget name from filename
    set w ".win[clock seconds]"
    if {[winfo exists $w]} {destroy $w}

    # creating widgets
    toplevel $w -class Toplevel
    wm focusmodel $w passive
    #wm maxsize $w 1009 738
    wm minsize $w 1 1
    wm overrideredirect $w 0
    wm resizable $w 1 1
    wm title $w $filename

    frame $w.toolbar

    pack_iconbutton $w.toolbar.copy   -image $icons(copy) -command "edit_copy $w.main.text"
    pack_iconbutton $w.toolbar.find   -image $icons(find) -command "findDialog $w.main.text"
    pack_iconbutton $w.toolbar.sep20  -separator
    pack_iconbutton $w.toolbar.save   -image $icons(save) -command "savefile $w $filename"
    pack_iconbutton $w.toolbar.sep21  -separator

    set help_tips($w.toolbar.copy)   {Copy selected text to clipboard (Ctrl+C)}
    set help_tips($w.toolbar.find)   {Find string in window (Ctrl+F}
    set help_tips($w.toolbar.save)   {Save window contents to file}


    pack $w.toolbar  -anchor center -expand 0 -fill x -side top

    frame $w.main  -borderwidth 1 -relief sunken
    pack $w.main  -anchor center -expand 1 -fill both -ipadx 0 -ipady 0 -padx 0 -pady 0 -side top
    scrollbar $w.main.sb -command "$w.main.text yview"
    pack $w.main.sb -anchor center -expand 0 -fill y -ipadx 0 -ipady 0 -padx 0 -pady 0 -side right
    text $w.main.text -yscrollcommand "$w.main.sb set" -width 88 -height 30
    pack $w.main.text -anchor center -expand 1 -fill both -side left

    $w.main.text tag configure SELECT -back #808080 -fore #ffffff
    bind_findcommands_to_textwidget $w.main.text

    # Read file
    loadfile $w $filename
}

