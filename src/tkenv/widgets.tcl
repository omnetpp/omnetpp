#=================================================================
#  WIDGETS.TCL - part of
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
#    STARTUP PROCEDURES
#===================================================================

# checkTclTkVersion --
#
# Check required Tcl/Tk version
#
proc checkTclTkVersion {} {

   global tk_version tk_patchLevel

   catch {package require Tk}
   if {[string compare $tk_patchLevel "8.4.0"]<0} {
      wm deiconify .
      wm title . "Bad news..."
      frame .f
      pack .f -expand 1 -fill both -padx 2 -pady 2
      label .f.l1 -text "Your version of Tcl/Tk is too old, please upgrade!"
      label .f.l2 -text "Tcl/Tk 8.4.0 or later required."
      button .f.b -text "  OK  " -command {exit}
      pack .f.l1 .f.l2 -side top -padx 5
      pack .f.b -side top -pady 5
      focus .f.b
      wm protocol . WM_DELETE_WINDOW {exit}
      tkwait variable ok
   }
}


# setupTkOptions --
#
# Sets up fonts and various options in Tk in order to have consistent look
# on Unix/Windows and across different Tk versions.
#
proc setupTkOptions {} {
   global fonts tcl_platform tk_version
   global tcl_wordchars tcl_nonwordchars
   global HAVE_BLT

   # test for BLT
   set HAVE_BLT 0
   catch {package require BLT; set HAVE_BLT 1}

   # load combobox
   package require combobox 2.3

   # work around Tcl bug: these vars got reset when words.tcl was autoloaded
   catch {tcl_wordBreakAfter};
   set tcl_wordchars {\w}
   set tcl_nonwordchars {\W}

   # by default, undo/redo bindings are platform-specific -- change it:
   catch {
       event add <<Undo>> <Control-Key-z>
       event add <<Undo>> <Control-Key-Z>
       event add <<Redo>> <Control-Key-y>
       event add <<Redo>> <Control-Key-Y>
   }

   # for some reason, Ctrl+v (Paste) doesn't work out-of-the box with Tk 8.3/8.4 on Unix,
   # we need the following lines:
   bind Entry <Control-v> {}
   bind Text <Control-v> {}

   # set up wheel support for a few extra widget types
   bindMouseWheel Canvas
   bindMouseWheel TreeView

   #
   # fonts() array elements:
   #  normal:  menus, labels etc
   #  bold:    buttons
   #  big:     STOP button
   #  msgname: message name during animation
   #  fixed:   text windows and listboxes
   #

   if {$tcl_platform(platform) == "unix"} {
      set fonts(normal)  -Adobe-Helvetica-Medium-R-Normal-*-12-*-*-*-*-*-*-*
      set fonts(bold)    -Adobe-Helvetica-Bold-R-Normal-*-12-*-*-*-*-*-*-*
      set fonts(big)     -Adobe-Helvetica-Medium-R-Normal-*-18-*-*-*-*-*-*-*
      set fonts(msgname) -Adobe-Helvetica-Medium-R-Normal-*-12-*-*-*-*-*-*-*
      set fonts(fixed)   fixed
      set fonts(balloon) -Adobe-Helvetica-Medium-R-Normal-*-12-*-*-*-*-*-*-*
   } else {
      # Windows, Mac
      if {$tk_version<8.2} {
         set s 140
      } else {
         set s 110
      }
      font create opp_normal -family "MS Sans Serif" -size 8
      font create opp_bold   -family "MS Sans Serif" -size 8 -weight bold
      font create opp_balloon -family "MS Sans Serif" -size 8

      set fonts(normal)  opp_normal
      set fonts(bold)    opp_bold
      set fonts(big)     -Adobe-Helvetica-Medium-R-Normal-*-*-180-*-*-*-*-*-*
      set fonts(msgname) -Adobe-Helvetica-Medium-R-Normal-*-*-$s-*-*-*-*-*-*
      set fonts(fixed)   FixedSys
      set fonts(balloon) opp_balloon

      option add *TreeView.font  $fonts(normal)
   }

   if {$tcl_platform(platform) == "unix"} {
       option add *Scrollbar.width  12
       option add *Menubutton.font  $fonts(normal)
       option add *Menu.font        $fonts(normal)
       option add *Label.font       $fonts(normal)
       option add *Entry.font       $fonts(normal)
       option add *Listbox.font     $fonts(fixed)
       option add *Text.font        $fonts(fixed)
       option add *TreeView.font    $fonts(fixed)
       option add *Button.font      $fonts(bold)

       # make menus look more contemporary
       menu .tmp
       set activebg [.tmp cget -activebackground]
       set activefg [.tmp cget -activeforeground]
       destroy .tmp
       option add *Menu.activeBorderWidth 0
       option add *Menu.relief raised
       option add *Menu.activeBackground #800000
       option add *Menu.activeForeground white
       option add *menubar.borderWidth 1
       option add *menubar.activeBorderWidth 1
       option add *menubar.activeBackground $activebg
       option add *menubar.activeForeground $activefg
   }
}


#===================================================================
#    UTILITY PROCEDURES
#===================================================================

