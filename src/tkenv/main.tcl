#==========================================================================
#  MAIN.TCL -
#            part of the Tcl/Tk windowing environment of
#                            OMNeT++
#==========================================================================

#----------------------------------------------------------------#
#  Copyright (C) 1992,99 Andras Varga
#  Technical University of Budapest, Dept. of Telecommunications,
#  Stoczek u.2, H-1111 Budapest, Hungary.
#
#  This file is distributed WITHOUT ANY WARRANTY. See the file
#  `license' for details on this and other legal matters.
#----------------------------------------------------------------#

#------
# Parts of this file were created using Stewart Allen's Visual Tcl (vtcl)
#------

#===================================================================
#    MAIN OMNET++ WINDOW
#===================================================================

proc create_omnetpp_window {} {
    set w .omnetpp
    if {[winfo exists $w]} {
        wm deiconify $w; return
    }

    global icons fonts tcl_version help_tips

    toplevel $w -class Toplevel
    wm focusmodel $w passive
    wm geometry $w 540x275
    wm maxsize $w 1009 738
    wm minsize $w 1 1
    wm overrideredirect $w 0
    wm resizable $w 1 1
    wm deiconify $w
    wm title $w "OMNeT++/Tkenv"
    wm protocol $w WM_DELETE_WINDOW {exit_omnetpp}

    #################################
    # Menu bar
    #################################

    if {$tcl_version < 8.0} {
        frame $w.menubar -borderwidth 1 -relief raised
        pack $w.menubar -expand 0 -fill x -side top
    } else {
        menu $w.menubar
        $w config -menu $w.menubar
    }

    # Create menus
    foreach i {
       {filemenu     -$label_opt File -underline 0}
       {simulatemenu -$label_opt Simulate -underline 0}
       {tracemenu    -$label_opt Trace -underline 0}
       {inspectmenu  -$label_opt Inspect -underline 0}
       {viewmenu     -$label_opt View -underline 0}
       {optionsmenu  -$label_opt Options -underline 0}
       {helpmenu     -$label_opt Help -underline 0}
    } {
       if {$tcl_version < 8.0} {
           set label_opt "text"; set m ".m"
           set mb [eval menubutton $w.menubar.$i -padx 4 -pady 3]
           menu $mb.m -tearoff 0
           $mb config -menu $mb.m
       } else {
           set label_opt "label"; set m ""
           eval $w.menubar add cascade -menu $w.menubar.$i
           menu "$w.menubar.[lindex $i 0]" -tearoff 0
       }
    }

    # File menu
    foreach i {
      {command -command new_run -label {Select Run #...} -underline 7}
      {command -command new_network -label {Select network...} -underline 7}
      {command -command create_snapshot -label {Create snapshot...} -underline 7}
      {separator}
      {command -command exit_omnetpp -label Exit -underline 1}
    } {
      eval $w.menubar.filemenu$m add $i
    }

    # Simulate menu
    #  {command -command init_step -label {First real event} -underline 1}
    #  {separator}
    foreach i {
      {command -command one_step -label {One step} -underline 4}
      {command -command slow_exec -label {Slow execution} -underline 1}
      {separator}
      {command -command run -label {Run} -underline 0 ;# -accelerator F9}
      {command -command run_fast -label {Fast run (rare display updates)} -underline 0}
      {command -command run_notracing -label {Express run (tracing off)} -underline 0}
      {command -command run_until -label {Run until...} -underline 4}
      {separator}
      {command -command stop_simulation -label {Stop execution} -underline 0}
      {separator}
      {command -command call_finish -label {Call finish() for all modules} -underline 0}
      {command -command rebuild -label {Rebuild network} -underline 1}
    } {
      eval $w.menubar.simulatemenu$m add $i
    }

    # Trace menu
    foreach i {
      {command -command module_windows -label {Module output...} -underline 0}
      {command -command message_windows -label {Message sending...} -underline 8}
      {separator}
      {command -command clear_windows -label {Clear main window} -underline 0}
    } {
      eval $w.menubar.tracemenu$m add $i
    }

    # Inspect menu
    foreach i {
      {command -command inspect_systemmodule -label {Network} -underline 0}
      {command -command inspect_messagequeue -label {Scheduled events (FES)} -underline 0}
      {separator}
      {cascade -label {Linked-in components} -underline 10 -menu $w.menubar.inspectmenu$m.components}
      {separator}
      {command -command inspect_anyobject -label {From list of all objects...} -underline 0}
      {command -command inspect_matching -label {By pattern matching...} -underline 3}
      {command -command inspect_bypointer -label {By pointer...} -underline 4}
    } {
      eval $w.menubar.inspectmenu$m add $i
    }

    # Inspect|Components menu
    menu $w.menubar.inspectmenu$m.components -tearoff 0
    foreach i {
      {command -command inspect_networks -label {Available networks} -underline 10}
      {command -command inspect_moduletypes -label {Module types} -underline 0}
      {command -command inspect_channeltypes -label {Channel types} -underline 0}
      {command -command inspect_functions -label {Registered functions} -underline 11}
    } {
      eval $w.menubar.inspectmenu$m.components add $i
    }

    # Inspect|Special menu
    #  {cascade -label {Special} -underline 1 -menu $w.menubar.inspectmenu$m.special}
    # menu $w.menubar.inspectmenu$m.special -tearoff 0
    # foreach i {
    #  {command -command inspect_anyobject -label {From list of all objects...} -underline 0}
    #  {command -command inspect_matching -label {By pattern matching...} -underline 3}
    #  {command -command inspect_bypointer -label {By pointer...} -underline 4}
    #} {
    #  eval $w.menubar.inspectmenu$m.special add $i
    #}

    # View menu
    foreach i {
      {command -label {Ini file} -underline 0 -command {view_file [opp_getfilename ini]}}
      {command -label {README} -underline 0 -command {view_file readme}}
      {separator}
      {command -label {Snapshot file} -underline 0 -command {view_file [opp_getfilename snapshot]}}
      {command -label {Output vector file} -underline 0 -command {view_file [opp_getfilename outvector]}}
      {command -label {Parameter changes file} -underline 0 -command {view_file [opp_getfilename parchanges]}}
      {command -label {Inspector list file} -underline 1 -command {view_file inspect.lst}}
      {separator}
      {command -label {View/Edit file...} -underline 0 -command {edit_textfile}}
    } {
      eval $w.menubar.viewmenu$m add $i
    }

    # Options menu
    foreach i {
      {command -command simulation_options -label {Simulation options...} -underline 0}
      {separator}
      {command -label {Load inspector list...} -underline 0 -command load_inspectorlist}
      {command -label {Save open inspectors list...} -underline 0 -command save_inspectorlist}
      {command -label {Update inspectors now} -underline 0 -command opp_updateinspectors}
      {separator}
      {command -command show_console -label {TCL console} -underline 0}
    } {
      eval $w.menubar.optionsmenu$m add $i
    }

    # Help menu
    foreach i {
      {command -command about -label {About OMNeT++} -underline 0}
    } {
      eval $w.menubar.helpmenu$m add $i
    }

    # Pack menus on menubar
    if {$tcl_version < 8.0} {
        foreach i {
          filemenu simulatemenu tracemenu inspectmenu viewmenu optionsmenu helpmenu
        } {
          pack $w.menubar.$i -anchor n -expand 0 -fill none -side left
        }
    }

    #################################
    # Create toolbar
    #################################

    frame $w.toolbar -relief raised -borderwidth 1
    foreach i {
      {sep0     -separator}
      {step     -image $icons(step)    -command {one_step}}
      {sep1     -separator}
      {run      -image $icons(run)     -command {run}}
      {fastrun  -image $icons(fast)    -command {run_fast}}
      {exprrun  -image $icons(express) -command {run_notracing}}
      {sep2     -separator}
      {until    -image $icons(until)   -command {run_until}}
      {sep3     -separator}
      {stop     -image $icons(stop)    -command {stop_simulation}}
      {sep4     -separator}
      {network  -image $icons(network) -command {inspect_systemmodule}}
      {fes      -image $icons(fes)     -command {inspect_messagequeue}}
    } {
      set b [eval iconbutton $w.toolbar.$i]
      pack $b -anchor n -expand 0 -fill none -side left -padx 0 -pady 2
    }

    set help_tips($w.toolbar.step)    {Execute one event}
    set help_tips($w.toolbar.run)     {Run with full animation}
    set help_tips($w.toolbar.fastrun) {Run faster: no animation and rare inspector updates}
    set help_tips($w.toolbar.exprrun) {Run at full speed: no text output, animation or inspector updates}
    set help_tips($w.toolbar.until)   {Run until time or event number}
    set help_tips($w.toolbar.stop)    {Stop simulation (if running)}
    set help_tips($w.toolbar.network) {Inspect network}
    set help_tips($w.toolbar.fes)     {Inspect Future Event Set}

    #################################
    # Create status bar
    #################################

    frame $w.statusbar
    label $w.statusbar.networklabel \
        -relief groove -text {(no network set up)} -width 19
    label $w.statusbar.eventlabel \
        -justify left -relief groove -text {Event #0} -width 12
    label $w.statusbar.timelabel \
        -justify left -relief groove -text {T=0.0000000 (0.00s)} -width 20
    label $w.statusbar.nextlabel \
        -justify left -relief groove -text Next: -width 26

    pack $w.statusbar.networklabel -anchor n -expand 1 -fill x -side left
    pack $w.statusbar.eventlabel -anchor n -expand 1 -fill x -side left
    pack $w.statusbar.timelabel -anchor n -expand 1 -fill x -side left
    pack $w.statusbar.nextlabel -anchor n -expand 1 -fill x -side right

    #################################
    # Create main display area
    #################################

    frame $w.main -borderwidth 1 -height 30 -relief sunken -width 30
    text $w.main.text -yscrollcommand "$w.main.sb set"
    scrollbar $w.main.sb -command "$w.main.text yview"
    $w.main.text tag configure event -foreground blue

    pack $w.main.sb -anchor center -expand 0 -fill y  -side right
    pack $w.main.text -anchor center -expand 1 -fill both -side left

    #################################
    # Pack everything
    #################################
    if {$tcl_version < 8.0} {
        pack $w.menubar   -anchor center -expand 0 -fill x -side top
    }
    pack $w.toolbar   -anchor center -expand 0 -fill x -side top
    pack $w.statusbar -anchor center -expand 0 -fill x -side top
    pack $w.main      -anchor center -expand 1 -fill both -side top

    global tcl_platform
    if {$tcl_platform(platform) == "windows"} {
        update
    }
}

#===================================================================
#    LOAD BITMAPS
#===================================================================

proc load_bitmaps {path} {
   global tcl_platform

   puts "Loading bitmaps from $path:"

   # On Windows, we use ";" to separate directories in $path. Using ":" (the 
   # Unix separator) would cause trouble with dirs containing drive letter
   # (like "c:\bitmaps"). Using a space is also not an option (think of
   # "C:\Program Files\...").

   if {$tcl_platform(platform) == "unix"} {
       set sep {:;}
   } else {
       set sep {;}
   }

   set files {}
   foreach dir [split $path $sep] {
       set files [concat $files \
                     [glob -nocomplain -- [file join $dir {*.gif}]] \
                     [glob -nocomplain -- [file join $dir {*.xpm}]] \
                     [glob -nocomplain -- [file join $dir {*.xbm}]]]
   }
   if {[llength $files] == 0} {puts "none found!"; return}

   foreach f $files {

      set type ""
      case [string tolower [file extension $f]] {
        {.xbm} {set type bitmap}
        {.xbm} {set type photo}
        {.gif} {set type photo}
      }
      if {$type==""} {error "load_bitmaps: internal error"}

      set name [string tolower [file tail [file rootname $f]]]
      if {$name=="proc"} {
         puts -nonewline "(*** $name -- Tk dislikes this name, skipping ***) "
      } elseif [catch {image type $name}] {
         if [catch {
            image create $type $name -file $f
            puts -nonewline "$name "
         } err] {         
            puts -nonewline "(*** $name is bad: $err ***) "
         }
      } else {
         puts -nonewline "($name is duplicate) "
      }
   }
   puts ""
}


#===================================================================
#    LOAD PLUGINS
#===================================================================

proc load_plugins {path} {
   puts ""
   puts "Loading plugins:"

   # load tcl files
   set files [lsort [glob -nocomplain -- [file join $path {*.tcl}]]]
   if {[llength $files] == 0} {
      puts "no .tcl file in $path"
   } else {
      foreach file $files {
         if [catch {source $file} errmsg] {
             puts ""
             puts "error in sourcing $file: $errmsg"
         } else {          
             set name [string tolower [file tail [file rootname $file]]]
             puts -nonewline "$name " 
         }  
      }
      puts ""
   }

   # load dynamic libraries
   set files [lsort [glob -nocomplain -- [file join $path {*.so*}]]]
   if {[llength $files] == 0} {
      puts "no .so file in $path"
   } else {
      foreach file $files {
         if [catch {opp_loadlib $file} errmsg] {
             puts ""
             puts "error in loading shared library $file: $errmsg"
         } else {          
             set name [string tolower [file tail [file rootname $file]]]
             puts -nonewline "$name " 
         }  
      }
      puts ""
   }

}

#===================================================================
#    GENERIC BINDINGS
#===================================================================

proc generic_bindings {} {
   bind Button <Return> {tkButtonInvoke %W}
}

#===================================================================
#    FONT BINDINGS
#===================================================================

proc font_bindings {} {
   global fonts tcl_platform

   #
   # fonts() array elements:
   #  normal:  menus, labels etc
   #  bold:    buttons
   #  icon:    toolbar 'icons'
   #  big:     STOP button
   #  msgname: message name during animation
   #  fixed:   text windows and listboxes
   #

   if {$tcl_platform(platform) == "unix"} {
      set fonts(normal) -Adobe-Helvetica-Medium-R-Normal-*-*-120-*-*-*-*-*-*
      set fonts(bold)   -Adobe-Helvetica-Bold-R-Normal-*-*-120-*-*-*-*-*-*
      set fonts(icon)   -Adobe-Times-Bold-I-Normal-*-*-120-*-*-*-*-*-*
      set fonts(big)    -Adobe-Helvetica-Medium-R-Normal-*-*-180-*-*-*-*-*-*
      set fonts(msgname) -Adobe-Helvetica-Medium-R-Normal-*-*-120-*-*-*-*-*-*
      set fonts(fixed)  fixed
      set fonts(balloon) -Adobe-Helvetica-Medium-R-Normal-*-*-120-*-*-*-*-*-*
   } else {
      # Windows, Mac
      set fonts(normal) -Adobe-Helvetica-Medium-R-Normal-*-*-140-*-*-*-*-*-*
      set fonts(bold)   -Adobe-Helvetica-Bold-R-Normal-*-*-140-*-*-*-*-*-*
      set fonts(icon)   -Adobe-Helvetica-Bold-R-Normal-*-*-140-*-*-*-*-*-*
      set fonts(big)    -Adobe-Helvetica-Medium-R-Normal-*-*-180-*-*-*-*-*-*
      set fonts(msgname) -Adobe-Helvetica-Medium-R-Normal-*-*-140-*-*-*-*-*-*
      set fonts(fixed)  FixedSys
      set fonts(balloon) -Adobe-Helvetica-Medium-R-Normal-*-*-140-*-*-*-*-*-*
   }

   if {$tcl_platform(platform) == "unix"} {
       option add *Scrollbar.width  12
       option add *Menubutton.font  $fonts(normal)
       option add *Menu.font        $fonts(normal)
       option add *Label.font       $fonts(normal)
       option add *Entry.font       $fonts(normal)
       option add *Listbox.font     $fonts(fixed)
       option add *Text.font        $fonts(fixed)
       option add *Button.font      $fonts(bold)
   }
}

proc checkVersion {} {

   global tk_version tk_patchLevel

   catch {package require Tk}
   if {$tk_version<8.0} {
      wm deiconify .
      wm title . "Bad news..."
      frame .f
      pack .f -expand 1 -fill both -padx 2 -pady 2
      label .f.l1 -text "Your version of Tcl/Tk is too old!"
      label .f.l2 -text "Tcl/Tk 8.0 or later required."
      button .f.b -text "OK" -command {exit}
      pack .f.l1 .f.l2 -side top -padx 5
      pack .f.b -side top -pady 5
      focus .f.b
      wm protocol . WM_DELETE_WINDOW {exit}
      tkwait variable ok
   }
   if {[string compare $tk_patchLevel "8.0p1"]<0} {
      tk_messageBox -title {Warning} -type ok -icon warning \
        -message {Old Tcl/Tk version. At least 8.0p1 is strongly recommended!} 
   }
}


proc startup_commands {} {

    set run [opp_getsimoption default-run]
    if {$run>0} {
        opp_newrun $run

        if {[opp_object_systemmodule] != [opp_object_nullpointer]} {
            opp_inspect [opp_object_systemmodule] (default)
            # tk_messageBox -title {OMNeT++} -message {[Run 1] set up.} \
                            -type ok -icon info
        }
    } else {
        new_run
    }

}

