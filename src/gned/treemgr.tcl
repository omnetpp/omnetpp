#==========================================================================
#  TREEMGR.TCL -
#            part of the GNED, the Tcl/Tk graphical topology editor of
#                            OMNeT++
#
#   By Andras Varga
#
#==========================================================================

#----------------------------------------------------------------#
#  Copyright (C) 1992,99 Andras Varga
#  Technical University of Budapest, Dept. of Telecommunications,
#  Stoczek u.2, H-1111 Budapest, Hungary.
#
#  This file is distributed WITHOUT ANY WARRANTY. See the file
#  `license' for details on this and other legal matters.
#----------------------------------------------------------------#


# updateTreeManager --
#
# Redraws the manager window (left side of main window).
#
proc updateTreeManager {} {
    global gned

    Tree:build $gned(manager).tree
}


#-------------- temp solution: -----------------

# This user-supplied function gets called by the tree widget to get info about
# tree nodes. The widget itself only stores the state (open/closed) of the
# nodes, everything else comes from this function.
#
proc getNodeInfo {w op {nodeid {}}} {

  global ned

  switch $op {
    root {
      return 1
    }
    text {
      return "Node-$nodeid"
    }
    icon {
      if {[expr $nodeid<32]} {
        return "idir"
      } else {
        return "ifile"
      }
    }
    parent {
      if {$nodeid=="1"} {
        return ""
      } else {
        return [expr int($nodeid/2)]
      }
    }
    children {
      return [list [expr 2*$nodeid] [expr 2*$nodeid+1]]
    }
    haschildren {
      if {[expr $nodeid<32]} {
        return 1
      } else {
        return 0
      }
    }
  }
}


image create photo idir -data {
    R0lGODdhEAAQAPIAAAAAAHh4eLi4uPj4APj4+P///wAAAAAAACwAAAAAEAAQAAADPVi63P4w
    LkKCtTTnUsXwQqBtAfh910UU4ugGAEucpgnLNY3Gop7folwNOBOeiEYQ0acDpp6pGAFArVqt
    hQQAO///
}
image create photo ifile -data {
    R0lGODdhEAAQAPIAAAAAAHh4eLi4uPj4+P///wAAAAAAAAAAACwAAAAAEAAQAAADPkixzPOD
    yADrWE8qC8WN0+BZAmBq1GMOqwigXFXCrGk/cxjjr27fLtout6n9eMIYMTXsFZsogXRKJf6u
    P0kCADv/
}
