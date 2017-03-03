#=================================================================
#  TOOLBUTTON.TCL - part of
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

#
# toolbutton: a button-like toolbar item with optional toggle state.
#
# We emulate button with label, because on OS X, neither tk::button nor
# ttk::button can be configured to remain flat or sunken, and we want
# a uniform implementation across platforms, for maintenance/testability.
# On Windows and Linux, a (non-ttk) button looks the same as a label
# with relief anyway.
#
# We don't need hover effect in OS X (it's not customary to have one).
#
# We need to work around OS X Tcl's bug: disabled labels don't  show the image
# (and print scary errors when hovering with the mouse): so we always keep
# the label widget enabled, and manage the state ourselves.
#

proc toolbutton {w args} {
    global icons

    if {$args=="-separator"} {
        ttk::label $w -image $icons(toolbarsep) -border 1
    } else {
        toolbutton:create $w $args
    }
    return $w
}

proc toolbutton:create {w arglist} {
    global toolbutton icons

    # defaults
    array set opts {
        -command ""
        -image $icons(16pixtransp)
        -hoverimage ""
        -pressedimage ""
        -disabledimage ""
        -state normal
        -sunken 0
        -armed 0
    }

    # overwrite with options passed in
    array set opts $arglist

    if {$opts(-hoverimage)==""} {
        set img [image create photo]
        $img copy $opts(-image)
        opp_colorizeimage $img "#ffffff" 5
        set opts(-hoverimage) $img
    }

    if {$opts(-pressedimage)==""} {
        set opts(-pressedimage) [toolbutton:getshiftedimg $opts(-image)]
    }

    if {$opts(-disabledimage)==""} {
        set img [image create photo]
        $img copy $opts(-image)
        opp_colorizeimage $img "#f0f0f0" 80
        set opts(-disabledimage) $img
    }

    # create button state variables
    foreach {key value} [array get opts] {
        set key [string range $key 1 end]
        set toolbutton($w:$key) $value
    }

    ttk::label $w -relief flat -border 1 -image $toolbutton($w:image) -class Toolbutton
    bind $w <Enter> [list toolbutton:enter $w]
    bind $w <Leave> [list toolbutton:leave $w]
    bind $w <ButtonPress-1> [list toolbutton:press $w]
    bind $w <ButtonRelease-1> [list toolbutton:release $w %X %Y]
}

proc toolbutton:getshiftedimg {srcimg} {
    # shift 1px to the right
    set w [image width $srcimg]
    set h [image height $srcimg]
    set img [image create photo -width $w -height $h]
    $img copy $srcimg -from 0 0 [expr $w-1] $h -to 1 0
    return $img
}

proc toolbutton:setstate {w state} {
    global toolbutton
    if {$state!="normal" && $state!="disabled"} {error "invalid state $state"}
    set toolbutton($w:state) $state
    toolbutton:refresh $w 0
}

proc toolbutton:getstate {w} {
    global toolbutton
    return $toolbutton($w:state)
}

proc toolbutton:setsunken {w value} {
    global toolbutton
    set toolbutton($w:sunken) $value
    toolbutton:refresh $w 0
}

proc toolbutton:issunken {w} {
    global toolbutton
    return $toolbutton($w:sunken)
}

proc toolbutton:enter {w} {
    toolbutton:refresh $w 1
}

proc toolbutton:leave {w} {
    toolbutton:refresh $w 0
}

proc toolbutton:press {w} {
    global toolbutton
    set toolbutton($w:armed) 1
    toolbutton:refresh $w 1
}

proc toolbutton:release {w x y} {
    global toolbutton
    set toolbutton($w:armed) 0
    set isover [string equal [winfo containing $x $y] $w]
    toolbutton:refresh $w $isover
    if {$isover && $toolbutton($w:state)!="disabled"} {
        eval $toolbutton($w:command)
    }
}

proc toolbutton:refresh {w mouseover} {
    global toolbutton
    if {$toolbutton($w:state)=="disabled"} {
        set img $toolbutton($w:disabledimage)
        set relief flat
    } elseif {$toolbutton($w:sunken)} {
        set img $toolbutton($w:pressedimage)
        set relief sunken
    } elseif {!$mouseover} {
        set img $toolbutton($w:image)
        set relief flat
    } elseif {$toolbutton($w:armed)} {
        set img $toolbutton($w:pressedimage)
        set relief sunken
    } else {
        set img $toolbutton($w:hoverimage)
        set isAqua [string equal [tk windowingsystem] aqua]
        set relief [expr {$isAqua?"flat":"raised"}]; # toolbar hover effect is not customary on OS X
    }
    $w config -relief $relief -image $img
}

