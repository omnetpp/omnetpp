#==========================================================================
#  MAIN.TCL -
#            part of the GNED, the Tcl/Tk graphical topology editor of
#                            OMNeT++
#   By Andras Varga
#==========================================================================

#----------------------------------------------------------------#
#  Copyright (C) 1992,99 Andras Varga
#  Technical University of Budapest, Dept. of Telecommunications,
#  Stoczek u.2, H-1111 Budapest, Hungary.
#
#  This file is distributed WITHOUT ANY WARRANTY. See the file
#  `license' for details on this and other legal matters.
#----------------------------------------------------------------#

#===================================================================
#    GLOBAL VARIABLES
#===================================================================

set gned(canvas) ""
set gned(tab) ""
set gned(toolbar) ""
set gned(horiz-toolbar) ""
set gned(statusbar) ""
set gned(manager) ""

set gned(canvas_id) ""
set gned(canvas_lastid) 0
set gned(snaptogrid) 1
set gned(connmodeauto) 0
set gned(autocheck) 0
set gned(editmode) "draw"
set gned(icons) {}

# canvas(1,canvas)     canvas widget
# canvas(1,textedit)   textedit widget
# canvas(1,tab)        tab widget
# canvas(1,mode)       current mode. values: "canvas" or "textedit"
# canvas(1,module-key) key value in the ned() array

# For the ned() array, see datadict.tcl
# ned($key,$field)

#===================================================================
#    MAIN OMNET++ WINDOW
#===================================================================