# wsize --
#
# Utility to set a widget's size to exactly width x height pixels.
# Usage example:
#    button .b1 -text "OK"
#    pack [wsize .b1 40 40] -side top -expand 1 -fill both
#
proc wsize {w width height} {
  set f ${w}_f
  frame $f -width $width -height $height
  place $w -in $f -x 0 -y 0 -width $width -height $height
  raise $w
  return $f
}


#===================================================================
#    PROCEDURES FOR CREATING NEW 'WIDGET TYPES'
#===================================================================

proc iconbutton {w args} {
    global fonts

    if {$args=="-separator"} {
        # space between two buttons
        frame $w -height 1 -width 4
        #W2K: frame $w -height 23 -width 2 -bd 1 -relief groove
    } {
        # button
        eval button $w -bd 1 $args
        #W2K: eval button $w -bd 1 -relief flat $args
        #W2K: bind $w <Enter> [list $w config -relief raised]
        #W2K: bind $w <Leave> [list $w config -relief flat]
    }
    return $w
}

proc pack_iconbutton {w args} {
    eval iconbutton $w $args
    pack $w -anchor n -side left -padx 0 -pady 2
}

proc rpack_iconbutton {w args} {
    eval iconbutton $w $args
    pack $w -anchor n -side right -padx 0 -pady 2
}

proc config_iconbutton {w icon command tooltip} {
    global help_tips

    $w config -image $icon
    $w config -command $command
    set help_tips($w) $tooltip
}


proc combo {w list {cmd {}}} {
    # implements a combo box widget (which is missing from Tk)
    # using a menubutton and a menu

    global fonts

    combobox::combobox $w
    foreach i $list {
        $w list insert end $i
    }
    catch {$w configure -value [lindex $list 0]}
    $w configure -command "$cmd ;#"
    return $w
}

proc comboconfig {w list {cmd {}}} {
    # reconfigures a combo box widget

    $w list delete 0 end
    foreach i $list {
        $w list insert end $i
    }
    if {[lsearch $list [$w cget -value]] == -1} {
        catch {$w configure -value [lindex $list 0]}
    }
    $w configure -command "$cmd ;#"
    return $w
}

proc combo-onchange {w cmd} {
    $w configure -command "$cmd ;#" -commandstate normal
}

proc label-entry {w label {text {}}} {
    # utility function: create a frame with a label+entry
    frame $w
    label $w.l -anchor w -width 16 -text $label
    entry $w.e -highlightthickness 0
    pack $w.l -anchor center -expand 0 -fill x -padx 2 -pady 2 -side left
    pack $w.e -anchor center -expand 1 -fill x -padx 2 -pady 2 -side right
    $w.e insert 0 $text
}

proc label-entry-help {w label helptext {text {}}} {
    # utility function: create a frame with a label+entry
    frame $w
    label $w.l -anchor w -text $label
    entry $w.e -highlightthickness 0
    label $w.h -anchor w -text "(Help)" -fg "#0000a0"
    grid $w.l $w.e $w.h -sticky news
    grid columnconfigure $w 1 -weight 1
    bind $w.h <Button-1> [list helplabel_showhelp $w $helptext %X %Y]
    $w.e insert 0 $text
}

proc label-entry-chooser {w label text chooserproc} {
    # utility function: create a frame with a label+entry+button
    # the button is expected to call a dialog where the user can select
    # a value for the entry
    frame $w
    label $w.l -anchor w -width 16 -text $label
    entry $w.e -highlightthickness 0
    button $w.c -text " ... " -command [list chooser:choose $w.e $chooserproc]
    pack $w.l -anchor center -expand 0 -fill none -padx 2 -pady 2 -side left
    pack [wsize $w.c 20 20] -anchor center -expand 0 -fill none -padx 2 -pady 2 -side right
    pack $w.e -anchor center -expand 1 -fill x -padx 2 -pady 2 -side left
    $w.e insert 0 $text
}

