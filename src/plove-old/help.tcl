#==========================================================================
#  HELP.TCL -
#            part of OMNeT++
#==========================================================================

#----------------------------------------------------------------#
#  Copyright (C) 1992-2001 Andras Varga
#  Technical University of Budapest, Dept. of Telecommunications,
#  Stoczek u.2, H-1111 Budapest, Hungary.
#
#  This file is distributed WITHOUT ANY WARRANTY. See the file
#  `license' for details on this and other legal matters.
#----------------------------------------------------------------#

proc helpAbout {} {
    # implements Help|About

    tk_messageBox -title {About Plove} \
                  -icon info -type ok \
                  -message {
OMNeT++ Plove

(c) Andras Varga, 1992-2001

Tech. Univ. Budapest
Dept. of Telecommunications\

NO WARRANTY -- see license.\
    }
}


proc helpReadme {} {
    global OMNETPP_PLOVE_README

    set fname $OMNETPP_PLOVE_README
    if [catch {
        set f [open $fname r]
        set contents [read $f]
        close $f
        openTextWindow .readme "Plove README" $contents
    } errmsg] {
       tk_messageBox -icon warning -type ok -message "Error: $errmsg"
    }
}

proc openTextWindow {w title text} {

    if {[winfo exists $w]} {
        wm deiconify $w; return
    }

    toplevel $w -class Toplevel
    wm focusmodel $w passive
    wm geometry $w 512x275
    wm maxsize $w 1009 738
    wm minsize $w 1 1
    wm overrideredirect $w 0
    wm resizable $w 1 1
    wm title $w $title

    frame $w.main
    scrollbar $w.main.sb -command "$w.main.text yview"
    pack $w.main.sb -anchor center -expand 0 -fill y -side right
    text $w.main.text -yscrollcommand "$w.main.sb set"
    pack $w.main.text -anchor center -expand 1 -fill both -side left

    $w.main.text insert end $text
    $w.main.text config -state disabled

    frame $w.butt
    button $w.butt.close -text Close -command "destroy $w"
    pack $w.butt.close -anchor n -expand 0 -side right

    pack $w.butt -expand 0 -fill x -side bottom -padx 5 -pady 5
    pack $w.main -expand 1 -fill both -side top

    focus $w.main.text
}

