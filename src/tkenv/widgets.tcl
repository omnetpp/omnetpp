#=================================================================
#  WIDGETS.TCL - part of
#
#                     OMNeT++/OMNEST
#            Discrete System Simulation in C++
#
#=================================================================

#----------------------------------------------------------------#
#  Copyright (C) 1992-2008 Andras Varga
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

# turn on anti-aliasing on OS/X
catch {set tk::mac::CGAntialiasLimit 1}

# setupTkOptions --
#
# Sets up fonts and various options in Tk in order to have consistent look
# on Unix/Windows and across different Tk versions.
#
proc setupTkOptions {} {
   global fonts defaultfonts icons tcl_platform tk_version
   global tcl_wordchars tcl_nonwordchars
   global HAVE_BLT B2 B3

   # test for BLT
   set HAVE_BLT 0
   catch {package require BLT; set HAVE_BLT 1}
   if {!$HAVE_BLT && ![string equal [tk windowingsystem] aqua]} {
      # no BLT on OS X Tk Aqua, so skip the warning
      puts "\n*** BLT Tcl/Tk extension not found -- please make sure it is installed, and TCL_LIBRARY is set properly."
   }
   if {$HAVE_BLT} {
       if [catch {
           blt::tabset .test_blt_tabset
           blt::treeview .test_blt_treeview
       } errmsg] {
           set HAVE_BLT 0
           puts "\n*** BLT installation seems to be broken, reverting to non-BLT widgets! Details: \"package require BLT\" command was successful, but could not create BLT widgets: $errmsg\n"
       }
       catch {destroy .test_blt_tabset}
       catch {destroy .test_blt_treeview}
   }

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

   # on Mac OS/X, the Aqua version of Tcl/Tk (at least on older ones?) reports
   # right mouse button as button 2, and middle one as button 3. This is
   # quote the opposite of X11 and Windows.
   # see http://support.svi.nl/wiki/MouseButtonsInMacAqua
   set B2 2
   set B3 3
   if {[string equal [tk windowingsystem] aqua]}  {
       set B2 3
       set B3 2
   }

   # also, work around bug in Tk/Aqua image handling
   if {[string equal [tk windowingsystem] aqua]}  {
       iconsWorkaroundForOSX
   }

   # set up wheel support for a few extra widget types
   bindMouseWheel Canvas
   bindMouseWheel TreeView

   # set up fonts.
   # Note: font settings will take effect in reflectSettingsInGui,
   # which is called after .tkenvrc has been read
   if {[string equal [tk windowingsystem] x11]} {
      set normalfamily [getFirstAvailableFontFamily {Ubuntu Arial Verdana Helvetica Tahoma "DejaVu Sans" "Nimbus Sans L" FreeSans Sans} unknown]
      set monofamily [getFirstAvailableFontFamily {"DejaVu Sans Mono" "Courier New" "FreeMono" "Courier"} unknown]
      # note: standard font names (not families!) are: TkCaptionFont TkSmallCaptionFont TkTooltipFont TkFixedFont TkHeadingFont TkMenuFont TkIconFont TkTextFont TkDefaultFont
      if {[tk scaling] > 1.5} {set size 11} else {set size 9}
      set fonts(normal)   [list $normalfamily $size]
      set fonts(bold)     [list $normalfamily $size bold]
      set fonts(big)      [list $normalfamily 18]
      set fonts(text)     [list $normalfamily $size]
      set fonts(balloon)  [list $normalfamily $size]
      set fonts(canvas)   [list $normalfamily $size]
      set fonts(timeline) [list $normalfamily $size]
      set fonts(mono)     [list $monofamily $size]
   } elseif {[string equal [tk windowingsystem] aqua]} {
      # Mac
      set normalfamily [getFirstAvailableFontFamily {"Lucida Grande" Helvetica} unknown]
      set monofamily [getFirstAvailableFontFamily {"Monaco" "Courier"} unknown]
      set size 13
      set fonts(normal)   [list $normalfamily 12]
      set fonts(bold)     [list $normalfamily 12 bold]
      set fonts(big)      [list $normalfamily 18]
      set fonts(text)     [list $monofamily 12]
      set fonts(balloon)  [list $normalfamily 12]
      set fonts(timeline) [list $normalfamily 11]
      set fonts(canvas)   [list $normalfamily 12]
      set fonts(mono)     [list $monofamily $size]
   } else {
      # Windows
      set normalfamily [getFirstAvailableFontFamily {"Segoe UI" "MS Sans Serif" "Arial"} unknown]
      set monofamily [getFirstAvailableFontFamily {"DejaVu Sans Mono" "Courier New" "Consolas" "Terminal"} unknown]
      if {$normalfamily == "Segoe UI"} {
          set size 9  ;# text in this font appears to be smaller than in MS Sans Serif or Arial
      } else {
          set size 8
      }
      set fonts(normal)   [list $normalfamily $size]
      set fonts(bold)     [list $normalfamily $size]
      set fonts(big)      [list $normalfamily 18]
      set fonts(text)     [list $normalfamily $size]
      set fonts(balloon)  [list $normalfamily $size]
      set fonts(timeline) [list $normalfamily $size]
      set fonts(canvas)   [list $normalfamily $size]
      set fonts(mono)     [list $monofamily $size]
   }

   # remember default font settings (we'll only save the non-default ones to .tkenvrc)
   foreach i [array names fonts] {
       set defaultfonts($i) $fonts($i)
   }

   if {[string equal [tk windowingsystem] x11]} {
       # make menus look more contemporary
       menu .tmp
       set activebg [.tmp cget -activebackground]
       set activefg [.tmp cget -activeforeground]
       destroy .tmp
       option add *Scrollbar.width  12
       option add *Menu.activeBorderWidth 0
       option add *Menu.relief raised
       option add *Menu.activeBackground #800000
       option add *Menu.activeForeground white
       option add *menubar.borderWidth 1
       option add *menubar.activeBorderWidth 1
       option add *menubar.activeBackground $activebg
       option add *menubar.activeForeground $activefg
   }

   # patch icons on OS X
   foreach key [array names icons] {
       fixupImageIfNeeded $icons($key)
   }

   # On OS X, the application comes up in the background. The workaround is
   # to set the "always on top" option (which can be removed later)
   if {[string equal [tk windowingsystem] aqua]} {
       after idle {
           wm attributes . -topmost 1
           after 1000 {wm attributes . -topmost 0}  ;# note: doesn't work with "after idle"
       }
   }

}