# private proc for label-entry-chooser
proc chooser:choose {w chooserproc} {
    set current [$w get]
    set new [eval $chooserproc \"$current\"]
    if {$new!=""} {
       $w delete 0 end
       $w insert end $new
    }
}

proc label-sunkenlabel {w label {text {}}} {
    # utility function: create a frame with a label+"readonly entry"
    frame $w
    label $w.l -anchor w -width 16 -text $label
    label $w.e -relief groove -justify left -anchor w
    pack $w.l -anchor center -expand 0 -fill none -padx 2 -pady 2 -side left
    pack $w.e -anchor center -expand 1 -fill x -padx 2 -pady 2 -side right
    $w.e config -text $text
}

proc label-message {w label {text {}}} {
    # utility function: create a frame with a label+"readonly entry"
    frame $w
    label $w.l -anchor w -width 16 -text $label
    message $w.e -relief groove -justify left -anchor w -aspect 10000
    pack $w.l -anchor center -expand 0 -fill none -padx 2 -pady 2 -side left
    pack $w.e -anchor center -expand 1 -fill x -padx 2 -pady 2 -side right
    $w.e config -text $text
}

proc label-combo {w label list {text {}} {cmd {}}} {
    # utility function: create a frame with a label+combo
    frame $w
    label $w.l -anchor w -width 16 -text $label
    combo $w.e $list $cmd
    pack $w.l -anchor center -expand 0 -fill none -padx 2 -pady 2 -side left
    pack $w.e -anchor center -expand 1 -fill x -padx 2 -pady 2 -side right
    if {$text != ""} {
         $w.e configure -value $text
    } else {
         $w.e configure -value [lindex $list 0]
    }
}

proc label-combo2 {w label list {text {}} {cmd {}}} {
    # start with empty combo box
    label-combo $w $label $list $text $cmd
    $w.e delete 0 end
}

proc label-text {w label height {text {}}} {
    # utility function: create a frame with a label+text
    frame $w
    label $w.l -anchor w -width 16 -text $label
    text $w.t -highlightthickness 0 -height $height -width 40
    catch {$w.t config -undo true}
    pack $w.l -anchor n -expand 0 -fill none -padx 2 -pady 2 -side left
    pack $w.t -anchor center -expand 1 -fill both -padx 2 -pady 2 -side right
    $w.t insert 1.0 $text
}

proc label-scale {w label} {
    # utility function: create a frame with a label+scale
    frame $w
    label $w.l -anchor w -width 16 -text $label
    scale $w.e -orient horizontal -width 10 -sliderlength 20
    pack $w.l -anchor center -expand 0 -fill none -padx 2 -pady 0 -side left
    pack $w.e -anchor center -expand 1 -fill x -padx 2 -pady 0 -side right
}

proc label-button {w label {text {}}} {
    # utility function: create a frame with label+button
    frame $w
    label $w.l -anchor w -width 16 -text $label
    button $w.e
    pack $w.l -anchor center -expand 0 -fill none -padx 2 -pady 2 -side left
    pack $w.e -anchor center -expand 1 -fill x -padx 2 -pady 2 -side right
    $w.e config -text $text
}

proc label-check {w label first var} {
    # utility function: create a frame with a label+radiobutton for choose
    frame $w
    label $w.l -anchor w -width 16 -text $label
    frame $w.f
    checkbutton $w.f.r1 -text $first -variable $var

    pack $w.l -anchor w -expand 0 -fill none -side left
    pack $w.f -anchor w -expand 0 -side left -fill x
    pack $w.f.r1 -anchor w -expand 0 -side left
}

# label-colorchooser --
#
# utility function: create a frame with a label and a color chooser button
#
proc label-colorchooser {w label {color ""}} {
    global gned

    frame $w
    label $w.l -anchor w -width 16 -text $label
    frame $w.f
    pack $w.l -anchor nw -expand 0 -fill none -side left -padx 2 -pady 2
    pack $w.f -anchor n -expand 1 -fill x -side left

    entry $w.f.e
    pack $w.f.e -anchor center -expand 1 -fill x -side left -padx 2 -pady 2

    button $w.f.b -relief groove -command [list colorchooser:setColor $w.f.b $w.f.e [winfo toplevel $w]] -width 6
    pack $w.f.b -anchor c -expand 0 -fill none -side left -padx 2 -pady 2

    $w.f.e insert 0 $color
    set dispcolor [resolveDispStrColor $color ""]
    if {$dispcolor!=""} {
       $w.f.b config -background $dispcolor -activebackground $dispcolor
    }
}

proc colorchooser:setColor {b e pwin} {
    global gned

    set color [tk_chooseColor -parent $pwin]
    if {$color!=""} {
        $b config -background $color -activebackground $color
        $e delete 0 end
        $e insert 0 $color
        $e selection range 0 end
    }
}

proc commentlabel {w text} {
    # utility function: create a frame with a message widget
    frame $w
    message $w.e -justify left -text $text -aspect 1000
    pack $w.e -anchor center -expand 0 -fill x -padx 2 -pady 2 -side left
}

proc labelwithhelp {w text helptext} {
    frame $w
    label $w.l -justify left -text $text
    label $w.h -justify left -text "(Help)" -fg "#0000a0"
    pack $w.l $w.h -expand 0 -side left -anchor center -fill none -padx 2 -pady 2
    bind $w.h <Button-1> [list helplabel_showhelp $w $helptext %X %Y]
}

proc helplabel_showhelp {parentw text x y} {
    global help_tips

    # note: must be child of the current dialog box, otherwise won't receive
    # mouse events under Linux, due to the grab the dialog box holds...
    set w $parentw.helpwin
    catch {destroy $w}
    toplevel $w -relief flat
    wm overrideredirect $w true
    wm positionfrom $w program
    wm geometry $w "+[expr $x-200]+[expr $y+12]"
    label $w.tip -text $text -padx 4 -wraplength $help_tips(width) \
                            -bg $help_tips(color) -border 1 -relief solid \
                            -font $help_tips(font) -justify left
    pack $w.tip
    focus $w.tip

    movetoscreen $w

    bind $w <KeyPress> "catch { destroy $w }"
    bind $w <FocusOut> "catch { destroy $w }"
    bind $w <Button-1> "catch { destroy $w }"
}


# noteboook --
#
# Create 'tabbed notebook' widget
#
# Usage example:
#  notebook .x bottom
#  notebook_addpage .x p1 Egy
#  notebook_addpage .x p2 Ketto
#  notebook_addpage .x p3 Harom
#  pack .x -expand 1 -fill both
#  label .x.p1.e -text "One"
#  pack .x.p1.e
#
proc notebook {w {side top}} {
    global HAVE_BLT

    if {$HAVE_BLT} {
        blt::tabset $w -tearoff no -relief flat -side top -samewidth no -highlightthickness 0
    } else {
        # poor man's tabnotebook
        global nb
        set nb($w) ""

        frame $w
        frame $w.tabs
        pack $w.tabs -side $side -fill x
    }
}

#
#  utility function: add page to notebook widget
#
proc notebook_addpage {w name label} {
    global HAVE_BLT

    if {$HAVE_BLT} {
        set page $w.$name
        frame $page
        $w insert end $name -text $label -window $page -fill both
        $w select [$w index -name $name]
    } else {
        # poor man's tabnotebook
        set tab $w.tabs.$name
        set page $w.$name

        frame $page -border 2 -relief raised
        button $tab -text $label -command "notebook_showpage $w $name" -relief flat
        pack $tab -anchor n -expand 0 -fill none -side left

        global nb
        if {$nb($w)==""} {notebook_showpage $w $name}
    }
}

#
# show given notebook page
#
proc notebook_showpage {w name} {
    global HAVE_BLT

    if {$HAVE_BLT} {
        $w select [$w index -name $name]
    } else {
        # poor man's tabnotebook
        global nb

        if {$nb($w)==$name} return

        pack $w.$name -expand 1 -fill both
        $w.tabs.$name config -relief raised

        if {$nb($w)!=""} {
           pack forget $w.$nb($w)
           $w.tabs.$nb($w) config -relief flat
        }
        set nb($w) $name
    }
}


# vertResizeBar --
#
# Vertical 'resize bar' (divider)
#
proc vertResizeBar {w wToBeResized} {
    # create widget
    frame $w -width 5 -relief raised -borderwidth 1
    if [catch {$w config -cursor size_we}] {
      if [catch {$w config -cursor sb_h_double_arrow}] {
        catch {$w config -cursor sizing}
      }
    }

    # create bindings
    bind $w <Button-1> "vertResizeBar:buttonDown $w %X"
    bind $w <B1-Motion> "vertResizeBar:buttonMove %X"
    bind $w <ButtonRelease-1> "vertResizeBar:buttonRelease %X $wToBeResized"
    bind $w <Button-2> "catch {destroy .resizeBar}"
    bind $w <Button-3> "catch {destroy .resizeBar}"
}

proc vertResizeBar:buttonDown {w x} {
    global mouse
    set mouse(origx) $x

    catch {destroy .resizeBar}
    toplevel .resizeBar -relief flat -bg #606060
    wm overrideredirect .resizeBar true
    wm positionfrom .resizeBar program
    set geom "[winfo width $w]x[winfo height $w]+[winfo rootx $w]+[winfo rooty $w]"
    wm geometry .resizeBar $geom
}

proc vertResizeBar:buttonMove {x} {
    catch {wm geometry .resizeBar "+$x+[winfo rooty .resizeBar]"}
}

proc vertResizeBar:buttonRelease {x wToBeResized} {
    global mouse
    set dx [expr $x-$mouse(origx)]

    set width [$wToBeResized cget -width]
    set width [expr $width+$dx]
    $wToBeResized config -width $width

    catch {destroy .resizeBar}
}

# tableEdit --
#
# Create a "tableEdit" widget
#
# one $columnlist entry:
#   {title column-name command-to-create-widget-in-cell}
#
#  the command should use two variables:
#    $e - widget must be created with name stored in $e
#    $v - widget must be bound to variable whose name is in $v
#
# Example:
# tableEdit .t 20 {
#   {Name    name    {entry $e -textvariable $v -width 8 -bd 1 -relief sunken}}
#   {Value   value   {entry $e -textvariable $v -width 12 -bd 1 -relief sunken}}
#   {Comment comment {entry $e -textvariable $v -width 20 -bd 1 -relief sunken}}
# }
# pack .t -expand 1 -fill both
#
proc tableEdit {w numlines columnlist} {

    # clean up variables from earlier table instances with same name $w
    global tablePriv
    foreach i [array names tablePriv "$w,*"] {
        unset tablePriv($i)
    }

    # create widgets
    frame $w; # -bg green
    frame $w.tb -height 16
    canvas $w.c -yscrollcommand "$w.vsb set" -height 150 -bd 0
    scrollbar $w.vsb -command "$w.c yview"

    grid rowconfig $w 1 -weight 1 -minsize 0

    grid $w.tb -in $w -row 0 -column 0 -rowspan 1 -columnspan 1 -sticky news
    grid $w.c   -in $w -row 1 -column 0 -rowspan 1 -columnspan 1 -sticky news
    grid $w.vsb -in $w -row 1 -column 1 -rowspan 1 -columnspan 1 -sticky news

    frame $w.c.f -bd 0
    $w.c create window 0 0 -anchor nw -window $w.c.f

    set tb $w.tb
    set f $w.c.f

    for {set li 0} {$li<$numlines} {incr li} {
       set col 0
       foreach entry $columnlist {
           # get fields from entry
           set title   [lindex $entry 0]
           set attr    [lindex $entry 1]
           set wcmd    [lindex $entry 2]

           # add table entry
           set e $f.li$li-$attr
           set v tablePriv($w,$li,$attr)
           eval $wcmd
           grid $e -in $f -row $li -column $col -rowspan 1 -columnspan 1 -sticky news

           # make sure edited entry is always visible
           bind $e <Key> [list _focusTableEntry $f.li$li-$attr $w.c]

           # key bindings: up, down
           if {$li!=0} {
               bind $e <Up> [list _focusTableEntry $f.li[expr $li-1]-$attr $w.c]
           }
           if {$li!=$numlines-1} {
               bind $e <Down> [list _focusTableEntry $f.li[expr $li+1]-$attr $w.c]
           }

           # next column
           incr col
       }
    }

    update idletasks

    # create title labels
    set dx 2
    foreach entry $columnlist {
        # get fields from entry
        set title   [lindex $entry 0]
        set attr    [lindex $entry 1]

        set e $f.li0-$attr
        label $tb.$attr -bd 1 -relief raised -text $title

        # add title bar
        set width [expr [winfo width $e]]
        place $tb.$attr -in $tb -x $dx -y 0 -width $width -height [winfo height $tb]
        set dx [expr $dx + $width]
    }

    # adjust canvas width to frame width
    $w.c config -width [winfo width $f]
    $w.c config -scrollregion "0 0 0 [winfo height $f]"

    #focus $w.l0c0

}

#
# internal to tableEdit: ensures current widget is always visible
#
proc _focusTableEntry {e c} {
    focus $e
    # are we below the visible canvas area?
    set d [expr [winfo rooty $e]-[winfo rooty $c]-[winfo height $c]]
    if {$d>-10} {
        $c yview scroll [expr $d+10] units
    }
    # are we above the visible canvas area?
    set d [expr [winfo rooty $c]-[winfo rooty $e]-[winfo height $e]]
    if {$d>0} {
        $c yview scroll [expr -$d+10] units
    }
}

#
# Create multicolumn listbox. The $columnlist arg should contain a list of
# column descriptions, each one either as {name label width} or
# {name label} pair (in the latter case, column width will be auto).
#
# Example:
#  multicolumnlistbox .lb {
#     {name    Name          20}
#     {date    Date          15}
#     {descr   Description}
#  }
#
#
proc multicolumnlistbox {w columnlist args} {
    global HAVE_BLT
    if {$HAVE_BLT} {
        blt::treeview $w -allowduplicates yes -flat yes
        $w column configure treeView -hide no -width 15 -state disabled
        if {$args!=""} {
             eval $w config $args
        }

        foreach i $columnlist {
            set name [lindex $i 0]
            set label [lindex $i 1]
            set width [lindex $i 2]
            $w column insert end $name -text $label -justify left -edit no -pad 8 \
                -command [list multicolumnlistbox_blt_sortcolumn $w $name]
            if {$width!=""} {
                $w column config $name -width $width
            }
        }
        # eliminate "last column quirk" by adding a very wide dummy column:
        $w column insert end "dummy" -text "" -edit no -width 1000
        bind $w <3> {%W selection clearall; %W select set [%W nearest %x %y]}
        #bind $w <Motion> {puts "[%W nearest %x %y] of [%W index view.top]..[%W index view.bottom] -- [%W find view.top view.bottom]"}
    } else {
        # emulate it with listbox widget
        global mclistbox
        listbox $w
        if {$args!=""} {
             eval $w config $args
        }
        # unlike blt::tree, listbox counts in chars
        if {[$w cget -width]>100} {
             $w config -width [expr [$w cget -width]/8]
        }
        if {[$w cget -height]>100} {
             $w config -height [expr [$w cget -height]/12]
        }
        # store column names -- we'll need them later
        set cols {}
        foreach i $columnlist {
            lappend cols [lindex $i 0]
        }
        set mclistbox($w,columns) $cols
    }
}

#
# private procedure for multicolumnlistbox
#
proc multicolumnlistbox_blt_sortcolumn {w column} {
    set old [$w sort cget -column]
    set decreasing 0
    if {$old==$column} {
        set decreasing [$w sort cget -decreasing]
        set decreasing [expr !$decreasing]
    }
    $w sort configure -decreasing $decreasing -column $column
    $w configure -flat yes
    $w sort auto yes
    blt::busy hold $w
    update
    blt::busy release $w
}

#
# Inserts a line into a multicolumn-listbox. The $rowname can be used later
# to identify the row. $data contains values for different columns in the
# format {name1 value1 name2 value2 ...}, conventiently produced from
# arrays by the command "array get".
#
proc multicolumnlistbox_insert {w rowname data {icon ""}} {
    global icons
    global HAVE_BLT
    if {$HAVE_BLT} {
        if {$icon==""} {set icon $icons(16pixtransp)}
        $w insert end $rowname -data $data -button no -icons [list $icon $icon] -activeicons [list $icon $icon]
    } else {
        global mclistbox
        array set ary $data
        set row ""
        foreach col $mclistbox($w,columns) {
            # catch because it might be missing from the array
            catch {append row "$ary($col)   "}
        }
        append row [string repeat " " 160]
        append row $rowname
        $w insert end $row
    }
}

#
# Updates a given row.
#
proc multicolumnlistbox_modify {w rowname data} {
    global HAVE_BLT
    if {$HAVE_BLT} {
        set id [$w find -full $rowname]
        if {$id==""} {error "row $rowname not found"}
        $w entry config $id -data $data
    } else {
        error "multicolumnlistbox_modify not supported without BLT!"
    }
}

#
# Returns data from the given row.
#
proc multicolumnlistbox_getrow {w rowname} {
    global HAVE_BLT
    if {$HAVE_BLT} {
        set id [$w find -full $rowname]
        if {$id==""} {error "row $rowname not found"}
        return [$w entry cget $id -data]
    } else {
        error "multicolumnlistbox_getrow not supported without BLT!"
    }
}

#
# Returns true if the given row exists.
#
proc multicolumnlistbox_hasrow {w rowname} {
    global HAVE_BLT
    if {$HAVE_BLT} {
        set id [$w find -full $rowname]
        if {$id!=""} {return 1} else {return 0}
    } else {
        error "multicolumnlistbox_hasrow not supported without BLT!"
    }
}

#
# Returns a list containing the rownames of all of the entries
# that are currently selected. If there are no entries selected,
# then the empty string is returned.
#
proc multicolumnlistbox_curselection {w} {
    global HAVE_BLT
    if {$HAVE_BLT} {
        set rownamelist {}
        foreach id [$w curselection] {
            lappend rownamelist [$w get -full $id]
        }
        return $rownamelist
    } else {
        set sel [$w curselection]
        if {$sel == ""} {return ""}
        set rownames {}
        foreach i $sel {
            set row [$w get $i]
            # catch because there might be a parse error when interpreting it as a list
            catch {lappend rownames [lindex $row end]}
        }
        return $rownames
    }
}

#
# Returns a list containing the rownames of all of the entries
# that are currently selected. If there are no entries selected,
# then the empty string is returned.
#
proc multicolumnlistbox_getrownames {w} {
    global HAVE_BLT
    if {$HAVE_BLT} {
        set rownamelist {}
        catch {
            # if the listbox is empty, $w find... will throw an error !?!@$!!
            foreach id [$w find view.top view.bottom] {
                lappend rownamelist [$w get -full $id]
            }
        }
        return $rownamelist
    } else {
        error "multicolumnlistbox_getrownames not supported without BLT!"
    }
}

#
# Delete the given rows.
#
proc multicolumnlistbox_delete {w rownames} {
    global HAVE_BLT
    if {$HAVE_BLT} {
        foreach rowname $rownames {
            set id [$w find -full $rowname]
            if {$id==""} {error "row $rowname not found"}
            $w delete $id
        }
    } else {
        error "multicolumnlistbox_delete not supported without BLT!"
    }
}

#
# Delete all rows.
#
proc multicolumnlistbox_deleteall {w} {
    global HAVE_BLT
    if {$HAVE_BLT} {
        $w delete all
    } else {
        $w delete 0 end
    }
}

#
# Inserts a dummy line.
#
proc multicolumnlistbox_adddummyline {w} {
    global HAVE_BLT icons
    if {$HAVE_BLT} {
        set icon $icons(16pixtransp)
        $w insert end [opp_null] -icons [list $icon $icon] -activeicons [list $icon $icon]
    }
}

# movetoscreen --
#
# utility function: moves a window so that it becomes fully visible
#
proc movetoscreen {w} {
    global tcl_platform

    # update all the geometry information. NOTE: with some X servers, "update idletasks"
    # has no effect if window is withdrawn...
    update idletasks

    set screenwidth [winfo screenwidth $w]
    set screenheight [winfo screenheight $w]
    set width [winfo width $w]
    set height [winfo height $w]
    set x [winfo rootx $w]
    set y [winfo rooty $w]

    #puts "($x,$y) size($width,$height) screen($screenwidth,$screenheight)"

    if {$x+$width > $screenwidth} {set x [expr $screenwidth - $width]}
    if {$y+$height > $screenheight} {set y [expr $screenheight - $height]}
    if {$x < 0} {set x 0}
    if {$y < 0} {set y 0}

    if {$tcl_platform(platform) != "windows"} {
        # X servers need a sidekick ("withdraw"+"deiconify") to re-position the window
        wm withdraw $w
        wm geom $w +$x+$y
        wm deiconify $w
    } else {
        # Windows is fine
        wm geom $w +$x+$y
    }
}

# center --
#
# utility function: centers a dialog on the screen
#
proc center {w} {

    global tcl_platform

    # preliminary placement...
    if {[winfo reqwidth $w]!=0} {
       set pre_x [expr ([winfo screenwidth $w]-[winfo reqwidth $w])/2-[winfo vrootx [winfo parent $w]]]
       set pre_y [expr ([winfo screenheight $w]-[winfo reqheight $w])/2-[winfo vrooty [winfo parent $w]]]
       wm geom $w +$pre_x+$pre_y
    }

    # withdraw the window, then update all the geometry information
    # so we know how big it wants to be, then center the window in the
    # display and de-iconify it.
    if {$tcl_platform(platform) != "windows"} {
        wm withdraw $w
    }
    update idletasks
    set x [expr [winfo screenwidth $w]/2 - [winfo width $w]/2  - [winfo vrootx [winfo parent $w]]]
    set y [expr [winfo screenheight $w]/2 - [winfo height $w]/2  - [winfo vrooty [winfo parent $w]]]
    wm geom $w +$x+$y
    if {$tcl_platform(platform) != "windows"} {
        wm deiconify $w
    }
    focus $w
}


# createOkCancelDialog --
#
# creates dialog with OK and Cancel buttons
# user's widgets can go into frame $w.f
#
proc createOkCancelDialog {w title} {
    global tk_version tcl_platform

    catch {destroy $w}
    toplevel $w -class Toplevel
    if {$tk_version<8.2 || $tcl_platform(platform)!="windows"} {
        wm transient $w [winfo toplevel [winfo parent $w]]
    }
    wm title $w $title
    wm iconname $w Dialog
    wm focusmodel $w passive
    wm overrideredirect $w 0
    wm resizable $w 1 1
    wm deiconify $w
    wm protocol $w WM_DELETE_WINDOW { }

    # preliminary placement (assumes 350x250 dialog)...
    set pre_x [expr ([winfo screenwidth $w]-350)/2-[winfo vrootx [winfo parent $w]]]
    set pre_y [expr ([winfo screenheight $w]-250)/2-[winfo vrooty [winfo parent $w]]]
    wm geom $w +$pre_x+$pre_y

    frame $w.f
    frame $w.buttons
    button $w.buttons.okbutton  -text {OK} -width 10
    button $w.buttons.cancelbutton  -text {Cancel} -width 10

    pack $w.buttons -expand 0 -fill x -padx 5 -pady 5 -side bottom
    pack $w.f -expand 1 -fill both -padx 5 -pady 5 -side top
    pack $w.buttons.cancelbutton  -anchor n -side right -padx 2
    pack $w.buttons.okbutton  -anchor n -side right -padx 2
}


# execOkCancelDialog --
#
# Executes the dialog.
# Optional validating proc may check if fields are correctly
# filled in -- it should return 1 if dialog contents is valid,
# 0 if there are invalid fields and OK button should not be
# accepted.
#
# Returns 1 if Ok was pressed, 0 if Cancel was pressed.
#
proc execOkCancelDialog {w {validating_proc {}}} {

    global opp

    $w.buttons.okbutton configure -command "set opp($w) 1"
    catch {$w.buttons.cancelbutton configure -command "set opp($w) 0"}

    bind $w <Return> "if {\[winfo class \[focus\]\]!=\"Text\"} {set opp($w) 1}"
    bind $w <Escape> "set opp($w) 0"

    wm protocol $w WM_DELETE_WINDOW "set opp($w) 0"

    # next line mysteriously solves "lost focus" problem of popup dialogs...
    after 1 "wm deiconify $w"

    center $w

    set oldGrab [grab current $w]
    if {$oldGrab != ""} {
        set grabStatus [grab status $oldGrab]
    }
    grab $w

    # Wait for the user to respond, then restore the focus and
    # return the index of the selected button.  Restore the focus
    # before deleting the window, since otherwise the window manager
    # may take the focus away so we can't redirect it.  Finally,
    # restore any grab that was in effect.

    if {$validating_proc==""} {
        tkwait variable opp($w)
    } else {
        tkwait variable opp($w)
        while {$opp($w)==1 && ![eval $validating_proc $w]} {
            tkwait variable opp($w)
        }
    }

    if {$oldGrab != ""} {
        if {$grabStatus == "global"} {
            grab -global $oldGrab
        } else {
            grab $oldGrab
        }
    }
    return $opp($w)
}

# createCloseDialog --
#
# Creates dialog with a Close button.
# User's widgets can go into frame $w.f, and extra buttons can go into frame $w.buttons.
#
proc createCloseDialog {w title} {
    global tk_version tcl_platform

    catch {destroy $w}
    toplevel $w -class Toplevel
    if {$tk_version<8.2 || $tcl_platform(platform)!="windows"} {
        wm transient $w [winfo toplevel [winfo parent $w]]
    }
    wm title $w $title
    wm iconname $w Dialog
    wm focusmodel $w passive
    wm overrideredirect $w 0
    wm resizable $w 1 1
    wm deiconify $w
    wm protocol $w WM_DELETE_WINDOW { }

    # preliminary placement (assumes 350x250 dialog)...
    set pre_x [expr ([winfo screenwidth $w]-350)/2-[winfo vrootx [winfo parent $w]]]
    set pre_y [expr ([winfo screenheight $w]-250)/2-[winfo vrooty [winfo parent $w]]]
    wm geom $w +$pre_x+$pre_y

    frame $w.f
    frame $w.buttons
    button $w.buttons.closebutton  -text {Close} -width 10

    pack $w.buttons -expand 0 -fill x -padx 5 -pady 5 -side bottom
    pack $w.f -expand 1 -fill both -padx 5 -pady 5 -side top
    pack $w.buttons.closebutton  -anchor n -side right -padx 2
}


# execCloseDialog --
#
# Executes the dialog.
#
proc execCloseDialog w {

    global opp

    $w.buttons.closebutton configure -command "set opp($w) 1"

    #bind $w <Return> "if {\[winfo class \[focus\]\]!=\"Text\"} {set opp($w) 1}"
    bind $w <Escape> "set opp($w) 0"

    wm protocol $w WM_DELETE_WINDOW "set opp($w) 0"

    # next line mysteriously solves "lost focus" problem of popup dialogs...
    after 1 "wm deiconify $w"

    center $w

    set oldGrab [grab current $w]
    if {$oldGrab != ""} {
        set grabStatus [grab status $oldGrab]
    }
    grab $w

    # Wait for the user to respond, then restore the focus and
    # return the index of the selected button.  Restore the focus
    # before deleting the window, since otherwise the window manager
    # may take the focus away so we can't redirect it.  Finally,
    # restore any grab that was in effect.

    tkwait variable opp($w)

    if {$oldGrab != ""} {
        if {$grabStatus == "global"} {
            grab -global $oldGrab
        } else {
            grab $oldGrab
        }
    }
    return $opp($w)
}


# aboutDialog --
#
# Displays an About dialog.
#
proc aboutDialog {title part1 part2} {
    global fonts
    catch {destroy .about}
    set w .about
    createOkCancelDialog $w $title
    $w.f config -padx 0 -pady 0 -bg #e0e0a0 -border 2 -relief groove
    # -font $fonts(big)
    label $w.f.l1 -text "$part1" -bg #e0e0a0 -padx 30 -font $fonts(bold)
    label $w.f.l2 -text "$part2\n\n" -bg #e0e0a0 -padx 30
    pack $w.f.l1 -expand 0 -fill x -side top
    pack $w.f.l2 -expand 1 -fill both -side top
    destroy $w.buttons.cancelbutton
    execOkCancelDialog $w
    destroy $w
}


#
# Show a hint once; state is saved into config(dontshow)
#
set config(dontshow) {}
proc showTextOnceDialog {key} {
    global config tmp_once hints

    if {[lsearch -exact $config(dontshow) $key]!=-1} {
        # already shown
        return
    }

    if {![info exists hints($key)]} {
        debug "dbg: showTextOnceDialog: hint hints($key) not found"
        return
    }

    set text $hints($key)

    # create dialog with OK button
    set w .once
    createOkCancelDialog $w "Hint"
    destroy $w.buttons.cancelbutton
    wm geometry $w "360x180"

    text $w.f.text -relief solid -bd 1 -wrap word
    $w.f.text insert 1.0 $text
    $w.f.text config -state disabled
    checkbutton $w.f.x -text {do not show this hint again} -variable tmp_once
    pack $w.f.x -expand 0 -fill x -side bottom
    pack $w.f.text -expand 1 -fill both -side top -padx 5 -pady 5

    set tmp_once 0

    # exec the dialog
    execOkCancelDialog $w
    if {$tmp_once} {
        lappend config(dontshow) $key
    }
    destroy $w
}

#
# Creates mouse wheel bindings for the given widget or widget class.
# Note: wheel events are only delivered to the widget IF IT HAS FOCUS!
#
# Code copied almost verbatim from lib/tk8.4/text.tcl.
#
proc bindMouseWheel {w} {

    # The MouseWheel will typically only fire on Windows.  However,
    # someone could use the "event generate" command to produce one
    # on other platforms.

    if {[string equal [tk windowingsystem] "classic"]
        || [string equal [tk windowingsystem] "aqua"]} {
        bind $w <MouseWheel> {
            if [canScrollY %W] {
                %W yview scroll [expr {- (%D)}] units
            }
        }
        bind $w <Option-MouseWheel> {
            if [canScrollY %W] {
                %W yview scroll [expr {-10 * (%D)}] units
            }
        }
    } else {
        bind $w <MouseWheel> {
            if [canScrollY %W] {
                %W yview scroll [expr {- (%D / 120) * 4}] units
            }
        }
    }

    if {[string equal "x11" [tk windowingsystem]]} {
        # Support for mousewheels on Linux/Unix commonly comes through mapping
        # the wheel to the extended buttons.  If you have a mousewheel, find
        # Linux configuration info at:
        #       http://www.inria.fr/koala/colas/mouse-wheel-scroll/
        bind $w <4> {
            if {!$tk_strictMotif && [canScrollY %W]} {
                %W yview scroll -5 units
            }
        }
        bind $w <5> {
            if {!$tk_strictMotif && [canScrollY %W]} {
                %W yview scroll 5 units
            }
        }
    }
}

proc canScrollY {w} {
    if {[winfo class $w]!="Canvas"} {
        return 1
    } else {
        set bbox [$w bbox all]
        set contentHeight [expr [lindex $bbox 3] - [lindex $bbox 1]]
        set widgetHeight [winfo height $w]
        return [expr $contentHeight > $widgetHeight]
    }
}
