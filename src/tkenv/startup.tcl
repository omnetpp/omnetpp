#=================================================================
#  STARTUP.TCL - part of
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
# This proc is invoked from C++ to set up the Tkenv GUI
#
proc startTkenv {} {
    global OMNETPP_IMAGE_PATH OMNETPP_PLUGIN_PATH OMNETPP_LIB_DIR
    global auto_path

    wm withdraw .
    checkTclTkVersion
    lappend auto_path $OMNETPP_LIB_DIR
    package require tkpath
    determineFontScaling
    setupTkOptions
    initBalloons
    createOmnetppWindow
    setApplicationIcon
    puts ""
    loadBitmaps $OMNETPP_IMAGE_PATH
    loadPlugins $OMNETPP_PLUGIN_PATH
    loadTkenvrc "~/.tkenvrc"
    loadTkenvrc ".tkenvrc"
    reflectSettingsInGui
    determineClocksPerSec
}

#
# This proc is invoked from C++ after startTkenv
#
proc startupCommands {} {
    # let the main window appear, otherwise startup dialogs will have
    # problems figuring out where to come up;
    # note: moving this 'update' into startTkenv will cause other problems
    update

    set configname [opp_getsimoption default_config]
    set runnumber [opp_getsimoption default_run]

    # if no configname is specified but there's only one ("General"),
    # and it also contains a network, use that without a question.
    if {$configname==""} {
        set confignames [opp_getconfignames]
        set cfgnetworkname [opp_getvaluefromconfig "network"]  ;# CFGID_NETWORK
        if {($confignames=="General" && $cfgnetworkname!="")} {
            set configname "General"
        }
    }

    if {$configname != ""} {
        # set up the selected config and run number
        opp_newrun $configname $runnumber
        reflectRecordEventlog
        if [opp_isnotnull [opp_object_systemmodule]] {
            notifyPlugins newNetwork
        }
    } else {
        # ask the user to select a network or a config
        if {$confignames=={} || $confignames=="General"} {
            newNetwork
        } else {
            newRun
        }
    }
}


#
# Check for the required Tcl/Tk version
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


