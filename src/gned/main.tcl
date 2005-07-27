#=================================================================
#  MAIN.TCL - part of
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
#    GLOBAL VARIABLES
#===================================================================

# default config settings
set config(configfile) "~/.gnedrc"
set config(snaptogrid) 1
set config(connmodeauto) 1
set config(autoextend) 1
set config(autosize) 0
set config(noportcheck) 0
set config(autoimport) 1
set config(autocheck) 0
set config(importpath) {}
set config(editor-findstring)     ""
set config(editor-replacestring)  ""
set config(editor-case-sensitive) 0
set config(editor-whole-words)    0
set config(editor-regexp)         0
set config(editor-backwards)      0
set config(default-dir)           "."
set config(default-impdir)        "."
set config(iconprefix)            ""
set config(iconsize)              "normal"

# widgets (these variables will be filled in later)
set gned(canvas) ""
set gned(tab) ""
set gned(toolbar) ""
set gned(horiz-toolbar) ""
set gned(statusbar) ""
set gned(manager) ""
set gned(history) {}
set gned(fwd-history) {}

# state variables
set gned(canvas_id) ""
set gned(canvas_lastid) 0
set gned(editmode) "draw"
set gned(icons) {}
set gned(iconprefixes) {}
set gned(iconsizes) {}

# canvas(1,canvas)     canvas widget
# canvas(1,textedit)   textedit widget
# canvas(1,tab)        tab widget
# canvas(1,mode)       current mode. values: "canvas" or "textedit"
# canvas(1,module-key) key value in the ned() array

# For the ned() array, see datadict.tcl
# ned($key,$field)


#
# simple debug-print procedure
#
proc debug {str} {
   # uncomment the next line if you want debug messages
   #puts "dbg: $str"
}

#===================================================================
#    MAIN GNED WINDOW
#===================================================================

