#=================================================================
#  FINDREPL.TCL - part of
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

# findReplaceDialog --
#
# mode is either find or replace
#
proc findReplaceDialog {w mode} {
    global tmp config

    set tmp(case-sensitive)  $config(editor-case-sensitive)
    set tmp(whole-words)     $config(editor-whole-words)
    set tmp(regexp)          $config(editor-regexp)
    set tmp(backwards)       $config(editor-backwards)

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

    checkbutton $dlg.f.backwards -text {search backwards} -variable tmp(backwards)
    pack $dlg.f.backwards  -anchor w -side top

    focus $dlg.f.find.e

    # exec the dialog, extract its contents if OK was pressed, then delete dialog
    if {[execOkCancelDialog $dlg] == 1} {

        # collect data from dialog
        set findstring [$dlg.f.find.e get]
        set case $tmp(case-sensitive)
        set words $tmp(whole-words)
        set regexp $tmp(regexp)
        set backwards $tmp(backwards)

        set config(editor-case-sensitive) $case
        set config(editor-whole-words) $words
        set config(editor-regexp) $regexp
        set config(editor-backwards) $backwards
        set config(editor-findstring) $findstring

        if {$mode == "replace"} {
            set replstring [$dlg.f.repl.e get]
            set config(editor-replacestring) $replstring
        }

        destroy $dlg

        # execute find/replace
        if {$mode == "find"} {
            doFind $w $findstring $case $words $regexp $backwards
        } else {
            doReplace $w $findstring $replstring $case $words $regexp $backwards
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
proc _doReplace {w length findstring replstring case regexp backwards} {

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
proc doReplace {w findstring replstring case words regexp backwards} {

    while 1 {
        # find occurrence
        set length [_doFind $w $findstring $case $words $regexp $backwards]
        if {$length == ""} {
            tk_messageBox  -title "Find/Replace" -icon warning -type ok -message "'$findstring' not found."
            return
        }

        # ask whether to replace it
        set action [askReplaceYesNo $w]

        case $action in {
            yes {
                _doReplace $w $length $findstring $replstring $case $regexp $backwards
                syntaxHighlight $w "insert linestart" "insert lineend"
            }
            all {
                set count 0
                while {$length != ""} {
                    _doReplace $w $length $findstring $replstring $case $regexp $backwards
                    incr count
                    set length [_doFind $w $findstring $case $words $regexp $backwards]
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


