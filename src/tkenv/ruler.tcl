#=================================================================
#  RULER.TCL - part of
#
#                     OMNeT++/OMNEST
#            Discrete System Simulation in C++
#
#=================================================================

#----------------------------------------------------------------#
#  Copyright (C) 1992-2017 Andras Varga
#
#  This file is distributed WITHOUT ANY WARRANTY. See the file
#  `license' for details on this and other legal matters.
#----------------------------------------------------------------#


proc createRuler {c args} {
    global ruler

    canvas $c -height 18 -closeenough 5 {*}$args
    $c bind tabstop <Enter>   "$c config -cursor sb_h_double_arrow"
    $c bind tabstop <Leave>   "$c config -cursor {}"
    bind $c <Button-1>        "ruler:dragStart %W %x %y"
    bind $c <B1-Motion>       "ruler:dragMove %W %x %y"
    bind $c <ButtonRelease-1> "$c config -cursor {}"

    set ruler($c:widths) {}
    set ruler($c:titles) {}
    set ruler($c:xoffset) 0

    return $c
}

proc ruler:setColumnWidths {c widths} {
    global ruler
    set ruler($c:widths) $widths
    set ruler($c:titles) [lsetlength $ruler($c:titles) [llength $widths] ""]
    ruler:refresh $c
}

proc ruler:getColumnWidths {c} {
    global ruler
    return $ruler($c:widths)
}

proc ruler:getTabstops {c} {
    global ruler
    set tabstops {}
    set x 0
    foreach w $ruler($c:widths) {
        incr x $w
        lappend tabstops $x
    }
    return $tabstops
}

proc ruler:setColumnTitles {c titles} {
    global ruler
    set ruler($c:titles) $titles
    set ruler($c:widths) [lsetlength $ruler($c:widths) [llength $titles] 200]
    ruler:refresh $c
}

proc lsetlength {list len fillvalue} {
    if {$len==0} {return {}}
    set paddedlist [concat $list [lrepeat $len $fillvalue]]
    return [lrange $paddedlist 0 [expr $len-1]]
}

proc ruler:getColumnTitles {c} {
    global ruler
    return $ruler($c:titles)
}

proc ruler:xscroll {c txt} {
    global ruler
    set ruler($c:xoffset) [lindex [$txt dlineinfo @0,0] 0]
    ruler:refresh $c
}

proc ruler:refresh {c} {
    global ruler
    if {[llength $ruler($c:titles)] != [llength $ruler($c:widths)]} {error "ruler internal error, #titles!=#widths"}
    $c delete all
    set y1 10
    set y2 18
    set dx 8
    set n [llength $ruler($c:titles)]
    set x $ruler($c:xoffset)
    set bgcolor [$c cget -bg]
    for {set i 0} {$i < $n} {incr i} {
        set title [lindex $ruler($c:titles) $i]
        set xt [expr $x-2]
        set textcid [$c create text $xt $y2 -text "  $title       " -anchor sw]
        set rectcid [$c create rect {*}[$c bbox $textcid] -fill $bgcolor -outline ""]
        $c lower $rectcid $textcid
        if {$i > 0} {
            set x2 [expr {$x+$dx}]
            $c create line $x $y1 $x $y2 $x2 $y2 -width 2 -joinstyle miter -fill "#808080" -tags "tabstop ts[expr $i-1]"
        }
        if {$i < $n-1} {
            incr x [lindex $ruler($c:widths) $i]
        }
    }
}

proc ruler:dragStart {c x y} {
    global ruler
    set tags [$c gettags current]
    set tag [lsearch -glob -inline $tags "ts*"]
    set ruler($c:tabindex) ""
    if {$tag!=""} {
        set ruler($c:tabindex) [string range $tag 2 end]
        set ruler($c:origx) $x
        set ruler($c:origwidth) [lindex $ruler($c:widths) $ruler($c:tabindex)]
    }
}

proc ruler:dragMove {c x y} {
    global ruler
    if {$ruler($c:tabindex)==""} {return}
    set width [expr {$x - $ruler($c:origx) + $ruler($c:origwidth)}]
    ruler:setColumnWidth $c $ruler($c:tabindex) $width
}

proc ruler:setColumnWidth {c tabindex width} {
    global ruler
    if {$width < 10} {set width 10}
    if {$width > 1000} {set width 1000}
    lset ruler($c:widths) $tabindex $width
    ruler:refresh $c
    event generate $c <<Changed>>
}