#
# Sets up fonts and various options in Tk in order to have consistent look
# on Linux, Windows and OS X, and across different Tk versions.
#
proc setupTkOptions {} {
   global icons tcl_platform tk_version
   global tcl_wordchars tcl_nonwordchars
   global B2 B3 BBack BFwd CTRL CTRL_ Control arrowcursor

   package require autoscroll

   # set theme
   if {[string equal [tk windowingsystem] x11]}  {
       package require ttk::theme::clearlooks
       ttk::style theme use clearlooks
   }

   # set color of non-themed widgets we use match those of themed ones
   set background [ttk::style lookup . -background]
   option add *Canvas.Background $background
   option add *Panedwindow.Background $background
   option add *Scale.Background $background
   option add *Toplevel.Background $background  ;# reduces dialog create/destroy time flicker
   option add *Frame.Background $background

   # additional options
   if {[string equal [tk windowingsystem] x11]}  {
       option add *Scrollbar.width  12
       option add *Menu.activeBorderWidth 0
       option add *menubar.borderWidth 0
       option add *menubar.activeBorderWidth 0
   }

   # menu styling on Linux
   if {[string equal [ttk::style theme use] clearlooks]} {
       # from clearlooks.tcl:
       array set colors {
           -frame          "#efebe7"
           -lighter        "#f5f3f0"
           -dark           "#cfcdc8"
           -darker         "#9e9a9e"
           -darkest        "#d4cfca"
           -selectbg       "#7c99ad"
           -selectfg       "#ffffff"
           -disabledfg     "#b5b3ac"
           -entryfocus     "#6f9dc6"
           -tabbg          "#c9c1bc"
           -tabborder      "#b5aca7"
           -troughcolor    "#d7cbbe"
           -troughborder   "#ae9e8e"
           -checklight     "#f5f3f0"
       }

       set foreground black
       set background $colors(-frame)
       set activefg   $colors(-selectfg)
       set activebg   $colors(-selectbg)
       set disabledfg $colors(-disabledfg)
       set disabledbg $colors(-frame)

       option add *menubar.Background $background
       option add *menubar.Foreground $foreground
       option add *menubar.activeBackground $activebg
       option add *menubar.activeForeground $activefg
       option add *menubar.disabledBackground $disabledbg
       option add *menubar.disabledForeground $disabledfg

       option add *Menu.Background $background
       option add *Menu.Foreground $foreground
       option add *Menu.selectColor $foreground
       option add *Menu.activeBackground $activebg
       option add *Menu.activeForeground $activefg
       option add *Menu.disabledBackground $disabledbg
       option add *Menu.disabledForeground $disabledfg
   }

   # labelframe
   ttk::style configure TLabelframe -padding {15 8}
   if {[string equal [tk windowingsystem] win32]} {
       # for some reason, the frame label was blue on Windows
       set labelcolor [ttk::style lookup TLabel -foreground]
       ttk::style configure TLabelframe.Label -foreground $labelcolor
   }

   # work around Tcl bug: these vars got reset when words.tcl was autoloaded
   catch {tcl_wordBreakAfter};
   set tcl_wordchars {\w}
   set tcl_nonwordchars {\W}

   # on Linux and OS X, "arrow" points right not left
   if {[string equal [tk windowingsystem] win32]} {
       set arrowcursor arrow
   } else {
       set arrowcursor left_ptr
   }

   # turn on anti-aliasing on OS/X
   catch {set tk::mac::CGAntialiasLimit 1}

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

   # mouse side buttons (back/forward); they are numbered differently on Linux
   # and OS X, and don't work at all on Windows
   if {[string equal [tk windowingsystem] aqua]}  {
       set BBack 4
       set BFwd  5
   } else {  # Linux
       set BBack 8
       set BFwd  9
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

   # hitting Enter on focused buttons should trigger the action
   bind Button <Return> {%W invoke}

   # set up wheel support for a few extra widget types
   bindMouseWheel Canvas
   bindMouseWheel TreeView

   # set up Control+wheel to send <<ZoomIn>> and <<ZoomOut>> events
   #setupZoomEvents

   # set up fonts
   if {[string equal [tk windowingsystem] win32]} {
      # Windows
      set normalfamily [getFirstAvailableFontFamily {"Segoe UI" "MS Sans Serif" "Arial"} unknown]
      set condensedfamily [getFirstAvailableFontFamily {"Segoe Condensed" "Gill Sans MT Condensed" "Liberation Sans Narrow"} $normalfamily]
      set monofamily [getFirstAvailableFontFamily {"DejaVu Sans Mono" "Courier New" "Consolas" "Terminal"} unknown]
      set size 9
      if {$normalfamily == "Segoe UI"} {set size 9}  ;# text in this font appears to be smaller than in MS Sans Serif or Arial
   } elseif {[string equal [tk windowingsystem] aqua]} {
      # Mac
      set normalfamily [getFirstAvailableFontFamily {"Lucida Grande" Helvetica} unknown]
      set condensedfamily [getFirstAvailableFontFamily {"Arial Narrow"} $normalfamily]
      set monofamily [getFirstAvailableFontFamily {"Monaco" "Courier"} unknown]
      set size 13
   } else {
      # Linux and other systems
      set normalfamily [getFirstAvailableFontFamily {Ubuntu Arial Verdana Helvetica Tahoma "DejaVu Sans" "Nimbus Sans L" FreeSans Sans} unknown]
      set condensedfamily [getFirstAvailableFontFamily {"Ubuntu Condensed" "Arial Narrow" "DejaVu Sans Condensed"} $normalfamily]
      set monofamily [getFirstAvailableFontFamily {"Ubuntu Mono" "DejaVu Sans Mono" "Courier New" "FreeMono" "Courier"} unknown]
      if {[tk scaling] > 1.5} {set size 11} else {set size 9}
   }

   set simtimefontsize [expr $size+2]

   # Note: standard font names (not families!) are: TkCaptionFont TkSmallCaptionFont
   # TkTooltipFont TkFixedFont TkHeadingFont TkMenuFont TkIconFont TkTextFont TkDefaultFont
   # but we use only 2 of them:

   # create fonts: TimelineFont, CanvasFont, LogFont, BIGFont, BoldFont; additionally
   # we'll use TkDefaultFont and TkTooltipFont, but not the other Tk default fonts.
   font configure TkDefaultFont -family $normalfamily -size $size
   font configure TkTooltipFont -family $normalfamily -size $size
   font configure TkFixedFont -family $monofamily -size $size
   font create BoldFont -family $normalfamily -size $size -weight bold
   font create SimtimeFont -family $monofamily -size $simtimefontsize -weight bold
   font create EventnumFont -family $monofamily -size $simtimefontsize
   font create TimelineFont -family $condensedfamily -size $size
   font create CanvasFont -family $normalfamily -size $size
   font create LogFont -family $monofamily -size $size
   font create BIGFont -family $normalfamily -size 18
   updateTkpFonts

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

   # enabling antialiasing in tkpath
   set tkp::antialias 1
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

proc isTkAtLeast {version} {
    global tk_patchLevel

    # must use dictionary comparison due to the embedded numbers
    if {[strdictcmp $tk_patchLevel $version] < 0} {
        return 0
    } else {
        return 1
    }
}

proc setApplicationIcon {} {
    global tcl_platform icons

    set iconphoto_main $icons(logo128m)
    set iconphoto_other $icons(logo128w)
    if {$tcl_platform(platform) == "windows"} {
        if {![isTkAtLeast "8.5.6"]} {
            # Tk bug #2504402: "On Windows the wm iconphoto command only works with
            # 32-bit color displays. Other display settings produce a black icon."
            # This bug appears to have been fixed in Tk 8.5.6. For earlier versions,
            # only turn on icon for 32-bit displays.
            if {[winfo screendepth .] == 32} {
                # Bug #1467997: "the displayed icons have red and blue colors transposed."
                # This bug was was fixed in 8.4.16. For earlier versions, we manually swap
                # the R and B channels.
                if {![isTkAtLeast "8.4.16"]} {
                    opp_swapredandblue $iconphoto_other
                    opp_swapredandblue $iconphoto_main
                }
                # note: on win7, without the "after" command wm iconphoto causes startup glitches (empty window+delay)
                after 200 "wm iconphoto . -default $iconphoto_other; wm iconphoto . $iconphoto_main"
            }
        } else {
            # note: on win7, without the "after" command wm iconphoto causes startup glitches (empty window+delay)
            after 200 "wm iconphoto . -default $iconphoto_other; wm iconphoto . $iconphoto_main"
        }
    } elseif {[string equal [tk windowingsystem] aqua]}  {
        # On OS X, wm iconphoto doesn't work (does nothing or raises an error);
        # instead, we need to use the TkDock extension.
        setOSXDockIcon

        # note: repeat because the first update resets the dock icon on OS X!
        after idle setOSXDockIcon
        # or maybe twice: after idle {after idle setOSXDockIcon}
    } else {
        # On linux, 8.4.19 was tested and known to be working.
        wm iconphoto . -default $iconphoto_other
        wm iconphoto . $iconphoto_main
    }
}

proc setOSXDockIcon {} {
    global OMNETPP_IMAGE_PATH

    # find the icon to use
    set icon ""
    foreach dir [splitPath $OMNETPP_IMAGE_PATH] {
        set f [file join $dir "logo/logo128m.png"]
        if [file exists $f] {
            set icon $f
            break;
        }
    }
    if {$icon==""} {
        debug "application icon not found"
        return
    }

    if [catch {package require tkdock}] {
        debug "could not load tkdock"
        return
    }

    # set the icon
    tkdock::switchIcon $icon
}

#
# Figure out the conversion rate between Tk font sizes (points)
# and Tkpath font sizes (-fontsize, pixels), experimentally.
#
proc measureFontScaling {} {
    set fontFamily "Arial"

    canvas .c
    font create tmpFont -family $fontFamily -size 100
    .c create text 0 0 -text LTgy -font tmpFont -anchor nw -tags txt

    tkp::canvas .cc
    .cc create ptext 0 0 -text LTgy -fontfamily $fontFamily -fontsize 100 -textanchor nw -tags txt

    set h1 [lindex [.c bbox txt] 3]
    set h2 [lindex [.cc bbox txt] 3]
    set f [expr $h1/double($h2)*1.1]; # 1.1 is an empirical correction factor, needed because canvas and tkp::canvas compute text bounding boxes slightly differently

    destroy .c
    destroy .cc
    font delete tmpFont

    return $f
}

proc determineFontScaling {} {
    global fontScaling tcl_platform

    # We only trust system DPI settings on Windows and OS X. On other platforms
    # (e.g. Linux), manually measuring fonts gives more trustworthy results.
    if {$tcl_platform(platform) == "unix"} {
        set fontScaling [measureFontScaling]
    } else {
        #set dpi [winfo fpixels . 1i]
        #set fontScaling [expr $dpi/72.0)]
        set fontScaling [tk scaling]
    }
}

#
# Tkpath doesn't know about Tk fonts, instead it has -fontfamily and
# -fontsize options for its ptext items. Moreover, -fontsize is in pixels,
# while Tk fonts are in points.
#
proc getFontAsTkpOptions {font} {
    global fontScaling
    array set tmp [font actual $font]
    set pxsize [expr int($tmp(-size) * $fontScaling + 0.5)];
    return [list -fontfamily $tmp(-family) -fontsize $pxsize]
}

#
# Update tkpFonts() array for all known fonts in Tkenv. Elements of
# tkpFonts() can be used for ptext items on tkp::canvas widgets.
#
proc updateTkpFonts {} {
    global tkpFont
    foreach font {TkDefaultFont TkTooltipFont TkFixedFont BoldFont SimtimeFont EventnumFont TimelineFont CanvasFont LogFont BIGFont} {
        set tkpFont($font) [getFontAsTkpOptions $font]
    }
}


#===================================================================
#    LOAD BITMAPS
#===================================================================

set bitmap_ctr 0

proc splitPath {path} {
   global tcl_platform

   # On Windows, we use ";" to separate directories in $path. Using ":" (the
   # Unix separator) would cause trouble with dirs containing drive letter
   # (like "c:\bitmaps"). Using a space is also not an option (think of
   # "C:\Program Files\...").
   set sep [expr {$tcl_platform(platform) == "unix" ? ":;" : ";"}]
   return [split $path $sep]
}

proc loadBitmaps {path} {
   global tcl_platform bitmaps bitmap_ctr

   foreach dir [splitPath $path] {
       if {$dir!=""} {
           puts -nonewline "Loading images from '$dir': "
           doLoadBitmaps $dir ""
           puts ""
       }
   }

   if {$bitmap_ctr==0} {
       puts "*** no images in $path"
   }

   puts ""
}

proc doLoadBitmaps {dir prefix} {
   set files [concat [glob -nocomplain -- [file join $dir {*.gif}]] \
                     [glob -nocomplain -- [file join $dir {*.png}]]]

   # load bitmaps from this directory
   set numTotal 0
   set numLoaded 0
   foreach f $files {
      if [loadImage $f $prefix] {incr numLoaded}
      incr numTotal
   }
   if {$numLoaded==$numTotal} {
      puts -nonewline "$prefix*: $numTotal  "
   } else {
      puts -nonewline "$prefix*: $numLoaded (skipped [expr $numTotal-$numLoaded])  "
   }

   # recurse into subdirs
   foreach f [glob -nocomplain -- [file join $dir {*}]] {
      if {[file isdirectory $f] && [file tail $f]!="CVS" && [file tail $f]!=".svn"} {
         doLoadBitmaps "$f" "$prefix[file tail $f]/"
      }
   }
}

proc loadImage {f {prefix ""} {name ""}} {
   global bitmap_ctr
   if {$name==""} {
       set name [file tail [file rootname $f]]
   }
   set img "i[incr bitmap_ctr]$name"
   if [catch {
      set loaded 0
      image create photo $img -file $f
      #fixupImageIfNeeded $img - no longer needed with tkpath
      set size "n" ;#default
      regexp -- {^(.*)_(vl|xl|l|n|s|vs|xs)$} $name dummy name size
      set loaded [doAddBitmap $img $prefix $name $size]
   } err] {
      puts -nonewline "(*** cannot load $f: $err ***) "
   }
   return $loaded
}

# On recent versions of OS X, images with partial transparency (alpha not 0 or 255)
# are not displayed at all. Workaround: replace all alpha values with 0 or 255.
proc fixupImageIfNeeded {img} {
   if {[string equal [tk windowingsystem] aqua]} {
       opp_reducealpha $img 128
   }
}

# register loaded image
proc doAddBitmap {img prefix name size} {
   global bitmaps

   # skip if already exists
   if [info exists bitmaps($prefix$name,$size)] {
       image delete $img
       return 0
   }

   # access via the s= display string option
   set bitmaps($prefix$name,$size) $img

   # access by the legacy way
   if {$size=="n"} {
       set bitmaps($prefix$name) $img
   } else {
       set bitmaps($prefix${name}_$size) $img
   }
   return 1
}

#===================================================================
#    LOAD PLUGINS
#===================================================================

proc loadPlugins {path} {
   global tcl_platform

   # On Windows, we use ";" to separate directories in $path. Using ":" (the
   # Unix separator) would cause trouble with dirs containing drive letter
   # (like "c:\bitmaps"). Using a space is also not an option (think of
   # "C:\Program Files\...").
   #
   # We'd ever wanted to read plugins relative to the executable,
   # its path is available via [info nameofexecutable].

   if {$tcl_platform(platform) == "unix"} {
       set dllpattern "*.so*"
       set sep {:;}
   } else {
       set dllpattern "*.dll"
       set sep {;}
   }

   puts "Plugin path: $path"

   set tclfiles 0
   set dllfiles 0
   foreach dir [split $path $sep] {

      # load tcl files
      set files [lsort [glob -nocomplain -- [file join $dir {*.tcl}]]]
      incr tclfiles [llength $files]
      if {[llength $files]!=0} {
          puts -nonewline "Loading tcl files from $dir: "
          foreach file $files {
             if [catch {source $file} errmsg] {
                 puts ""
                 puts "*** error sourcing $file: $errmsg"
             } else {
                 set name [file tail $file]
                 puts -nonewline "$name "
             }
          }
         puts ""
      }

      # load dynamic libraries
      set files [lsort [glob -nocomplain -- [file join $dir $dllpattern]]]
      incr dllfiles [llength $files]
      if {[llength $files]!=0} {
          puts -nonewline "Loading so/dll files from $dir: "
          foreach file $files {
             if [catch {opp_loadlib $file} errmsg] {
                 puts ""
                 puts "*** error loading shared library $file: $errmsg"
             } else {
                 set name [file tail $file]
                 puts -nonewline "$name "
             }
          }
          puts ""
      }
   }
}

#===================================================================
#    PLUGIN API
#===================================================================

proc registerPlugin {pluginname} {
    global pluginlist
    lappend pluginlist $pluginname
    #debug "registered plugin \"$pluginname\""
}

proc notifyPlugins {command args} {
    global pluginlist
    foreach pluginname $pluginlist {
        #debug "invoking plugin callback $pluginname:$command"
        eval $pluginname:$command $args
    }
}

