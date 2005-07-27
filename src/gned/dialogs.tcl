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


proc editImportPath {} {
    global config

    # create dialog with OK and Cancel buttons
    set w .dlg
    createOkCancelDialog $w "Edit import path"
    wm geometry $w "380x260"

    # create widgets
    label $w.f.lbl -text "Edit import path (one directory per line):" -pady 5
    text $w.f.txt -yscrollcommand "$w.f.sb set"
    scrollbar $w.f.sb -command "$w.f.txt yview"
    button $w.f.add -text " Add directory... " -command editImportPath:addDir

    # setting geometry
    $w.f config -padx 5 -pady 2
    grid $w.f.lbl -  -sticky nw
    grid $w.f.txt $w.f.sb -sticky news
    grid $w.f.add - -sticky ne -pady 5
    grid columnconfigure $w.f 0 -weight 1
    grid rowconfigure $w.f 1 -weight 1

    focus $w.f.txt

    # fill dialog
    $w.f.txt insert end [join $config(importpath) "\n"]
    $w.f.txt insert end "\n"

    # exec the dialog, extract its contents if OK was pressed, then delete dialog
    if {[execOkCancelDialog $w] == 1} {
        set dirlist [$w.f.txt get 1.0 end]
        set dirlist [string trim $dirlist]
        regsub -all -- { *\n[\n ]*} $dirlist "\n" dirlist
        set config(importpath) [split $dirlist "\n"]
    }
    destroy $w
}

proc editImportPath:addDir {} {
    global config

    set w .dlg
    set txt $w.f.txt

    set dir [tk_chooseDirectory -title "Select directory to add to import path:" \
                                -initialdir $config(default-impdir) \
                                -parent .dlg  -mustexist true]
    if {$dir!=""} {
        $txt insert "insert linestart" "$dir\n"
    }
}
