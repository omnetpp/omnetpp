#=================================================================
#  TEXTEDIT.TCL - part of
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

# $keywords: list of highlighted NED keywords
#
# Problematic keywords: "in:" and "out:" (contain special chars)
# This is all one line, cannot be broken into several lines!
#
set keywords {include|import|network|module|simple|channel|delay|error|datarate|for|do|true|false|ref|ancestor|input|const|sizeof|endsimple|endmodule|endchannel|endnetwork|endfor|parameters|gatesizes|gates|in:|out:|submodules|connections|display|on|like|machines|to|if|index|nocheck|numeric|xml|xmldoc|string|bool|anytype}


# configureEditor --
#
# Create tags and bindings for NED editor text widget.
#
proc configureEditor {w} {

    # bindings for syntax highlight and status bar update
    $w tag configure SELECT -back #808080 -fore #ffffff

    bind $w <Key> {
        %W tag remove SELECT 0.0 end
        after idle {
            syntaxHighlight %W {insert linestart - 1 lines} {insert lineend}
            updateTextStatusbar %W
        }
    }
    bind $w <Button-1> {
        %W tag remove SELECT 0.0 end
        after idle {
            updateTextStatusbar %W
        }
    }

    # bindings for find/replace
    #   'break' is needed below because
    #      ^H is originally bound to Backspace ('Text' tag), and
    #      ^F is originally bound to 1 char right
    #      ^N is originally bound to 1 line down
    bind $w <Control-f> {editFind;break}
    bind $w <Control-F> {editFind;break}
    bind $w <Control-h> {editReplace;break}
    bind $w <Control-H> {editReplace;break}
    bind $w <F3>        {editFindNext}
    bind $w <Control-n> {editFindNext;break}
    bind $w <Control-N> {editFindNext;break}

    bind $w <<Cut>>     {after idle {syntaxHighlight %W 1.0 end}}
    bind $w <<Paste>>   {after idle {syntaxHighlight %W 1.0 end}}
    bind $w <<Undo>>    {after idle {syntaxHighlight %W 1.0 end}}
    bind $w <<Redo>>    {after idle {syntaxHighlight %W 1.0 end}}

}

# syntaxHighlight --
#
# Applies NED syntax highlight to the text widget passed.
# Should be used like this:
#   bind $w <Key> {after idle {syntaxHighlight %W 1.0 end}}
#
proc syntaxHighlight {w {startpos {1.0}} {endpos end}} {

    #
    # BUG: if the end of a string constant falls into a comment,
    # highlighting will be wrong...
    #
    global keywords

    # setting up tags here is slightly redundant, but the advantage is to be
    # able to call syntaxHighlight on any text widget without any preparation
    $w tag configure KEYWORD -foreground #a00000
    $w tag configure STRING  -foreground #008000
    $w tag configure COMMENT -foreground #808080

    # remove existing tags...
    $w tag remove KEYWORD $startpos $endpos
    $w tag remove COMMENT $startpos $endpos
    $w tag remove STRING  $startpos $endpos

    # string constants...
    set cur $startpos
    while 1 {
        set cur [$w search -count length -regexp {"[^"]*"} $cur $endpos]
        if {$cur == ""} {
            break
        }
        $w tag add STRING $cur "$cur + $length char"
        set cur [$w index "$cur + $length char"]
    }

    # keywords...
    set cur $startpos
    while 1 {
        set cur [$w search -count length -regexp $keywords $cur $endpos]
        if {$cur == ""} {
            break
        }

        if {[$w compare $cur == "$cur wordstart"] && \
            [$w compare "$cur + $length char" == "$cur wordend"]} {
            $w tag add KEYWORD $cur "$cur + $length char"
        }
        set cur [$w index "$cur + $length char"]
    }

    # comments...
    set cur $startpos
    while 1 {
        set cur [$w search -count length -regexp {//.*$} $cur $endpos]
        if {$cur == ""} {
            break
        }
        $w tag add COMMENT $cur "$cur + $length char"
        set cur [$w index "$cur + $length char"]
    }
}

# updateTextStatusbar --
#
#
proc updateTextStatusbar {w} {
    global gned

    set linecol [split [$w index insert] "."]
    set line [lindex $linecol 0]
    set col  [expr [lindex $linecol 1] + 1]
    $gned(statusbar).mode config -text "Line $line Col $col"
}