proc create_omnetpp_window {} {
    set w .omnetpp
    if {[winfo exists $w]} {
        wm deiconify $w; return
    }

    global gned fonts icons tcl_version help_tips

    toplevel $w -class Toplevel
    wm focusmodel $w passive
    wm geometry $w 640x450

    wm maxsize $w 1009 738
    wm minsize $w 1 1
    wm overrideredirect $w 0
    wm resizable $w 1 1
    wm deiconify $w
    wm title $w "OMNeT++/GNED"
    wm protocol $w WM_DELETE_WINDOW {fileExit}

    #################################
    # Menu bar
    #################################
    menu $w.menubar
    $w config -menu $w.menubar

    # Create menus
    foreach i {
       {filemenu     -label File -underline 0}
       {editmenu     -label Edit -underline 0}
       {viewmenu     -label View -underline 0}
       {drawmenu     -label Draw -underline 0}
       {optionsmenu  -label Options -underline 0}
       {helpmenu     -label Help -underline 0}
    } {
       eval $w.menubar add cascade -menu $w.menubar.$i
       menu "$w.menubar.[lindex $i 0]" -tearoff 0
    }

    # File menu
    foreach i {
      {command -command fileNewNedfile   -label {New NED file} -underline 0}
      {cascade -menu $w.menubar.filemenu.newmenu -label {New component} -underline 1}
      {separator}
      {command -command fileOpen         -label {Open...} -underline 0}
      {command -command fileSave         -label {Save} -underline 0}
      {command -command fileSaveAs       -label {Save As...} -underline 5}
      {separator}
      {command -command fileCloseNedfile -label {Close NED file} -underline 0}
      {command -command fileCloseCanvas  -label {Close component} -underline 1}
      {separator}
      {command -command fileExit -label Exit -underline 1}
    } {
      eval $w.menubar.filemenu add $i
    }

    # File menu, New component... submenu
    menu $w.menubar.filemenu.newmenu -tearoff 0
    foreach i {
      {command -command {fileNewComponent imports} -label {imports} -underline 0}
      {command -command {fileNewComponent channel} -label {channel} -underline 0}
      {command -command {fileNewComponent simple}  -label {simple}  -underline 0}
      {command -command {fileNewComponent module}  -label {module}  -underline 0}
      {command -command {fileNewComponent network} -label {network} -underline 0}
    } {
       eval $w.menubar.filemenu.newmenu add $i
    }

    # Edit menu
    foreach i {
      {command -command editCut    -label {Cut} -accel {Ctrl-X} -underline 2}
      {command -command editCopy   -label {Copy} -accel {Ctrl-C}  -underline 0}
      {command -command editPaste  -label {Paste} -accel {Ctrl-V}  -underline 0}
      {separator}
      {command -command editDelete -label {Delete}  -accel {Delete} -underline 0}
      {separator}
      {command -command editCheck    -label {Check consistency} -underline 0}
    } {
      eval $w.menubar.editmenu add $i
    }

    # View menu
    foreach i {
      {command -command switchToGraphics -label {As Graphics} -underline 3}
      {command -command switchToNED      -label {As NED source} -underline 3}
    } {
      eval $w.menubar.viewmenu add $i
    }

    # Draw menu
    foreach i {
      {command -command {drawBindings $gned(canvas)} -label {Submodule/connection drawing mode}}
      {command -command {selectOrMoveBindings $gned(canvas)} -label {Select/resize mode}}
      {separator}
      {command -command {resetModuleBounds}  -label {Reset parent module bounds} -underline 0}
      {separator}
      {command -command {propertiesSelected $gned(canvas)} -label {Properties of selected item...}}
      {command -command {drawOptionsSelected $gned(canvas)} -label {Appearance of selected item...}}
    } {
      eval $w.menubar.drawmenu add $i
    }

    # Options menu
    foreach i {
      {check -command {toggleGrid 0} -variable gned(snaptogrid) -label {Snap to grid} -underline 0}
      {check -variable gned(connmodeauto) -label {Default conn. drawing mode is auto} -underline 0}
      {check -variable gned(autocheck) -label {Auto check module consistency} -underline 0}
      {separator}
      {command -command optionsViewFile -label {View/edit file...} -underline 0}
    } {
      eval $w.menubar.optionsmenu add $i
    }

    # Help menu
    foreach i {
      {command -command helpAbout -label {About GNED...} -underline 0}
      {separator}
      {command -command {helpRelNotes} -label {Release notes} -underline 0}
    } {
      eval $w.menubar.helpmenu add $i
    }


    #################################
    # Create horiz. toolbar
    #################################

    frame $w.toolbar -relief raised -borderwidth 1
    foreach i {
      {sep0    -separator}
      {new     -image $icons(new)   -command {fileNewComponent module}}
      {open    -image $icons(open)  -command {fileOpen}}
      {save    -image $icons(save)  -command {fileSave}}
      {sep1    -separator}
      {cut     -image $icons(cut)   -command {editCut}}
      {copy    -image $icons(copy)  -command {editCopy}}
      {paste   -image $icons(paste) -command {editPaste}}
      {sep2    -separator}
      {graph   -image $icons(graph) -command {switchToGraphics}}
      {ned     -image $icons(ned)   -command {switchToNED}}
      {sep3    -separator}
      {check   -image $icons(check) -command {editCheck}}
    } {
      set b [eval iconbutton $w.toolbar.$i]
      pack $b -anchor n -expand 0 -fill none -side left -padx 0 -pady 2
    }
    $w.toolbar.graph config -relief sunken

    # close button
    set b [button $w.toolbar.close -image $icons(close) -command fileCloseCanvas -relief flat]
    pack $b -anchor n -expand 0 -fill none -side right -padx 0 -pady 2

    set gned(horiz-toolbar) $w.toolbar

    set help_tips($gned(horiz-toolbar).new)   {Create new module in current file}
    set help_tips($gned(horiz-toolbar).open)  {Open NED file}
    set help_tips($gned(horiz-toolbar).save)  {Save NED file of current canvas}
    set help_tips($gned(horiz-toolbar).cut)   {Cut to clipboard}
    set help_tips($gned(horiz-toolbar).copy)  {Copy to clipboard}
    set help_tips($gned(horiz-toolbar).paste) {Paste from clipbard}
    set help_tips($gned(horiz-toolbar).graph) {Edit in graphics view}
    set help_tips($gned(horiz-toolbar).ned)   {Edit in NED source view}
    set help_tips($gned(horiz-toolbar).check) {Check module consistency}
    set help_tips($gned(horiz-toolbar).close) {Close current canvas}


    #################################
    # Create main display area
    #################################
    frame $w.main


    #################################
    # Create manager (vert. tree view)
    #################################
    frame $w.main.mgr -relief flat -borderwidth 1

    vertResizeBar $w.main.mgr.resize $w.main.mgr.tree

    canvas $w.main.mgr.tree -width 140 -bg #ffffe0 -relief groove \
                            -yscrollcommand "$w.main.mgr.sb set"
    scrollbar $w.main.mgr.sb -command "$w.main.mgr.tree yview"
    pack $w.main.mgr.resize -side right -fill y
    pack $w.main.mgr.sb -side right -fill y
    pack $w.main.mgr.tree -side left -fill y -padx 0 -pady 0 -ipadx 0 -ipady 0

    set gned(manager) $w.main.mgr
    initTreeManager


    #################################
    # Create vert. toolbar
    #################################
    frame $w.main.toolbar -relief raised -borderwidth 1

    set gned(toolbar) $w.main.toolbar

    foreach i {
      {sep0     -separator}
      {draw     -image $icons(draw)     -command {drawBindings $gned(canvas)}}
      {select   -image $icons(select)   -command {selectOrMoveBindings $gned(canvas)}}
      {delete   -image $icons(delete)   -command {deleteSelected}}
      {sep1     -separator}
      {grid     -image $icons(grid)     -command {toggleGrid 1}}
      {bounds   -image $icons(bounds)   -command {resetModuleBounds}}
      {sep2     -separator}
      {props    -image $icons(props)    -command {propertiesSelected $gned(canvas)}}
      {opts     -image $icons(drawopts) -command {drawOptionsSelected $gned(canvas)}}
    } {
      set b [eval iconbutton $gned(toolbar).$i]
      pack $b -anchor w -expand 0 -fill x -side top -padx 2 -pady 0
    }

    if {$gned(snaptogrid)} {
        $gned(toolbar).grid config -relief sunken
    }

    set help_tips($gned(toolbar).draw)   {Switch to submodule/connection drawing mode}
    set help_tips($gned(toolbar).select) {Switch to select/resize mode}
    set help_tips($gned(toolbar).delete) {Delete selected items}
    set help_tips($gned(toolbar).grid)   {Snap to grid on/off}
    set help_tips($gned(toolbar).bounds) {Reset parent module bounds}
    set help_tips($gned(toolbar).props)  {Properties of selected item}
    set help_tips($gned(toolbar).opts)   {Appearance of selected item}


    #################################
    # Create canvas & scrollbars
    #################################
    frame $w.main.f


    #################################
    # Create tabs below
    #################################
    frame $w.main.f.tabs
    pack $w.main.f.tabs -expand 0 -fill x -side bottom -ipadx 0 -ipady 0 -padx 0 -pady 0


    #################################
    # Create main area and scrollbars
    #################################
    set ww $w.main.f

    frame $ww.grid
    scrollbar $ww.hsb -orient horiz
    scrollbar $ww.vsb

    pack $ww.grid -expand yes -fill both -padx 1 -pady 1
    grid rowconfig    $ww.grid 0 -weight 1 -minsize 0
    grid columnconfig $ww.grid 0 -weight 1 -minsize 0

    grid $ww.vsb -in $ww.grid -row 0 -column 1 -rowspan 1 -columnspan 1 -sticky news
    grid $ww.hsb -in $ww.grid -row 1 -column 0 -rowspan 1 -columnspan 1 -sticky news


    set gned(canvas) ""


    #################################
    # Pack the vert.toolbar, the tree manager and the main area
    #################################
    pack $w.main.mgr     -expand 0 -fill y    -side left
    pack $w.main.toolbar -expand 0 -fill y    -side left
    pack $w.main.f       -expand 1 -fill both -side right

    #################################
    # Create status bar
    #################################
    frame $w.statusbar -relief sunken -border 1
    label $w.statusbar.mode -justify left -text {(mode)}

    pack $w.statusbar.mode -anchor n -expand 0 -side left

    set gned(statusbar) $w.statusbar

    #################################
    # Pack main and status bar
    #################################
    pack $w.statusbar -fill x -side bottom -padx 2 -pady 2
    pack $w.toolbar   -side top -fill x
    pack $w.main      -expand 1 -fill both -side top
}

