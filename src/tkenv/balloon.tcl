#=================================================================
#  BALLOON.TCL - part of
#
#                     OMNeT++/OMNEST
#            Discrete System Simulation in C++
#
#  Based on balloon.tcl, Copyright (c) by Victor Wagner, 1997
#
#=================================================================

#
# Tcl Library to provide balloon help.
# balloon help bindings are automatically added to all buttons and
# menus by init_balloon call.
# you have only to provide descriptions for buttons or menu items by
# setting elements of global help_tips array, indexed by button path or
# menu path,item to something useful.
# if you want to have balloon helps for any other widget you can
# do so by enableBalloon widget_path_or_class
# or enable_balloon_selective widget_path_or_class Tcl_Script
#
# You can toggle balloon help globally on and off by setting variable
# use_balloons to true or false
#

proc enableBalloon {name_to_bind {script {}}} {
    if ![llength $script] {
        bind $name_to_bind <Any-Enter> "+scheduleBalloon %W %X %Y %x %y"
        bind $name_to_bind <Any-Motion> "+resetBalloon %W %X %Y %x %y"
    } else {
        bind $name_to_bind <Any-Enter> "+scheduleBalloon %W %X %Y %x %y \[$script\]"
        bind $name_to_bind <Any-Motion> "+resetBalloon %W %X %Y %x %y \[$script\]"
    }
    bind $name_to_bind <Any-Leave> "+cancelBalloon"
}

proc scheduleBalloon {window x y winx winy {item {}}} {
    global use_balloons help_tips balloon_after_ID
    if !$use_balloons return

    if [info exists help_tips(helptip_proc)] {
        set tip [$help_tips(helptip_proc) $window $winx $winy]
        if [string length $tip] {
            set balloon_after_ID [after $help_tips(delay) [list createBalloon $window $tip $x $y]]
            return
        }
    }
    if [string length $item] {
        set index "$window,$item"
    } else {
        set index $window
    }
    if [info exists help_tips($index)] {
        set balloon_after_ID [after $help_tips(delay) [list createBalloon $window $help_tips($index) $x $y]]
    }
}

proc resetBalloon {window x y winx winy {item {}}} {
    cancelBalloon
    scheduleBalloon $window $x $y $winx $winy $item
}

proc cancelBalloon {} {
    global balloon_after_ID
    if [info exists balloon_after_ID] {
        after cancel $balloon_after_ID
        unset balloon_after_ID
    } else {
        if [winfo exists .balloon_help] {destroy .balloon_help}
    }
}

proc createBalloon {window text x y} {
    global balloon_after_ID help_tips

    if {![winfo exists $window]} return

    set window [winfo toplevel $window]

    catch {destroy .balloon_help}
    toplevel .balloon_help -relief flat
    catch {unset balloon_after_ID}
    wm overrideredirect .balloon_help true
    wm positionfrom .balloon_help program

    set l .balloon_help.tip
    label $l -text $text -padx 4 -wraplength $help_tips(width) -bg $help_tips(color) \
             -border 1 -relief solid -font TkTooltipFont -justify left

    set balloonrect [list [expr $x-5] [expr $y+16] [winfo reqwidth $l] [winfo reqheight $l]]
    set windowrect [list [winfo rootx $window] [winfo rooty $window] [winfo width $window] [winfo height $window]]
    setvars {balloonx balloony} [moveRectIntoRect $balloonrect $windowrect]

    wm geometry .balloon_help "+$balloonx+$balloony"

    if {[string equal [tk windowingsystem] aqua]}  {
        # from http://wiki.tcl.tk/3060
        catch { ::tk::unsupported::MacWindowStyle style .balloon_help help none }
    }

    pack .balloon_help.tip
}

proc initBalloons {args} {
    global help_tips use_balloons

    set help_tips(width) 600
    set help_tips(color) #f0f0d0
    set help_tips(delay) 500
    set help_tips(helptip_proc) getHelpTip
    set use_balloons 1

    foreach c {Button TButton Label TLabel Entry TEntry Checkbutton TCheckbutton
               Radiobutton TRadiobutton TCombobox Toolbutton Scale TScale Text
               Menubutton} {
        enableBalloon $c
    }
    enableBalloon Menu "%W index active"
    enableBalloon Canvas "%W find withtag current"  ;#DO NOT CHANGE THIS
}

#package provide balloon 1.0

