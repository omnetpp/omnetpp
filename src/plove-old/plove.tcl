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

if [catch {file readlink $argv0} link] {set link ""}
set plove_dir [file join [file dirname $argv0] [file dirname $link]]
source [file join $plove_dir panes.tcl]
source [file join $plove_dir filter.tcl]
source [file join $plove_dir filtcfg.tcl]
source [file join $plove_dir config.tcl]
source [file join $plove_dir doplot.tcl]
source [file join $plove_dir widgets.tcl]
source [file join $plove_dir help.tcl]
source [file join $plove_dir main.tcl]

startPlove $argv
