#=================================================================
#  FILEVIEW.TCL - part of
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

#===================================================================
#    FILE VIEWER/EDITOR PROCEDURES
#===================================================================

proc loadFile {win filename} {
    if [catch {
       set f [open $filename r]
       set contents [read $f]
       close $f
    } err] {
       tk_messageBox -icon warning -type ok -message "Error: $err"
       return
    }

    set t $win.main.text
    set curpos [$t index insert]
    $win.main.text delete 1.0 end
    $win.main.text insert end $contents
    catch {$t mark set insert $curpos}
    $t see insert
}

proc saveFile {win filename} {
    if [catch {open $filename w} f] {
       tk_messageBox -icon warning -type ok -message "Error: $f"
       return
    }
    if [catch {puts -nonewline $f [$win.main.text get 1.0 end]} err] {
       messagebox {Error} "Error: $err" info 0 OK
       return
    }
    close $f
}

proc createFileViewer {filename} {
    # Open file viewer/editor window

    if {$filename == ""} return

    # create a widget name from filename
    set w ".win[clock seconds]"
    if {[winfo exists $w]} {destroy $w}

    if {[winfo exists $w]} {destroy $w}

    ###################
    # CREATING WIDGETS
    ###################
    toplevel $w -class Toplevel
    wm focusmodel $w passive
    wm geometry $w 512x275
    #wm maxsize $w 1009 738
    wm minsize $w 1 1
    wm overrideredirect $w 0
    wm resizable $w 1 1
    wm title $w $filename

    frame $w.main
    scrollbar $w.main.sb -borderwidth 1 -command "$w.main.text yview"
    pack $w.main.sb -anchor center -expand 0 -fill y -ipadx 0 -ipady 0 -padx 0 -pady 0 -side right
    text $w.main.text -yscrollcommand "$w.main.sb set"
    catch {$w.main.text config -undo true}

    pack $w.main.text -anchor center -expand 1 -fill both -side left

    frame $w.butt -relief sunken
    button $w.butt.reload -text Reload -command "loadFile $w $filename"
    button $w.butt.save -text Save -command "saveFile $w $filename"
    button $w.butt.close -text Close -command "destroy $w"
    pack $w.butt.reload -anchor n -expand 0 -side right
    pack $w.butt.save -anchor n -expand 0 -side right
    pack $w.butt.close -anchor n -expand 0 -side right

    pack $w.butt -expand 0 -fill x -side bottom
    pack $w.main -expand 1 -fill both -side top

    # Read file
    loadFile $w $filename
}


# currently not used:
proc textView {w title text} {
    if {[winfo exists $w]} {
        wm deiconify $w
        return
    }

    toplevel $w -class Toplevel
    wm focusmodel $w passive
    wm geometry $w 512x275
    #wm maxsize $w 1009 738
    wm minsize $w 1 1
    wm overrideredirect $w 0
    wm resizable $w 1 1
    wm title $w $title

    frame $w.main
    scrollbar $w.main.sb -borderwidth 1 -command "$w.main.text yview"
    pack $w.main.sb -anchor center -expand 0 -fill y -ipadx 0 -ipady 0 -padx 0 -pady 0 -side right
    text $w.main.text -yscrollcommand "$w.main.sb set"
    catch {$w.main.text config -undo true}
    pack $w.main.text -anchor center -expand 1 -fill both -side left

    frame $w.butt
    button $w.butt.close -text Close -command "destroy $w"
    pack $w.butt.close -anchor n -expand 0 -side right

    pack $w.butt -expand 0 -fill x -side bottom
    pack $w.main -expand 1 -fill both -side top

    set t $w.main.text
    set curpos [$t index insert]
    $t insert end $text
    $t mark set insert 1.0
    $t see insert
}
