#==========================================================================
#  FINDREPL.TCL -
#            part of the GNED, the Tcl/Tk graphical topology editor of
#                            OMNeT++
#   By Andras Varga
#==========================================================================

#----------------------------------------------------------------#
#  Copyright (C) 1992-2001 Andras Varga
#  Technical University of Budapest, Dept. of Telecommunications,
#  Stoczek u.2, H-1111 Budapest, Hungary.
#
#  This file is distributed WITHOUT ANY WARRANTY. See the file
#  `license' for details on this and other legal matters.
#----------------------------------------------------------------#

# findReplaceDialog --
#
# mode is either find or replace
#
proc findReplaceDialog {w mode} {
    global tmp config

    set tmp(case-sensitive)  $config(editor-case-sensitive)
    set tmp(whole-words)     $config(editor-whole-words)
    set tmp(regexp)          $config(editor-regexp)

    # dialog should be child of the window which contains the text widget
    set dlg [winfo toplevel $w].dlg
    if {$dlg=="..dlg"} {set dlg .dlg}

    # create dialog with OK and Cancel buttons
    if {$mode == "find"} {
        set title "Find"
    } else {
        set title "Find/Replace"
    }
    createOkCancelDialog $dlg $title

    # add entry fields
    label-entry $dlg.f.find "Find string:"
    pack $dlg.f.find  -expand 0 -fill x -side top
    $dlg.f.find.e insert 0 $config(editor-findstring)
    $dlg.f.find.e select range 0 end

    if {$mode == "replace"} {
        label-entry $dlg.f.repl "Replace with:"
        pack $dlg.f.repl  -expand 0 -fill x -side top
        $dlg.f.repl.e insert 0 $config(editor-replacestring)
    }

    checkbutton $dlg.f.regexp -text {regular expression} -variable tmp(regexp)
    pack $dlg.f.regexp  -anchor w -side top

    checkbutton $dlg.f.case -text {case sensitive} -variable tmp(case-sensitive)
    pack $dlg.f.case  -anchor w -side top

    checkbutton $dlg.f.words -text {whole words only} -variable tmp(whole-words)
    pack $dlg.f.words  -anchor w -side top

    focus $dlg.f.find.e

    # exec the dialog, extract its contents if OK was pressed, then delete dialog
    if {[execOkCancelDialog $dlg] == 1} {

        # collect data from dialog
        set findstring [$dlg.f.find.e get]
        set case $tmp(case-sensitive)
        set words $tmp(whole-words)
        set regexp $tmp(regexp)

        set config(editor-case-sensitive) $case
        set config(editor-whole-words) $words
        set config(editor-regexp) $regexp
        set config(editor-findstring) $findstring

        if {$mode == "replace"} {
            set replstring [$dlg.f.repl.e get]
            set config(editor-replacestring) $replstring
        }

        destroy $dlg

        # execute find/replace
        if {$mode == "find"} {
            doFind $w $findstring $case $words $regexp
        } else {
            doReplace $w $findstring $replstring $case $words $regexp
        }
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

    doFind $w $findstring $case $words $regexp
}

# doFind --
#
#
proc doFind {w findstring case words regexp} {
    if {[_doFind $w $findstring $case $words $regexp] == ""} {
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
proc _doFind {w findstring case words regexp} {

    # remove previous highlights
    $w tag remove SELECT 0.0 end

    # find the string
    set cur "insert"
    while 1 {
        if {$case && $regexp} {
            set cur [$w search -count length -regexp -- $findstring $cur end]
        } elseif {$case} {
            set cur [$w search -count length -- $findstring $cur end]
        } elseif {$regexp} {
            set cur [$w search -count length -nocase -regexp -- $findstring $cur end]
        } else {
            set cur [$w search -count length -nocase -- $findstring $cur end]
        }
        if {$cur == ""} {
            break
        }
        if {!$words} {
            break
        }
        if {[$w compare $cur == "$cur wordstart"] && \
            [$w compare "$cur + $length char" == "$cur wordend"]} {
            break
        }
        set cur "$cur + 1 char"
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


# askReplaceYesNo --
#
#
proc askReplaceYesNo {w} {

    global result

    # dialog should be child of the window which contains the text widget
    set dlg [winfo toplevel $w].dlg
    if {$dlg=="..dlg"} {set dlg .dlg}

    catch {destroy $dlg}
    toplevel $dlg
    wm title $dlg "Find/Replace"
    wm protocol $dlg WM_DELETE_WINDOW { }
    wm transient $dlg [winfo toplevel [winfo parent $dlg]]

    set bbox [$w bbox insert]
    if {[llength $bbox] == 4} {
        set x [expr [winfo rootx $w] + [lindex $bbox 0] - 100 ]
        set y [expr [winfo rooty $w] + [lindex $bbox 1] + 40 ]
        wm geometry $dlg "+$x+$y"
    }

    frame $dlg.x
    label $dlg.x.bm -bitmap question
    label $dlg.x.l  -text "Replace this occurrence?"

    frame $dlg.f
    button $dlg.f.yes -text "Yes" -underline 0 \
        -command "set result yes ; destroy $dlg"
    button $dlg.f.no -text "No" -underline 0 \
        -command "set result no; destroy $dlg"
    button $dlg.f.all -text "All" -underline 0 \
        -command "set result all; destroy $dlg"
    button $dlg.f.close -text "Close" -underline 0 \
        -command "set result close; destroy $dlg"

    pack $dlg.x -side top  -fill x
    pack $dlg.x.bm -side left -padx 5  -pady 5
    pack $dlg.x.l -side top -fill x -expand 1
    pack $dlg.f -side bottom -anchor w
    pack $dlg.f.yes $dlg.f.no $dlg.f.all $dlg.f.close -side left -padx 5 -pady 5

    bind $dlg <y> "$dlg.f.yes invoke"
    bind $dlg <Y> "$dlg.f.yes invoke"
    bind $dlg <n> "$dlg.f.no invoke"
    bind $dlg <N> "$dlg.f.no invoke"
    bind $dlg <a> "$dlg.f.all invoke"
    bind $dlg <A> "$dlg.f.all invoke"
    bind $dlg <c> "$dlg.f.close invoke"
    bind $dlg <C> "$dlg.f.close invoke"
    bind $dlg <Return> "$dlg.f.yes invoke"
    bind $dlg <Escape> "$dlg.f.close invoke"
    focus $dlg.f.yes

    tkwait variable result
    return $result
}


# _doReplace --
#
# Internal proc for doReplace.
#
# Replaces the string: in text widget w, the from length chars before to the cursor
# to the cursor, with replstring, using regexp replace if regexp is nonzero, and case-sensitive
# is case is nonzero.
#
proc _doReplace {w length findstring replstring case regexp} {

    if {!$regexp} {
        $w delete "insert - $length char" "insert"
        $w insert insert $replstring
    } elseif {$regexp} {
        set foundtext [$w get "insert - $length char" "insert"]
        if {$case} {
            regsub -- $findstring $foundtext $replstring replacetext
        } else {
            regsub -nocase -- $findstring $foundtext $replstring replacetext
        }
        $w delete "insert - $length char" "insert"
        $w insert insert $replacetext
    }
}


# doReplace --
#
#
proc doReplace {w findstring replstring case words regexp} {

    while 1 {
        # find occurrence
        set length [_doFind $w $findstring $case $words $regexp]
        if {$length == ""} {
            tk_messageBox  -title "Find/Replace" -icon warning -type ok -message "'$findstring' not found."
            return
        }

        # ask whether to replace it
        set action [askReplaceYesNo $w]

        case $action in {
            yes {
                _doReplace $w $length $findstring $replstring $case $regexp
                syntaxHighlight $w "insert linestart" "insert lineend"
            }
            all {
                set count 0
                while {$length != ""} {
                    _doReplace $w $length $findstring $replstring $case $regexp
                    incr count
                    set length [_doFind $w $findstring $case $words $regexp]
                }
                syntaxHighlight $w 1.0 end ;# for multi-line replaces...
                tk_messageBox  -title "Find/Replace" -icon info -type ok -message "$count occurrences of '$findstring' replaced."

                return
            }
            no {
            }
            close {
                syntaxHighlight $w 1.0 end ;# for multi-line replaces...
                return
            }
        }
    }
}


