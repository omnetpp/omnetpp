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
   if {[string compare $tk_patchLevel "8.5.0"]<0} {
      wm deiconify .
      wm title . "Bad news..."
      ttk::frame .f
      pack .f -expand 1 -fill both -padx 2 -pady 2
      label .f.l1 -text "Your version of Tcl/Tk is too old, please upgrade!"
      label .f.l2 -text "Tcl/Tk 8.5.0 or later required."
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
   global B2 B3 CTRL CTRL_ Control


#package require ttk::theme::clearlooks
#ttk::style theme use clearlooks


   if {[string equal [tk windowingsystem] x11]}  {
       package require ttk::theme::clearlooks
       ttk::style theme use clearlooks
       #TODO adjust colors to theme! text background, mytree selection, slider background, etc!
   }

   # work around Tcl bug: these vars got reset when words.tcl was autoloaded
   catch {tcl_wordBreakAfter};
   set tcl_wordchars {\w}
   set tcl_nonwordchars {\W}

   # on Mac OS X, the Aqua version of Tcl/Tk (at least on older ones?) reports
   # right mouse button as button 2, and middle one as button 3. This is
   # quote the opposite of X11 and Windows.
   # see http://support.svi.nl/wiki/MouseButtonsInMacAqua
   set B2 2
   set B3 3
   if {[string equal [tk windowingsystem] aqua]}  {
       set B2 3
       set B3 2
   }

   # we want to use the Command key for shortcuts instead of Ctrl:
   set Control Control  ;# for bind
   set CTRL    Ctrl     ;# for -accel
   set CTRL_   Ctrl+    ;# for help_tips
   if {[string equal [tk windowingsystem] aqua]}  {
       set Control M1
       set CTRL    Meta
       set CTRL_   \u2318
   }

   # also, work around bug in Tk/Aqua image handling
   if {[string equal [tk windowingsystem] aqua]}  {
       iconsWorkaroundForOSX
   }

   # by default, undo/redo bindings are platform-specific -- change it:
   catch {
       event add <<Undo>> <$Control-Key-z>
       event add <<Undo>> <$Control-Key-Z>
       event add <<Redo>> <$Control-Key-y>
       event add <<Redo>> <$Control-Key-Y>
   }

   # for some reason, Ctrl+v (Paste) doesn't work out-of-the box with Tk 8.3/8.4 on Unix,
   # we need the following lines:
   bind Entry <$Control-v> {}
   bind Text <$Control-v> {}

   # set up wheel support for a few extra widget types
   bindMouseWheel Canvas
   bindMouseWheel TreeView

   # set up fonts.
   # Note: font settings will take effect in reflectSettingsInGui,
   # which is called after .tkenvrc has been read
   if {[string equal [tk windowingsystem] x11]} {
      set normalfamily [getFirstAvailableFontFamily {Ubuntu Arial Verdana Helvetica Tahoma "DejaVu Sans" "Nimbus Sans L" FreeSans Sans} unknown]
      set condensedfamily [getFirstAvailableFontFamily {"Ubuntu Condensed" "Arial Narrow" "DejaVu Sans Condensed"} $normalfamily]
      set monofamily [getFirstAvailableFontFamily {"Ubuntu Mono" "DejaVu Sans Mono" "Courier New" "FreeMono" "Courier"} unknown]
      # note: standard font names (not families!) are: TkCaptionFont TkSmallCaptionFont TkTooltipFont TkFixedFont TkHeadingFont TkMenuFont TkIconFont TkTextFont TkDefaultFont
      if {[tk scaling] > 1.5} {set size 11} else {set size 9}
      set fonts(normal)   [list $normalfamily $size]
      set fonts(bold)     [list $normalfamily $size bold]
      set fonts(big)      [list $normalfamily 18]
      set fonts(text)     [list $normalfamily $size] ;# or: $monofamily
      set fonts(balloon)  [list $normalfamily $size]
      set fonts(canvas)   [list $normalfamily $size]
      set fonts(timeline) [list $condensedfamily $size]
   } elseif {[string equal [tk windowingsystem] aqua]} {
      # Mac
      set normalfamily [getFirstAvailableFontFamily {"Lucida Grande" Helvetica} unknown]
      set condensedfamily [getFirstAvailableFontFamily {"Arial Narrow"} $normalfamily]
      set monofamily [getFirstAvailableFontFamily {"Monaco" "Courier"} unknown]
      set size 13
      set fonts(normal)   [list $normalfamily 12]
      set fonts(bold)     [list $normalfamily 12 bold]
      set fonts(big)      [list $normalfamily 18]
      set fonts(text)     [list $monofamily 12]
      set fonts(balloon)  [list $normalfamily 12]
      set fonts(timeline) [list $condensedfamily 12]
      set fonts(canvas)   [list $normalfamily 12]
   } else {
      # Windows
      set normalfamily [getFirstAvailableFontFamily {"Segoe UI" "MS Sans Serif" "Arial"} unknown]
      set condensedfamily [getFirstAvailableFontFamily {"Segoe Condensed" "Gill Sans MT Condensed" "Liberation Sans Narrow"} $normalfamily]
      set monofamily [getFirstAvailableFontFamily {"DejaVu Sans Mono" "Courier New" "Consolas" "Terminal"} unknown]
      if {$normalfamily == "Segoe UI"} {
          set size 9  ;# text in this font appears to be smaller than in MS Sans Serif or Arial
      } else {
          set size 8
      }
      set fonts(normal)   [list $normalfamily $size]
      set fonts(bold)     [list $normalfamily $size]
      set fonts(big)      [list $normalfamily 18]
      set fonts(text)     [list $normalfamily $size] ;# or: $monofamily
      set fonts(balloon)  [list $normalfamily $size]
      set fonts(timeline) [list $condensedfamily $size]
      set fonts(canvas)   [list $normalfamily $size]
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
       option add *Menu.activeBackground gray
       option add *Menu.activeForeground $activefg
       option add *menubar.borderWidth 0
       option add *menubar.activeBorderWidth 0
       option add *menubar.activeBackground gray
       option add *menubar.activeForeground $activefg
   }

   # labelframe
   ttk::style configure TLabelframe -padding {15 8}
   if {[string equal [tk windowingsystem] win32]} {
       # for some reason, the frame label was blue on Windows
       set labelcolor [ttk::style lookup TLabel -foreground]
       ttk::style configure TLabelframe.Label -foreground $labelcolor
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
#    PROCEDURES FOR CREATING NEW 'WIDGET TYPES'
#===================================================================

proc packToolbutton {w args} {
    eval toolbutton $w $args
    pack $w -anchor n -side left -padx 1 -pady 2 -ipadx 1 -ipady 1
}

proc rpackToolbutton {w args} {
    eval toolbutton $w $args
    pack $w -anchor n -side right -padx 1 -pady 2 -ipadx 1 -ipady 1
}

proc label-entry {w label {text {}}} {
    # utility function: create a frame with a label+entry
    ttk::frame $w
    ttk::label $w.l -anchor w -width 16 -text $label
    ttk::entry $w.e
    pack $w.l -anchor center -expand 0 -fill x -padx 2 -pady 2 -side left
    pack $w.e -anchor center -expand 1 -fill x -padx 2 -pady 2 -side right
    $w.e insert 0 $text
}

proc label-entry-help {w label helptext {text {}}} {
    # utility function: create a frame with a label+entry
    global help_tips
    ttk::frame $w
    ttk::label $w.l -anchor w -text $label
    ttk::entry $w.e
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
    ttk::frame $w
    ttk::label $w.l -anchor w -width 16 -text $label
    ttk::entry $w.e
    ttk::button $w.c -text " ... " -command [list chooser:choose $w.e $chooserproc]
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

proc label-spinbox {w label {value {}}} {
    # utility function: create a frame with a label+spinbox
    ttk::frame $w
    ttk::label $w.l -anchor w -width 16 -text $label
    ttk::spinbox $w.e
    pack $w.l -anchor center -expand 0 -fill x -padx 2 -pady 2 -side left
    pack $w.e -anchor center -expand 1 -fill x -padx 2 -pady 2 -side right
    $w.e set $value
}

proc label-sunkenlabel {w label {text {}}} {
    # utility function: create a frame with a label+"readonly entry"
    ttk::frame $w
    ttk::label $w.l -anchor w -width 16 -text $label
    ttk::label $w.e -relief groove -justify left -anchor w
    pack $w.l -anchor center -expand 0 -fill none -padx 2 -pady 2 -side left
    pack $w.e -anchor center -expand 1 -fill x -padx 2 -pady 2 -side right
    $w.e config -text $text
}

proc label-combo {w label list {text {}} {cmd {}}} {
    # utility function: create a frame with a label+combo
    ttk::frame $w
    ttk::label $w.l -anchor w -width 16 -text $label
    ttk::combobox $w.e -values $list
    pack $w.l -anchor center -expand 0 -fill none -padx 2 -pady 2 -side left
    pack $w.e -anchor center -expand 1 -fill x -padx 2 -pady 2 -side right

    if {$text != ""} {
        $w.e set $text
    } else {
        $w.e set [lindex $list 0]
    }
    if {$cmd != ""} {
        bind $w.e <<ComboboxSelected>> $cmd
    }
}

proc label-combo2 {w label list {text {}} {cmd {}}} {
    # start with empty combo box
    label-combo $w $label $list $text $cmd
    $w.e delete 0 end
}

proc label-fontcombo {w label {font {}}} {
    # utility function: create a frame with a label+combo for font selection
    ttk::frame $w
    ttk::label $w.l -anchor w -width 16 -text $label
    ttk::combobox $w.e
    ttk::label $w.p -anchor w

    grid $w.l $w.e -sticky news -padx 2 -pady 2
    grid x    $w.p -sticky news -padx 2 -pady 2
    grid columnconfigure $w 1 -weight 1

    $w.e set $font

    bind $w.e <<ComboboxSelected>> "after idle {fontcombo:update $w}"
    bind $w.e <KeyRelease> "after idle {fontcombo:update $w}"
    after idle [list fontcombo:update $w]
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
    foreach family [fontcombo:getfontfamilies] {
        lappend fontlist [string trim "$family $size"]
    }

    $w configure -values $fontlist
    catch {$w current 0}

    set oldfont [string map {"{" "" "}" "" "\"" ""} $oldfont]
    $w set $oldfont
}

proc fontcombo:getfontfamilies {} {
    set list {}
    foreach family [lsort [font families]] {
        if {[regexp {^[A-Za-z]} $family] && [lsearch -exact $list $family]==-1} {
            lappend list $family
        }
    }
    return $list
}

proc label-text {w label height {text {}}} {
    # utility function: create a frame with a label+text
    ttk::frame $w
    ttk::label $w.l -anchor w -width 16 -text $label
    text $w.t -height $height -width 40
    catch {$w.t config -undo true}
    pack $w.l -anchor n -expand 0 -fill none -padx 2 -pady 2 -side left
    pack $w.t -anchor center -expand 1 -fill both -padx 2 -pady 2 -side right
    $w.t insert 1.0 $text
}

proc label-scale {w label} {
    # utility function: create a frame with a label+scale
    ttk::frame $w
    ttk::label $w.l -anchor w -width 30 -text $label
    ttk::scale $w.e -orient horizontal
    pack $w.l -anchor center -expand 0 -fill none -padx 2 -pady 0 -side left
    pack $w.e -anchor center -expand 1 -fill x -padx 2 -pady 0 -side right
}

proc label-button {w label {text {}}} {
    # utility function: create a frame with label+button
    ttk::frame $w
    ttk::label $w.l -anchor w -width 16 -text $label
    ttk::button $w.e
    pack $w.l -anchor center -expand 0 -fill none -padx 2 -pady 2 -side left
    pack $w.e -anchor center -expand 1 -fill x -padx 2 -pady 2 -side right
    $w.e config -text $text
}

proc label-check {w label first var} {
    # utility function: create a frame with a label+radiobutton for choose
    ttk::frame $w
    ttk::label $w.l -anchor w -width 16 -text $label
    ttk::frame $w.f
    ttk::checkbutton $w.f.r1 -text $first -variable $var

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

    ttk::frame $w
    ttk::label $w.l -anchor w -width 16 -text $label
    ttk::frame $w.f
    pack $w.l -anchor nw -expand 0 -fill none -side left -padx 2 -pady 2
    pack $w.f -anchor n -expand 1 -fill x -side left

    ttk::entry $w.f.e
    pack $w.f.e -anchor center -expand 1 -fill x -side left -padx 2 -pady 2

    ttk::button $w.f.b -relief groove -command [list colorChooser:setColor $w.f.b $w.f.e [winfo toplevel $w]] -width 6
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

proc commentlabel {w text {preferredwidth 60}} {
    # Note: make sure to specify -fill x when packing!
    ttk::label $w -text $text -anchor nw -justify left -width $preferredwidth
    bind $w <Configure> {%W configure -wraplength [expr {%w-4}] }
}

proc labelwithhelp {w text helptext} {
    global help_tips
    ttk::frame $w
    ttk::label $w.l -justify left -text $text
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
#TODO: add wm transient!!! maybe it will work
    ttk::label .helpwin.tip -text $text -padx 4 -wraplength $help_tips(width) \
                            -bg $help_tips(color) -border 1 -relief solid \
                            -font $help_tips(font) -justify left -takefocus 1
    pack .helpwin.tip
    bind .helpwin.tip <Return> "catch { destroy .helpwin }"
    bind .helpwin.tip <Escape> "catch { destroy .helpwin }"
    bind .helpwin.tip <FocusOut> "catch { destroy .helpwin }"
    bind .helpwin.tip <Button-1> "catch { destroy .helpwin }"
    waitForFocus .helpwin.tip
}

proc ttkTreeview:deleteAll {tree} {
    $tree delete [$tree children {}]
}

proc panedwindow:getsashposition {w} {
    set pos [$w sash coord 0]
    if {[$w cget -orient]=="horizontal"} {
        return [lindex $pos 0]
    } else {
        return [lindex $pos 1]
    }
}

proc panedwindow:setsashposition {w pos} {
    # This essentially does {$w sash place 0 $pos}, but well:
    # 1. "sash place" does nothing if child hasn't been laid out yet, so we may need "after idle"
    # 2. "sash place" allows 0px too, which makes the sash completely unnoticeable to the user
    # 3. allows pos=="" for convenience (it is a no-op)
    if {$pos == ""} {return}
    if {$pos < 5} {set pos 5}
    panedwindow:dosetsashposition $w $pos
    if {[panedwindow:getsashposition $w] != $pos} {
        after idle [list after idle [list panedwindow:dosetsashposition $w $pos]]
    }
}

proc panedwindow:dosetsashposition {w pos} {
    if {[$w cget -orient]=="horizontal"} {
        $w sash place 0 $pos 0
    } else {
        $w sash place 0 0 $pos
    }
}

#===================================================================
#    UTILITY PROCEDURES
#===================================================================

#
# Assigns variables from a list. Example: setvars {a b c} {1 2 3}
#
proc setvars {vars vals} {
    if {[llength $vars] != [llength $vals]} {error "number of vars and length of values list don't match"}
    uplevel [list foreach $vars $vals {}]
}

# wsize --
#
# Utility to set a widget's size to exactly width x height pixels.
# Usage example:
#    button .b1 -text "OK"
#    pack [wsize .b1 40 40] -side top -expand 1 -fill both
#
proc wsize {w width height} {
    set f ${w}_f
    ttk::frame $f -width $width -height $height
    place $w -in $f -x 0 -y 0 -width $width -height $height
    raise $w
    return $f
}


#
# Copies the given string to the clipboard
#
proc setClipboard {str} {
    clipboard clear
    clipboard append -- $str
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
