#==========================================================================
#  GNED.TCL -
#            graphical network editor for
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

#
# intro text
#
puts {GNED 2.0 Beta 5 - Graphical Network Editor, part of OMNeT++
(c) 1992,99 Andras Varga, TU Budapest
See the license for distribution terms and warranty disclaimer.

GNED uses human-readable NED as the ONLY file format. It is a fully
two-way tool: you can edit the modules in graphics or in NED source form,
and switch to the other view any time.

WARNING! Code under development -- see TODO for known bugs and missing features.}

#
# Load library files
#

# OMNETPP_GNED_DIR is set from gned.cc
if [info exist OMNETPP_GNED_DIR] {

   set dir $OMNETPP_GNED_DIR

   if [catch {
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
     source [file join $dir parsened.tcl]
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

   } errmsg] {
     puts "**** Error: $errmsg"
   }
}

#
# Exec startup code
#
proc start_gned {} {
   global argv config OMNETPP_BITMAP_PATH

   wm withdraw .
   checkVersion
   defaultBindings
   init_balloons
   createMainWindow
   loadBitmaps $OMNETPP_BITMAP_PATH
   fileNewNedfile

   if [file readable $config(configfile)] {
       loadConfig $config(configfile)
   }
   reflectConfigInGUI

   foreach arg $argv {
       # on Windows, we have to expand wildcards manually
       foreach fname [glob -nocomplain $arg] {
           loadNED $fname
       }
   }
}


