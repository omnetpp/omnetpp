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


proc editImportPath {{fname ""}} {
    global config

    # create dialog with OK and Cancel buttons
    set w .dlg
    createOkCancelDialog $w "Edit import path"
    wm geometry $w "380x360"

    # create widgets
    label $w.f.lbl -pady 5 -justify left
    if {$fname==""} {
        $w.f.lbl config -text "Edit import path (one directory per line):"
    } else {
        $w.f.lbl config -text "Imported file\n    $fname\nwas not found in any of the current import directories -- please\nadd its location (syntax is one directory per line):"
    }
    text $w.f.txt -yscrollcommand "$w.f.sb set"
    scrollbar $w.f.sb -command "$w.f.txt yview"
    if {$fname==""} {
        button $w.f.add -text " Add directory... " -command editImportPath:addDir
    } else {
        button $w.f.add -text " Add directory... " -command [list editImportPath:addDirForFile $fname]
    }

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

        destroy $w
        return 1
    }
    destroy $w
    return 0
}

proc editImportPath:addDir {} {
    global config

    set w .dlg
    set txt $w.f.txt

    set dir [tk_chooseDirectory -title "Select directory to add to import path:" \
                                -initialdir $config(default-impdir) \
                                -parent .dlg  -mustexist true]
    if {$dir!=""} {
        set config(default-impdir) $dir
        $txt insert "insert linestart" "$dir\n"
    }
}

proc editImportPath:addDirForFile {fname} {
    global config

    set w .dlg
    set txt $w.f.txt

    set fpath [tk_getOpenFile -title "Select directory for $fname" \
                              -initialdir $config(default-impdir) \
                               -initialfile $fname \
                               -filetypes {{{NED files} {*.ned}} {{All files} {*}}} \
                               -parent .dlg]
    if {$fpath!=""} {
        set dir [file dirname $fpath]
        set config(default-impdir) $dir
        $txt insert "insert linestart" "$dir\n"
    }
}