#===================================================================
#    GENERIC BINDINGS
#===================================================================

proc defaultBindings {} {
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
      set fonts(normal)  -Adobe-Helvetica-Medium-R-Normal-*-*-120-*-*-*-*-*-*
      set fonts(bold)    -Adobe-Helvetica-Bold-R-Normal-*-*-120-*-*-*-*-*-*
      set fonts(icon)    -Adobe-Times-Bold-I-Normal-*-*-120-*-*-*-*-*-*
      set fonts(big)     -Adobe-Helvetica-Medium-R-Normal-*-*-180-*-*-*-*-*-*
      set fonts(msgname) -Adobe-Helvetica-Medium-R-Normal-*-*-120-*-*-*-*-*-*
      set fonts(fixed)   fixed
      set fonts(balloon) -Adobe-Helvetica-Medium-R-Normal-*-*-120-*-*-*-*-*-*
   } else {
      # Windows, Mac
      set fonts(normal)  -Adobe-Helvetica-Medium-R-Normal-*-*-140-*-*-*-*-*-*
      set fonts(bold)    -Adobe-Helvetica-Bold-R-Normal-*-*-140-*-*-*-*-*-*
      set fonts(icon)    -Adobe-Helvetica-Bold-R-Normal-*-*-140-*-*-*-*-*-*
      set fonts(big)     -Adobe-Helvetica-Medium-R-Normal-*-*-180-*-*-*-*-*-*
      set fonts(msgname) -Adobe-Helvetica-Medium-R-Normal-*-*-140-*-*-*-*-*-*
      #set fonts(fixed)  -Adobe-Courier-Medium-R-Normal-*-*-160-*-*-*-*-*-*
      set fonts(fixed)   FixedSys
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
       option add *Text.font        $fonts(fixed)
   } else {
       # Windows, Mac
       option add *Button.padX      0
       option add *Button.padY      0
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


proc load_bitmaps {path} {
   global gned tcl_platform

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
            lappend gned(icons) $name
         } err] {
            puts -nonewline "(*** $name is bad: $err ***) "
         }
      } else {
         puts -nonewline "($name is duplicate) "
      }
   }
   puts ""

   set gned(icons) [lsort $gned(icons)]
}

