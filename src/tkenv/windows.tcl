#==========================================================================
#  WINDOW.TCL -
#            part of the Tcl/Tk windowing environment of
#                            OMNeT++
#==========================================================================

#----------------------------------------------------------------#
#  Copyright (C) 1992-2001 Andras Varga
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
#    MESSAGE WINDOW - IT IS NOT INSPECTOR!
#===================================================================

proc create_messagewindow {name} {
    set w $name
    if {[winfo exists $w]} {
        wm deiconify $w; return
    }
    ###################
    # CREATING WIDGETS
    ###################
    toplevel $w -class Toplevel
    wm focusmodel $w passive
    wm maxsize $w 1009 738
    wm minsize $w 1 1
    wm overrideredirect $w 0
    wm resizable $w 1 1
    wm title $w "Sent/Delivered Messages"

    frame $w.statusbar
    label $w.statusbar.frommodule -justify left -relief groove -text {From: any module}
    label $w.statusbar.tomodule -justify left -relief groove -text {To: any module}
    frame $w.main
    text $w.main.text -yscrollcommand "$w.main.sb set" -width 88 -height 15
    scrollbar $w.main.sb -command "$w.main.text yview"
    $w.main.text tag configure event -foreground blue

    ###################
    # SETTING GEOMETRY
    ###################
    pack $w.statusbar  -anchor center -expand 0 -fill x -side top
    pack $w.statusbar.frommodule -anchor n -expand 1 -fill x -side left
    pack $w.statusbar.tomodule -anchor n -expand 1 -fill x -side left
    pack $w.main -anchor center -expand 1 -fill both -side top
    pack $w.main.sb -anchor center -expand 0 -fill y -ipadx 0 -ipady 0 -padx 0 -pady 0 -side right
    pack $w.main.text -anchor center -expand 1 -fill both -side left
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

proc savefile {win filename} {
    if [catch {open $filename w} f] {
       messagebox {Error} "Error: $f" info ok
       return
    }
    if [catch {puts -nonewline $f [$win.main.text get 1.0 end]} err] {
       messagebox {Error} "Error: $err" info ok
       return
    }
    close $f
}

proc create_fileviewer {filename} {
    # Open file viewer/editor window

    # create a widget name from filename
    set w $filename
    if {$w == ""} return
    regsub {[\. ]} $w {_} w
    set w .[string tolower $w]

    if {[winfo exists $w]} {
        wm deiconify $w; return
    }
    ###################
    # CREATING WIDGETS
    ###################
    toplevel $w -class Toplevel
    wm focusmodel $w passive
    wm geometry $w 512x275
    wm maxsize $w 1009 738
    wm minsize $w 1 1
    wm overrideredirect $w 0
    wm resizable $w 1 1
    wm title $w $filename

    frame $w.butt
    button $w.butt.reload -text Reload -command "loadfile $w $filename"
    button $w.butt.save -text Save -command "savefile $w $filename"
    pack $w.butt -anchor center -expand 0 -fill x -side bottom
    pack $w.butt.reload -expand 1 -fill x -ipadx 0 -ipady 0 -padx 0 -pady 0 -side left
    pack $w.butt.save -expand 1 -fill x -ipadx 0 -ipady 0 -padx 0 -pady 0 -side left
    frame $w.main  -borderwidth 1 -relief sunken
    pack $w.main  -anchor center -expand 1 -fill both -ipadx 0 -ipady 0 -padx 0 -pady 0 -side top
    scrollbar $w.main.sb -command "$w.main.text yview"
    pack $w.main.sb -anchor center -expand 0 -fill y -ipadx 0 -ipady 0 -padx 0 -pady 0 -side right
    text $w.main.text -yscrollcommand "$w.main.sb set"
    pack $w.main.text -anchor center -expand 1 -fill both -side left

    # Read file
    loadfile $w $filename
}

