#==========================================================================
#  SCALARS.TCL -
#            part of OMNeT++
#==========================================================================

#----------------------------------------------------------------#
#  Copyright (C) 1992-2003 Andras Varga
#
#  This file is distributed WITHOUT ANY WARRANTY. See the file
#  `license' for details on this and other legal matters.
#----------------------------------------------------------------#

puts {Scalars -- Experimental output scalar plotting tool.
(c) 2004 Andras Varga
See the license for distribution terms and warranty disclaimer.
}

# OMNETPP_SCALARS_DIR is set from scalars.cc or scalars
if [info exist OMNETPP_SCALARS_DIR] {

   set dir $OMNETPP_SCALARS_DIR

   source [file join $dir icons.tcl]
   source [file join $dir combobox.tcl]
   source [file join $dir widgets.tcl]
   source [file join $dir help.tcl]
   source [file join $dir scalartable.tcl]
   source [file join $dir main.tcl]
   source [file join $dir balloon.tcl]
   source [file join $dir menuproc.tcl]
   source [file join $dir charts.tcl]
   source [file join $dir dialogs.tcl]
   source [file join $dir bltgraph.tcl]
   set OMNETPP_SCALARS_README [file join $OMNETPP_SCALARS_DIR README]
} else {
   set OMNETPP_SCALARS_DIR [file dirname $argv0]
   set OMNETPP_SCALARS_README [file join $OMNETPP_SCALARS_DIR .. doc Readme-Scalars.txt]
}

set HAVE_BLT 1
if [catch {package require BLT} err] {
    puts "Cannot load BLT, exiting."
    exit
}

