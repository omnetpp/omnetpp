#==========================================================================
#  PLOVE.TCL -
#            part of OMNeT++
#==========================================================================

#----------------------------------------------------------------#
#  Copyright (C) 1992-2001 Andras Varga
#  Technical University of Budapest, Dept. of Telecommunications,
#  Stoczek u.2, H-1111 Budapest, Hungary.
#
#  This file is distributed WITHOUT ANY WARRANTY. See the file
#  `license' for details on this and other legal matters.
#----------------------------------------------------------------#

puts {Plove - Output vector plotting tool (Gnuplot front-end), part of OMNeT++.
(c) 1992-2001 Andras Varga, TU Budapest
See the license for distribution terms and warranty disclaimer.
}

# OMNETPP_PLOVE_DIR is set from plove.cc or plove
if [info exist OMNETPP_PLOVE_DIR] {

   set dir $OMNETPP_PLOVE_DIR

   source [file join $dir panes.tcl]
   source [file join $dir filter.tcl]
   source [file join $dir filtcfg.tcl]
   source [file join $dir config.tcl]
   source [file join $dir doplot.tcl]
   source [file join $dir widgets.tcl]
   source [file join $dir help.tcl]
   source [file join $dir main.tcl]
}