proc createMainWindow {} {

    global gned fonts icons tcl_version help_tips config

    # Set up toplevel.
    # Note: do not deiconify yet -- this will be done in startGNED

    # toplevel . -class Toplevel
    wm focusmodel . passive
    wm geometry . 780x580
    wm minsize . 1 1
    wm overrideredirect . 0
    wm resizable . 1 1
    wm title . "OMNeT++/GNED"
    wm protocol . WM_DELETE_WINDOW {fileExit}

    #################################
    # Menu bar
    #################################
    menu .menubar
    . config -menu .menubar

    # Create menus
    foreach i {
       {filemenu     -label File -underline 0}
       {editmenu     -label Edit -underline 0}
       {viewmenu     -label View -underline 0}
       {drawmenu     -label Draw -underline 0}
       {optionsmenu  -label Options -underline 0}
       {helpmenu     -label Help -underline 0}
    } {
       eval .menubar add cascade -menu .menubar.$i
       menu ".menubar.[lindex $i 0]" -tearoff 0
    }

    # File menu
    foreach i {
      {command -command fileNewNedfile   -label {New NED file} -underline 0}
      {cascade -menu .menubar.filemenu.newmenu -label {New component} -underline 1}
      {separator}
      {command -command fileOpen         -label {Open...} -underline 0}
      {command -command fileSave         -label {Save} -underline 0}
      {command -command fileSaveAs       -label {Save As...} -underline 5}
      {command -command fileSaveAll      -label {Save All} -underline 6}
      {separator}
      {command -command fileCloseNedfile -label {Close NED file} -underline 0}
      {command -command fileCloseCanvas  -label {Close component} -underline 4}
      {separator}
      {command -command fileExportEPS    -label {Export EPS...} -underline 2}
      {separator}
      {command -command fileImportXML    -label {Import XML...} -underline 2}
      {command -command fileExportXML    -label {Export XML...} -underline 2}
      {separator}
      {command -command fileExit -label Exit -underline 1}
    } {
      eval .menubar.filemenu add $i
    }

    # File menu, New component... submenu
    menu .menubar.filemenu.newmenu -tearoff 0
    foreach i {
      {command -command {fileNewComponent imports} -label {Import} -underline 0}
      {command -command {fileNewComponent channel} -label {Channel} -underline 0}
      {command -command {fileNewComponent simple}  -label {Simple module}  -underline 0}
      {command -command {fileNewComponent module}  -label {Compound module}  -underline 0}
      {command -command {fileNewComponent network} -label {Network} -underline 0}
    } {
       eval .menubar.filemenu.newmenu add $i
    }

    # Edit menu
    foreach i {
      {command -command editUndo   -label {Undo} -accel {Ctrl-Z}  -underline 0}
      {command -command editRedo   -label {Redo} -accel {Ctrl-Y}  -underline 0}
      {separator}
      {command -command editCut    -label {Cut} -accel {Ctrl-X} -underline 2}
      {command -command editCopy   -label {Copy} -accel {Ctrl-C}  -underline 0}
      {command -command editPaste  -label {Paste} -accel {Ctrl-V}  -underline 0}
      {separator}
      {command -command editFind    -label {Find...} -accel {Ctrl-F} -underline 0}
      {command -command editFindNext -label {Find next} -accel {Ctrl-N,F3} -underline 5}
      {command -command editReplace -label {Find/Replace...} -accel {Ctrl-H} -underline 5}
      {separator}
      {command -command editDelete -label {Delete}  -accel {Delete} -underline 0}
      {separator}
      {command -command editCheck    -label {Check consistency} -underline 0}
    } {
      eval .menubar.editmenu add $i
    }

    # View menu
    foreach i {
      {command -command switchToGraphics -label {As Graphics} -underline 3}
      {command -command switchToNED      -label {As NED source} -underline 3}
    } {
      eval .menubar.viewmenu add $i
    }

    # Draw menu
    foreach i {
      {command -command {drawBindings $gned(canvas)} -label {Submodule/connection drawing mode}}
      {command -command {selectOrMoveBindings $gned(canvas)} -label {Select/resize mode}}
      {separator}
      {command -command {resetModuleBounds}  -label {Fit compound module to contents} -underline 1}
      {separator}
      {command -command {propertiesSelected $gned(canvas)} -label {Properties of selected item...}}
      {command -command {drawOptionsSelected $gned(canvas)} -label {Appearance of selected item...}}
    } {
      eval .menubar.drawmenu add $i
    }

    # Options menu
    foreach i {
      {check -command {toggleGrid 0} -variable config(snaptogrid) -label {Snap to grid} -underline 0}
      {check -variable config(connmodeauto) -label {Default conn. drawing mode is auto} -underline 0}
      {check -variable config(autoimport) -label {Open imported files automatically} -underline 0}
      {check -variable config(autoextend) -label {Autoextend vector gates using the ++ notation} -underline 0}
      {check -command {toggleAutosize $config(autosize)} -variable config(autosize) -label {Auto-calculate gate size} -underline 0}
      {check -command {togglePortcheck $config(noportcheck)} -variable config(noportcheck) -label {Set "nocheck" for new modules} -underline 0}
      {separator}
      {command -command editImportPath -label {Edit import path...} -underline 0}
      {separator}
      {command -command optionsViewFile -label {View/edit text file...} -underline 0}
    } {
      eval .menubar.optionsmenu add $i
    }

    # Help menu
    foreach i {
      {command -command helpAbout -label {About GNED...} -underline 0}
    } {
      eval .menubar.helpmenu add $i
    }


    #################################
    # Create horiz. toolbar
    #################################

    set gned(horiz-toolbar) .toolbar
    frame $gned(horiz-toolbar) -relief raised -borderwidth 1
    #  {check   -image $icons(check) -command {editCheck}}
    #  {sep5    -separator}
    foreach i {
      {sep0    -separator}
      {new     -image $icons(new)   -command {fileNewNedfile}}
      {open    -image $icons(open)  -command {fileOpen}}
      {save    -image $icons(save)  -command {fileSave}}
      {sep2    -separator}
      {newcomp -image $icons(newmod) -command {newComponentMenuPopup newcomp}}
      {sep6    -separator}
      {back    -image $icons(back)  -command {goBack}}
      {forward -image $icons(forward) -command {goForward}}
      {sep4    -separator}
      {sep5    -separator}
    } {
      set b [eval iconbutton $gned(horiz-toolbar).$i]
      pack $b -anchor n -expand 0 -fill none -side left -padx 0 -pady 2
    }

    # close button
    set b [button $gned(horiz-toolbar).close -image $icons(close) -command fileCloseCanvas -relief flat]
    pack $b -anchor n -expand 0 -fill none -side right -padx 0 -pady 2

    set help_tips($gned(horiz-toolbar).new)     {New NED file}
    set help_tips($gned(horiz-toolbar).open)    {Open NED file}
    set help_tips($gned(horiz-toolbar).save)    {Save NED file of current canvas}
    #set help_tips($gned(horiz-toolbar).check) {Check module consistency}
    set help_tips($gned(horiz-toolbar).newcomp) {Add new component to current NED file}
    set help_tips($gned(horiz-toolbar).back)    {Back to previous view}
    set help_tips($gned(horiz-toolbar).forward) {Forward to next view}
    set help_tips($gned(horiz-toolbar).close)   {Close current canvas}


    #################################
    # Create main display area
    #################################
    frame .main


    #################################
    # Create manager (vert. tree view)
    #################################
    frame .main.mgr -relief flat -borderwidth 1

    vertResizeBar .main.mgr.resize .main.mgr.tree

    canvas .main.mgr.tree -width 140 -bg #ffffe0 -relief groove \
                            -yscrollcommand ".main.mgr.sb set"
    scrollbar .main.mgr.sb -command ".main.mgr.tree yview"
    pack .main.mgr.resize -side right -fill y
    pack .main.mgr.sb -side right -fill y
    pack .main.mgr.tree -side left -fill y -padx 0 -pady 0 -ipadx 0 -ipady 0

    set gned(manager) .main.mgr
    initTreeManager


    #################################
    # Create graphics toolbar
    #################################
    frame .toolbar.graphics
    pack .toolbar.graphics -expand 1 -fill both
    set gned(graphics-toolbar) .toolbar.graphics

    foreach i {
      {sep0     -separator}
      {select   -image $icons(select)   -command {selectOrMoveBindings $gned(canvas)}}
      {draw     -image $icons(draw)     -command {drawBindings $gned(canvas)}}
      {delete   -image $icons(del)      -command {deleteSelected}}
      {sep1     -separator}
      {opts     -image $icons(drawopts) -command {drawOptionsSelected $gned(canvas)}}
      {props    -image $icons(props)    -command {propertiesSelected $gned(canvas)}}
      {sep2     -separator}
      {grid     -image $icons(grid)     -command {toggleGrid 1}}
      {bounds   -image $icons(bounds)   -command {resetModuleBounds}}
    } {
      set b [eval iconbutton $gned(graphics-toolbar).$i]
      #pack $b -anchor w -expand 0 -fill x -side top -padx 2 -pady 0
      pack $b -anchor w -expand 0 -fill y -side left -padx 0 -pady 2
    }

    set help_tips($gned(graphics-toolbar).draw)   {Draw submodules and connections}
    set help_tips($gned(graphics-toolbar).select) {Select, move or resize items}
    set help_tips($gned(graphics-toolbar).delete) {Delete selected items}
    set help_tips($gned(graphics-toolbar).grid)   {Snap to grid on/off}
    set help_tips($gned(graphics-toolbar).bounds) {Fit compound module area to contents}
    set help_tips($gned(graphics-toolbar).props)  {Properties of selected item}
    set help_tips($gned(graphics-toolbar).opts)   {Appearance of selected item}


    #################################
    # Create textedit toolbar
    #################################
    frame .toolbar.textedit
    #pack .toolbar.textedit -expand 1 -fill both
    set gned(textedit-toolbar) .toolbar.textedit

    foreach i {
      {sep1    -separator}
      {cut     -image $icons(cut)   -command {editCut}}
      {copy    -image $icons(copy)  -command {editCopy}}
      {paste   -image $icons(paste) -command {editPaste}}
      {sep3    -separator}
      {undo    -image $icons(undo)  -command {editUndo}}
      {redo    -image $icons(redo)  -command {editRedo}}
      {sep0    -separator}
      {find    -image $icons(find)  -command {editFind}}
      {repl    -image $icons(findrepl) -command {editReplace}}
    } {
      set b [eval iconbutton $gned(textedit-toolbar).$i]
      #pack $b -anchor w -expand 0 -fill x -side top -padx 2 -pady 0
      pack $b -anchor w -expand 0 -fill y -side left -padx 0 -pady 2
    }

    set help_tips($gned(textedit-toolbar).cut)   {Cut to clipboard (ctrl-X)}
    set help_tips($gned(textedit-toolbar).copy)  {Copy to clipboard (ctrl-C)}
    set help_tips($gned(textedit-toolbar).paste) {Paste from clipbard (ctrl-V)}
    set help_tips($gned(textedit-toolbar).undo)  {Undo editing (ctrl-Z)}
    set help_tips($gned(textedit-toolbar).redo)  {Redo editing (ctrl-Y)}
    set help_tips($gned(textedit-toolbar).find)  {Find text (ctrl-F)}
    set help_tips($gned(textedit-toolbar).repl)  {Replace text (ctrl-H)}

    # check if undo works
    set gned(supports-undo) 1
    if [catch {text .tmp -undo true; destroy .tmp}] {
        set gned(supports-undo) 0
        set help_tips($gned(textedit-toolbar).undo)  {This version of Tk doesn't support Undo/Redo}
        set help_tips($gned(textedit-toolbar).redo)  {This version of Tk doesn't support Undo/Redo}
        puts "NOTE: This version of Tcl/Tk doesn't support Undo in text widgets yet."
    }


    #################################
    # Create canvas & scrollbars
    #################################
    frame .main.f


    #################################
    # Create tabs and switching buttons
    #################################
    frame .main.f.tabs
    pack .main.f.tabs -expand 0 -fill x -side bottom -ipadx 0 -ipady 0 -padx 0 -pady 0

    set tabs .main.f.tabs
    canvas $tabs.c -height 15
    frame $tabs.c.f
    $tabs.c create window 0 0 -anchor nw -window $tabs.c.f

    frame .main.f.switcher
    pack .main.f.switcher -expand 0 -fill x -side top -ipadx 0 -ipady 0 -padx 0 -pady 0
    set gned(switchtographics-button) .main.f.switcher.graphics
    set gned(switchtotextedit-button) .main.f.switcher.textedit
    button $gned(switchtographics-button) -text {Graphics} -command {switchToGraphics} \
           -relief flat -highlightthickness 0 -pady 1
    button $gned(switchtotextedit-button) -text {NED source} -command {switchToNED} \
           -relief flat -highlightthickness 0 -pady 1
    pack $gned(switchtographics-button) $gned(switchtotextedit-button) -expand 0 -fill none -side left

    set help_tips($gned(switchtographics-button)) {Edit in graphics view}
    set help_tips($gned(switchtotextedit-button)) {Edit in NED source view}


    # create scrollbar to scroll buttons if there are too many windows open
    if {0} {
        # scrollbar version
        $tabs.c config -xscrollcommand "$tabs.s set"
        scrollbar $tabs.s -command "$tabs.c xview" -orient horiz
        pack $tabs.s -anchor center -expand 0 -fill none -side right
    } else {
        # use "<" ">" buttons instead of scrollbar
        button $tabs.right -highlightthickness 0 -height 10 -width 10 \
            -image $icons(scroll_r) -command "$tabs.c xview scroll 1 units"
        button $tabs.left -highlightthickness 0 -height 10 -width 10 \
            -image $icons(scroll_l) -command "$tabs.c xview scroll -1 units"
        pack $tabs.right -anchor c -expand 0 -fill none -side right
        pack $tabs.left -anchor c -expand 0 -fill none -side right
    }

    # the canvas has to be packed last, to make it shrink sooner than the buttons/scrollbar
    pack $tabs.c -side left -expand yes -fill both


    #################################
    # Create main area and scrollbars
    #################################
    set ww .main.f

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
    pack .main.mgr     -expand 0 -fill y    -side left
    #pack .main.toolbar -expand 0 -fill y    -side left
    pack .main.f       -expand 1 -fill both -side right

    #################################
    # Create status bar
    #################################
    frame .statusbar -relief sunken -border 1
    label .statusbar.mode -justify left -text {(mode)}

    pack .statusbar.mode -anchor n -expand 0 -side left

    set gned(statusbar) .statusbar

    #################################
    # Pack main and status bar
    #################################
    pack .statusbar -fill x -side bottom -padx 2 -pady 2
    pack .toolbar   -side top -fill x
    pack .main      -expand 1 -fill both -side top
}


set bitmap_ctr 0

proc loadBitmaps {path} {
   global tcl_platform gned bitmaps bitmap_ctr

   # On Windows, we use ";" to separate directories in $path. Using ":" (the
   # Unix separator) would cause trouble with dirs containing drive letter
   # (like "c:\bitmaps"). Using a space is also not an option (think of
   # "C:\Program Files\...").

   if {$tcl_platform(platform) == "unix"} {
       set sep {:;}
   } else {
       set sep {;}
   }

   foreach dir [split $path $sep] {
       if {$dir!=""} {
           puts -nonewline "Loading bitmaps from $dir: "
           do_load_bitmaps $dir ""
           puts ""
       }
   }

   set gned(icons) [lsort $gned(icons)]
   set gned(iconprefixes) [lsort $gned(iconprefixes)]
   set gned(iconsizes) [lsort $gned(iconsizes)]

   if {$bitmap_ctr==0} {
       puts "*** no bitmaps (gif) in $path"
   }

   puts ""
}

proc do_load_bitmaps {dir prefix} {
   global bitmaps bitmap_ctr

   #debug "entering $dir"
   set files [concat [glob -nocomplain -- [file join $dir {*.gif}]] \
                     [glob -nocomplain -- [file join $dir {*.png}]]]

   # load bitmaps from this directory
   set n 0
   foreach f $files {
      set name [string tolower [file tail [file rootname $f]]]
      set img "i[incr bitmap_ctr]$name"
      if [catch {
         image create photo $img -file $f
         set size "n" ;#default
         regexp -- {^(.*)_(vl|xl|l|n|s|vs|xs)$} $name dummy name size
         do_add_bitmap $img $prefix $name $size
         incr n
      } err] {
         puts -nonewline "(*** cannot load $f: $err ***) "
      }
   }
   puts -nonewline "$prefix*: $n  "

   # recurse into subdirs
   foreach f [glob -nocomplain -- [file join $dir {*}]] {
      if {[file isdirectory $f] && [file tail $f]!="CVS"} {
         do_load_bitmaps "$f" "$prefix[file tail $f]/"
      }
   }
}

# register loaded image
proc do_add_bitmap {img prefix name size} {
   global bitmaps gned

   # access via the s= display string option
   set bitmaps($prefix$name,$size) $img

   # access by the legacy way
   if {$size=="n"} {
       set bitmaps($prefix$name) $img
       lappend gned(icons) "$prefix$name"
   } else {
       set bitmaps($prefix${name}_$size) $img
       lappend gned(icons) "$prefix${name}_$size"
   }

   if {[lsearch $gned(iconprefixes) $prefix]==-1} {
       lappend gned(iconprefixes) $prefix
   }
   if {[lsearch $gned(iconsizes) $size]==-1} {
       lappend gned(iconsizes) $size
   }
}


proc busy {{msg {}}} {
    global gned

    if {$msg != ""} {
        $gned(statusbar).mode config -text $msg
        update idletasks
        . config -cursor watch
    } else {
        $gned(statusbar).mode config -text "Ready"
        update idletasks
        . config -cursor ""
    }
}

#----------------
set hints(insert-imports) {Imports contain the list of other NED files, from which\
you use modules or channels in this NED file. You can import NED files from other\
directories as well -- if you use nedtool you'll be able to specify NED import\
directories via the -I command-line option.

If you want to edit these imports later, you'll find them in the object tree on the left.}
#----------------
set hints(insert-channel) {You are creating a new channel type,\
which you can use later as a prototype for creating connections.

If you want to edit this channel later, you'll find it in the object tree on the left.}
#----------------
set hints(insert-simple)  {You are creating a new simple module type,\
which you can use as a building block for compound modules. Simple modules are\
active components -- to define the run-time behavior, this simple module\
NED definition will need to be acompanied by a C++ class of the same name.

If you want to edit this simple module later, you'll find it in the object tree on the left.\
You can also drag it on the canvas to create an instance.}
#----------------
set hints(insert-module)  {You are creating a new compound module type, which can
use existing module types as building blocks.

You can edit it graphically or in NED form in the main area,\
and you can also find it in the in the object tree on the left, where you
can edit its parameters and gates. You can also drag it into another compound module\
on the canvas to create an instance.}
#----------------
set hints(insert-network)  {You are creating a new NED "network". Networks are used\
to define simulations that can be executed in themselves. Networks are\
defined as an instance of a module type (e.g. of a compound module type named\
MyQueueingNetwork). You can create several networks for the same module,\
all with different parameter settings.

For further editing, you'll find this "network" component in the object tree on the left.}
#----------------
set hints(resetModuleBounds)  {This will make the compound module's area \
(that is, the background rectangle) always auto-adjust to the submodules in it. \
To turn off this behavior, drag the right or bottom edge of the background rectangle\
to the desired size.}
#----------------
set hints(compoundAutoResized)  {The compound module's area (that is,\
the background rectangle) is now always auto-adjusting to the bounding box\
of the submodules you're drawing in it. To turn off this behavior, drag the right\
or bottom edge of the background rectangle to the desired size.\
There is a toolbar button for turning it on again.}
#----------------
set hints(nocheckSet)  {Selecting the "nocheck" option causes the "nocheck" keyword to be appended to the \
"connections" section in the NED file. This means that the simulator will not check for \
unconnected gates in this compound module when the simulation is run. Simple modules should \
take care not to send messages via output gates that are not connected.}
#----------------
set hints(autosizeSet)  {When this option is set, Gned will automatically generate \
re-calculated "gatesizes" entries for all input and output gates every time you change \
the gate index of a connection. If you need full control over those entries, this \
option should not be checked!}