#
# Returns the first font family from the given preference list that is
# available on the system. If none are available, returns $defaultvalue.
#
proc getFirstAvailableFontFamily {preferencelist defaultvalue} {
    set families [font families]
    foreach family $preferencelist {
        set index [lsearch $families $family]
        if {$index != -1} {
            return [lindex $families $index]
        }
    }
    return $defaultvalue
}

#
# Work around bug in image handling in Aqua/Tk on Mac OS/X.
#
# Aqua/Tk crashes when one uses image buttons (toolbar!) where the image has
# partial transparency (ie alpha channel ia not just 0 or 1.) Workaround is
# to process the images with opp_resizeimage which somehow converts
# partial transparency to opaque (even though it should keep alpha, and
# indeed does it on all platforms *except* OS/X -- another funny point.)
#
proc iconsWorkaroundForOSX {} {
    global icons
    foreach i [array names icons] {
        set img $icons($i)
        set w [image width $img]
        set h [image height $img]
        set destimg [image create photo -width $w -height $h]
        opp_resizeimage $destimg $img
        set icons($i) $destimg
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

#
# Focuses the given widget.
#
# Sounds simple, right? Well, on the Aqua version of Tk, the focus command
# apparently gets ignored if the containing dialog is not already focused.
# In that case, we'll keep trying in the background until we succeed.
#
proc setInitialDialogFocus w {
    focus $w

    if {[string equal [tk windowingsystem] aqua]}  {
        set f [focus]
        if {$f != $w} {
            after 10 [list setInitialDialogFocusIfStillExists $w]   ;# retry after 10ms
        }
    }
}

proc setInitialDialogFocusIfStillExists w {
    if [winfo exist $w] {
        setInitialDialogFocus $w
    }
}

#
# Focuses the given widget. Only exists because of Aqua.
#
proc waitForFocus w {
    focus $w

    if {[string equal [tk windowingsystem] aqua]}  {
        while {[focus] != $w} {
            focus $w
            update
        }
    }
}


#===================================================================
#    PROCEDURES FOR CREATING NEW 'WIDGET TYPES'
#===================================================================

proc iconbutton {w args} {
    global fonts icons

    if {$args=="-separator"} {
        # space between two buttons
        if {[string equal [tk windowingsystem] aqua]}  {
            # in Aqua/Tk, image button cannot be used because it has a border regardless -bd 0
            frame $w -height 1 -width 6
        } else {
            button $w -image $icons(toolbarsep) -bd 0
        }
    } {
        # button
        #eval button $w -bd 1 $args
        eval button $w -bd 1 -relief flat $args
        bind $w <Enter> [list _changeRelief $w flat raised]
        bind $w <Leave> [list _changeRelief $w raised flat]
    }
    return $w
}

proc _changeRelief {w from to} {
    set current [$w cget -relief]
    if {$current == $from} {
        $w config -relief $to
    }
}

proc packIconButton {w args} {
    eval iconbutton $w $args
    pack $w -anchor n -side left -padx 1 -pady 2
}

proc rpackIconButton {w args} {
    eval iconbutton $w $args
    pack $w -anchor n -side right -padx 1 -pady 2
}

proc iconButton:configure {w icon command tooltip} {
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

proc combo:configure {w list {cmd {}}} {
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

proc combo:onChange {w cmd} {
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
    global help_tips
    frame $w
    label $w.l -anchor w -text $label
    entry $w.e -highlightthickness 0
    #label $w.h -anchor w -text "(Help)" -fg "#0000a0"
    set help_tips($w.l) $helptext
    set help_tips($w.e) $helptext
    #grid $w.l $w.e $w.h -sticky news
    grid $w.l $w.e -sticky news
    grid columnconfigure $w 1 -weight 1
    #bind $w.h <Button-1> [list helplabel:showhelp $helptext %X %Y]
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

proc label-fontcombo {w label {font {}}} {
    # utility function: create a frame with a label+combo for font selection
    frame $w
    label $w.l -anchor w -width 16 -text $label
    combo $w.e {}
    label $w.p -anchor w

    grid $w.l $w.e -sticky news -padx 2 -pady 2
    grid x    $w.p -sticky news -padx 2 -pady 2
    grid columnconfigure $w 1 -weight 1

    $w.e configure -value $font
    $w.e.entry configure -validate all -validatecommand "after idle {fontcombo:update $w}; return 1"
}

# private proc for label-fontcombo
proc fontcombo:update {w} {
    set font [actualFont [fixupFontName [$w.e get]]]
    if {$font==""} {set font "n/a"}
    $w.p configure -text "Actual: $font"
}

proc fontcombo:set {w oldfont} {
    # reuse size from existing font
    set size ""
    catch {
        array set fontprops [font actual $oldfont]
        set size $fontprops(-size)
    }

    # produce font list with the given size
    set fontlist {}
    foreach family [lsort [font families]] {
        if {[regexp {^[A-Za-z]} $family]} {
            if {[llength $family]>1} {set family "\"$family\""}
            lappend fontlist [string trim "$family $size"]
        }
    }
    combo:configure $w $fontlist
    regsub -all "\[{}\]" $oldfont "\"" oldfont
    $w configure -value $oldfont
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

proc fixupFontName {font} {
    # remove special chars that may cause problems
    set font [string map {"{" "" "}" "" "\"" ""} $font]
    set font [string trim $font]
    regsub -- {  +} $font { } font

    # quote font family names that consist of more than one words (everything before the size is family)
    regsub -- {(.+? +[^ ]+?) +([0-9]+)} $font {"\1" \2} font
    return $font
}

# returns the "readable" name of the actual font a font maps to
proc actualFont {font} {
    set actualfont ""
    catch {
        # "font actual" sample output:
        # -family Helvetica -size 24 -weight bold -slant roman -underline 0 -overstrike 0
        array set fontprops [font actual $font]
        set actualfont [list $fontprops(-family) $fontprops(-size)]
        if {$fontprops(-weight)!="normal"} {lappend actualfont $fontprops(-weight)}
        if {$fontprops(-slant)!="roman"} {lappend actualfont $fontprops(-slant)}
        if {$fontprops(-underline)!="0"} {lappend actualfont "underline"}
        if {$fontprops(-overstrike)!="0"} {lappend actualfont "overstrike"}
        set actualfont [string map {"{" "\"" "}" "\""} $actualfont]
    }
    return $actualfont
}

proc changeFontSize {font size} {
    catch {
        return [lreplace $font 1 1 $size]
    }
    return $font
}

proc makeBoldFont {font} {
    if {[lsearch $font "bold"] != -1} {
        return $font
    } else {
        return [lappend $font "bold"]
    }
}

# recurse widget tree and apply font to all text widgets
proc applyFont {class font {w .}} {
    if {"[winfo class $w]"=="$class"} {
        catch {$w config -font $font}
    }
    foreach i [winfo children $w] {
        applyFont $class $font $i
    }
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

    button $w.f.b -relief groove -command [list colorChooser:setColor $w.f.b $w.f.e [winfo toplevel $w]] -width 6
    pack $w.f.b -anchor c -expand 0 -fill none -side left -padx 2 -pady 2

    $w.f.e insert 0 $color
    set dispcolor [resolveDispStrColor $color ""]
    if {$dispcolor!=""} {
       $w.f.b config -background $dispcolor -activebackground $dispcolor
    }
}

proc colorChooser:setColor {b e pwin} {
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
    global help_tips
    frame $w
    label $w.l -justify left -text $text
    #label $w.h -justify left -text "(Help)" -fg "#0000a0"
    set help_tips($w.l) $helptext
    #pack $w.l $w.h -expand 0 -side left -anchor center -fill none -padx 2 -pady 2
    pack $w.l -expand 0 -side left -anchor center -fill none -padx 2 -pady 2
    #bind $w.h <Button-1> [list helplabel:showhelp $helptext %X %Y]
}

# do not use - DOES NOT work on Linux (focus problems...)
proc helplabel:showhelp {text x y} {
    global help_tips
    catch {destroy .helpwin}
    toplevel .helpwin -relief flat
    wm overrideredirect .helpwin true
    wm positionfrom .helpwin program
    wm geometry .helpwin "+[expr $x-200]+[expr $y+5]"
    label .helpwin.tip -text $text -padx 4 -wraplength $help_tips(width) \
                            -bg $help_tips(color) -border 1 -relief solid \
                            -font $help_tips(font) -justify left -takefocus 1
    pack .helpwin.tip
    bind .helpwin.tip <Return> "catch { destroy .helpwin }"
    bind .helpwin.tip <Escape> "catch { destroy .helpwin }"
    bind .helpwin.tip <FocusOut> "catch { destroy .helpwin }"
    bind .helpwin.tip <Button-1> "catch { destroy .helpwin }"
    waitForFocus .helpwin.tip
}


# noteboook --
#
# Create 'tabbed notebook' widget
#
# Usage example:
#  notebook .x
#  notebook:addPage .x p1 Egy
#  notebook:addPage .x p2 Ketto
#  notebook:addPage .x p3 Harom
#  pack .x -expand 1 -fill both
#  label .x.p1.e -text "One"
#  pack .x.p1.e
#
proc notebook {w {side top}} {
    global HAVE_BLT fonts

    if {$HAVE_BLT} {
        blt::tabset $w -tearoff no -relief flat -side top -samewidth no -highlightthickness 0 -font $fonts(normal)
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
proc notebook:addPage {w name label} {
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
        button $tab -text $label -command "notebook:showPage $w $name" -relief flat
        pack $tab -anchor n -expand 0 -fill none -side left

        global nb
        if {$nb($w)==""} {notebook:showPage $w $name}
    }
}

#
# show given notebook page
#
proc notebook:showPage {w name} {
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
    global B2 B3

    # create widget
    frame $w -width 5 -relief raised -borderwidth 1
    if [catch {$w config -cursor size_we}] {
      if [catch {$w config -cursor sb_h_double_arrow}] {
        catch {$w config -cursor sizing}
      }
    }

    # create bindings
    bind $w <Button-1> "vertResizeBar:buttonDown %W %X"
    bind $w <B1-Motion> "vertResizeBar:buttonMove %X"
    bind $w <ButtonRelease-1> "vertResizeBar:buttonRelease %X $wToBeResized"
    bind $w <Button-$B2> "catch {destroy .resizeBar}"
    bind $w <Button-$B3> "catch {destroy .resizeBar}"
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
    global HAVE_BLT B2 B3 fonts
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
                -command [list multicolumnlistbox:bltSortColumn $w $name]
            if {$width!=""} {
                $w column config $name -width $width
            }
        }
        # eliminate "last column quirk" by adding a very wide dummy column:
        $w column insert end "dummy" -text "" -edit no -width 1000
        # right-click support: should select the item (unless already selected)
        bind $w <$B3> {%W selection clearall; %W select set [%W nearest %x %y]}
        #bind $w <Motion> {puts "[%W nearest %x %y] of [%W index view.top]..[%W index view.bottom] -- [%W find view.top view.bottom]"}
    } else {
        # emulate it with listbox widget
        global mclistbox
        listbox $w -font $fonts(mono)
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
            set name [lindex $i 0]
            set label [lindex $i 1]
            set width [lindex $i 2]
            if {$width==""} {set width 15} else {set width [expr $width/6]}
            lappend cols $name
            set mclistbox($w,columnwidth,$name) $width
        }
        set mclistbox($w,columns) $cols
    }
}

#
# private procedure for multicolumnlistbox
#
proc multicolumnlistbox:bltSortColumn {w column} {
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
proc multicolumnlistbox:insert {w rowname data {icon ""}} {
    global icons
    global HAVE_BLT
    if {$HAVE_BLT} {
        if {$icon==""} {set icon $icons(16pixtransp)}
        $w insert end $rowname -data $data -button no -icons [list $icon $icon] -activeicons [list $icon $icon]
    } else {
        global mclistbox
        array set ary $data
        set row ""
        set overshoot 0
        foreach col $mclistbox($w,columns) {
            # catch because it might be missing from the array
            set value "n/a"
            catch {set value $ary($col)}
            set width $mclistbox($w,columnwidth,$col)
            set valuelength [string length $value]
            set padlength [expr $width - $valuelength - $overshoot]
            if {$padlength < 0} {set padlength 0}
            incr overshoot [expr $valuelength + $padlength - $width]
            set padding [string repeat " " $padlength]
            append row "$value$padding  "
        }
        append row [string repeat " " 160]
        append row $rowname
        $w insert end $row
    }
}

#
# Updates a given row.
#
proc multicolumnlistbox:modify {w rowname data} {
    global HAVE_BLT
    if {$HAVE_BLT} {
        set id [$w find -full $rowname]
        if {$id==""} {error "row $rowname not found"}
        $w entry config $id -data $data
    } else {
        error "multicolumnlistbox:modify not supported without BLT!"
    }
}

#
# Returns data from the given row.
#
proc multicolumnlistbox:getRow {w rowname} {
    global HAVE_BLT
    if {$HAVE_BLT} {
        set id [$w find -full $rowname]
        if {$id==""} {error "row $rowname not found"}
        return [$w entry cget $id -data]
    } else {
        error "multicolumnlistbox:getRow not supported without BLT!"
    }
}

#
# Returns true if the given row exists.
#
proc multicolumnlistbox:hasRow {w rowname} {
    global HAVE_BLT
    if {$HAVE_BLT} {
        set id [$w find -full $rowname]
        if {$id!=""} {return 1} else {return 0}
    } else {
        error "multicolumnlistbox:hasRow not supported without BLT!"
    }
}

#
# Returns a list containing the rownames of all of the entries
# that are currently selected. If there are no entries selected,
# then the empty string is returned.
#
proc multicolumnlistbox:curSelection {w} {
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
proc multicolumnlistbox:getRowNames {w} {
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
        error "multicolumnlistbox:getRowNames not supported without BLT!"
    }
}

#
# Delete the given rows.
#
proc multicolumnlistbox:delete {w rownames} {
    global HAVE_BLT
    if {$HAVE_BLT} {
        foreach rowname $rownames {
            set id [$w find -full $rowname]
            if {$id==""} {error "row $rowname not found"}
            $w delete $id
        }
    } else {
        error "multicolumnlistbox:delete not supported without BLT!"
    }
}

#
# Delete all rows.
#
proc multicolumnlistbox:deleteAll {w} {
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
proc multicolumnlistbox:addDummyLine {w} {
    global HAVE_BLT icons
    if {$HAVE_BLT} {
        set icon $icons(16pixtransp)
        $w insert end [opp_null] -icons [list $icon $icon] -activeicons [list $icon $icon]
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

#
# Returns the bounds of the space of the screen that is commonly available for
# application windows. This is the screen area minus the system menu area,
# taskbar area, etc if they exist. This method tries to err on the safe side,
# i.e. rather report a smaller area as usable.
#
proc wmGetDesktopBounds {w arrayname} {
    global tcl_platform
    upvar $arrayname a

    set a(x) [winfo vrootx $w]

    set topmargin 0
    if {$tcl_platform(platform) != "windows"} {set topmargin 20}  ;# menu bar of OS X, Gnome, etc.
    set rooty [winfo vrooty $w]
    set a(y) [expr $rooty + $topmargin]

    set screenwidth [winfo screenwidth $w]
    set a(width) $screenwidth

    set vmargin 50  ;# lost space at top+bottom (due to system menu, taskbar, etc)
    set screenheight [winfo screenheight $w]
    set a(height) [expr $screenheight-$vmargin]

    set a(x2) [expr $a(x)+$a(width)]
    set a(y2) [expr $a(y)+$a(height)]

    #puts "desktop: $a(width)x$a(height) at ($a(x),$a(y))"
}

#
# Returns the typical sizes of window decoration (title bar, resize borders, etc.)
#
proc wmGetDecorationSize {arrayname} {
    upvar $arrayname a
    set a(left) 10
    set a(top) 25
    set a(right) 10
    set a(bottom) 10
    set a(width) [expr $a(left)+$a(right)]
    set a(height) [expr $a(top)+$a(bottom)]
}

# moveToScreen --
#
# utility function: move a toplevel window so that it is fully on the screen
#
proc moveToScreen {w} {
    global tcl_platform

    # Note:
    # - winfo width/height returns the size WITHOUT window manager decorations (title bar, handle border)
    # - winfo x/y returns the position WITHOUT window manager decorations
    # - wm geometry:
    #   - gets/sets the size WITHOUT window manager decorations (title bar, handle border);
    #   - BUT position is WITH decorations, i.e. top-left coordinate of the window's title bar
    # That is, "winfo x/y" and "wm geometry" are NOT compatible!
    # This seems to apply on all platforms. (Tested on Win7, KDE, OS X.)
    #

    update idletasks  ;# needed to get "winfo" and "geom" information updated

    set geom [wm geom $w]
    regexp -- {.*\+(-?[0-9]+)\+(-?[0-9]+)} $geom dummy x y
    set oldx $x
    set oldy $y
    set width [winfo width $w]
    set height [winfo height $w]
    wmGetDesktopBounds $w "desktop"
    wmGetDecorationSize "border"

    # note: x,y are top-left of the wm title bar, unlike width/height which are size of the window contents

    if {$width+$border(width) >= $desktop(width) || $x < $desktop(x)} {
        set x $desktop(x)
    } elseif {$x+$width+$border(width) > $desktop(x2)} {
        set x [expr $desktop(x2)-$border(width)-$width]
    }

    if {$height+$border(height) >= $desktop(height) || $y < $desktop(y)} {
        set y $desktop(y)
    } elseif {$y+$height+$border(height) > $desktop(y2)} {
        set y [expr $desktop(y2)-$border(height)-$height]
    }

    if {$x!=$oldx || $y!=$oldy} {
        # move the window
        if {$tcl_platform(platform) != "windows"} {wm withdraw $w}
        update idletasks  ;# needed
        wm geom $w +$x+$y
        if {$tcl_platform(platform) != "windows"} {wm deiconify $w}
    }
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
    button $w.buttons.okbutton  -text {OK} -width 10 -default active
    button $w.buttons.cancelbutton  -text {Cancel} -width 10

    set padx 5
    set pady 5
    if {[string equal [tk windowingsystem] aqua]}  {
        set padx 15
        set pady 10
    }

    pack $w.buttons -expand 0 -fill x -padx $padx -pady $pady -side bottom
    pack $w.f -expand 1 -fill both -padx $padx -pady $pady -side top
    pack $w.buttons.cancelbutton -side right -padx 2
    pack $w.buttons.okbutton -side right -padx 2
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
    button $w.buttons.closebutton  -text {Close} -width 10 -default active

    set padx 5
    set pady 5
    if {[string equal [tk windowingsystem] aqua]}  {
        set padx 15
        set pady 10
    }

    pack $w.buttons -expand 0 -fill x -padx $padx -pady $pady -side bottom
    pack $w.f -expand 1 -fill both -padx $padx -pady $pady -side top
    pack $w.buttons.closebutton  -anchor n -side right -padx 2
}


# executeCloseDialog --
#
# Executes the dialog.
#
proc executeCloseDialog w {

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
