#==========================================================================
#  GNED.TCL -
#            graphical network editor for
#                            OMNeT++
#   By Andras Varga
#==========================================================================

#----------------------------------------------------------------#
#  Copyright (C) 1992-2003 Andras Varga
#
#  This file is distributed WITHOUT ANY WARRANTY. See the file
#  `license' for details on this and other legal matters.
#----------------------------------------------------------------#

#
# intro text
#
puts {GNED 2.3b1 - Graphical Network Editor, part of OMNeT++
(c) 1992-2003 Andras Varga
See the license for distribution terms and warranty disclaimer.

GNED uses human-readable NED as the ONLY file format. It is a fully
two-way tool: you can edit the modules in graphics or in NED source form,
and switch to the other view any time.

See TODO for known bugs and missing features.}

#
# Load library files
#

# OMNETPP_GNED_DIR is set from gned.cc
if [info exist OMNETPP_GNED_DIR] {

   set dir $OMNETPP_GNED_DIR
   source [file join $dir combobox.tcl]
   source [file join $dir datadict.tcl]
   source [file join $dir widgets.tcl]
   source [file join $dir data.tcl]
   source [file join $dir canvas.tcl]
   source [file join $dir drawitem.tcl]
   source [file join $dir plotedit.tcl]
   source [file join $dir canvlbl.tcl]
   source [file join $dir textedit.tcl]
   source [file join $dir findrepl.tcl]
   source [file join $dir drawopts.tcl]
   source [file join $dir fileview.tcl]
   source [file join $dir loadsave.tcl]
   source [file join $dir makened.tcl]
   source [file join $dir genxml.tcl]
   source [file join $dir parsexml.tcl]
   source [file join $dir parsened.tcl]
   source [file join $dir dispstr.tcl]
   source [file join $dir menuproc.tcl]
   source [file join $dir switchvi.tcl]
   source [file join $dir icons.tcl]
   source [file join $dir tree.tcl]
   source [file join $dir treemgr.tcl]
   source [file join $dir dragdrop.tcl]
   source [file join $dir main.tcl]
   source [file join $dir gnedrc.tcl]
   source [file join $dir balloon.tcl]
   source [file join $dir props.tcl]
   source [file join $dir chanprops.tcl]
   source [file join $dir connprops.tcl]
   source [file join $dir imptprops.tcl]
   source [file join $dir modprops.tcl]
   source [file join $dir netwprops.tcl]
   source [file join $dir props.tcl]
   source [file join $dir submprops.tcl]
}

#
# Exec startup code
#
proc start_gned {} {
   global argv config OMNETPP_BITMAP_PATH

   wm withdraw .
   checkTclTkVersion
   setupTkOptions
   init_balloons
   createMainWindow
   loadBitmaps $OMNETPP_BITMAP_PATH
   fileNewNedfile

   if [file readable $config(configfile)] {
       loadConfig $config(configfile)
       set config(connmodeauto) 1  ;# FIXME deliberately change this setting; this line may be removed in the future
   }
   reflectConfigInGUI

   foreach arg $argv {
       # on Windows, we have to expand wildcards manually
       foreach fname [glob -nocomplain $arg] {
           loadNED $fname
       }
   }
}


