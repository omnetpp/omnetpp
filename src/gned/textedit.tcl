#==========================================================================
#  TEXTEDIT.TCL -
#            part of the GNED, the Tcl/Tk graphical topology editor of
#                            OMNeT++
#   By Andras Varga
#==========================================================================

#----------------------------------------------------------------#
#  Copyright (C) 1992,98 Andras Varga
#  Technical University of Budapest, Dept. of Telecommunications,
#  Stoczek u.2, H-1111 Budapest, Hungary.
#
#  This file is distributed WITHOUT ANY WARRANTY. See the file
#  `license' for details on this and other legal matters.
#----------------------------------------------------------------#

# $keywords: list of highlighted NED keywords
#
# Problematic keywords: "in:" and "out:" (contain special chars)
# This is all one line, cannot be broken into several lines!
#
set keywords {include|import|network|module|simple|channel|delay|error|datarate|for|do|true|false|ref|ancestor|input|const|sizeof|endsimple|endmodule|endchannel|endnetwork|endfor|parameters|gates|gatesizes|in:|out:|submodules|connections|display|on|like|machines|to|if|index|nocheck|numeric|string|bool|anytype}


# configTextForSyntaxHightlight --
#
# Create tags and bondings for NED editor text widget.
#
proc configTextForSyntaxHightlight {w} {

    puts "dbg: improve efficiency of syntax highlight!"
    # should only update current and previous line
    # "paste" should update the whole file
    bind $w <Key> {after idle {syntaxHighlight %W 1.0 end}}

    $w tag configure KEYWORD -foreground #a00000
    $w tag configure COMMENT -foreground #808080
    $w tag configure STRING  -foreground #00ff00  ;# not yet used
}


# syntaxHightlight --
#
# Applies NED syntax highlight to the text widget passed.
# Should be used like this:
#   bind $w <Key> {after idle {syntaxHighlight %W 1.0 end}}
#
proc syntaxHighlight {w startpos endpos} {

    global keywords

    $w tag remove KEYWORD $startpos $endpos
    $w tag remove COMMENT $startpos $endpos

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











