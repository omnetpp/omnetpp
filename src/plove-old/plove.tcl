#==========================================================================
#  PLOVE.TCL -
#            part of OMNeT++
#==========================================================================

#----------------------------------------------------------------#
#  Copyright (C) 1992-2003 Andras Varga
#
#  This file is distributed WITHOUT ANY WARRANTY. See the file
#  `license' for details on this and other legal matters.
#----------------------------------------------------------------#

puts {Plove 2.3 - Output vector plotting tool (Gnuplot front-end), part of OMNeT++.
(c) 1992-2003 Andras Varga
See the license for distribution terms and warranty disclaimer.
}

# OMNETPP_PLOVE_DIR is set from plove.cc or plove
if [info exist OMNETPP_PLOVE_DIR] {

   set dir $OMNETPP_PLOVE_DIR

   source [file join $dir icons.tcl]
   source [file join $dir combobox.tcl]
   source [file join $dir panes.tcl]
   source [file join $dir filter.tcl]
   source [file join $dir filtcfg.tcl]
   source [file join $dir config.tcl]
   source [file join $dir doplot.tcl]
   source [file join $dir widgets.tcl]
   source [file join $dir help.tcl]
   source [file join $dir main.tcl]
   source [file join $dir balloon.tcl]
   set OMNETPP_PLOVE_README [file join $OMNETPP_PLOVE_DIR README]
} else {
   set OMNETPP_PLOVE_DIR [file dirname $argv0]
   set OMNETPP_PLOVE_README [file join $OMNETPP_PLOVE_DIR .. doc Readme-Plove.txt]
}


