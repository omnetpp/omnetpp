#==========================================================================
#  CANVTEXT.TCL -
#            part of the GNED, the Tcl/Tk graphical topology editor of
#                            OMNeT++
#
#   Contains: text editing on canvas
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


# editCanvasText --
#
# Start editing a text item on a canvas. Uses an entry widget.
#  c    canvas
#  id   tag or id of text item
#  var  name of a global var to write the edit result into
#
proc editCanvasText {c id var} {
   global tmp
   set xy [$c coords $id]
   set txt  [$c itemcget $id -text]
   set font [$c itemcget $id -font]
   set anch [$c itemcget $id -anchor]
   set tmp(canvasentry_orig) $txt
   $c itemconfig $id -text ""
   catch {destroy $c.e}
   entry $c.e -bd 1 -relief solid -font $font
   $c.e insert end $txt
   set eid [$c create window [lindex $xy 0] [expr [lindex $xy 1]-2] -window $c.e -anchor $anch]
   resizeCanvasEntry $c.e
   bind $c.e <FocusOut> "commitCanvasEntry $c $id $c.e $eid $var"
   bind $c.e <Return> "commitCanvasEntry $c $id $c.e $eid $var"
   bind $c.e <Escape> "cancelCanvasEntry $c $id $c.e $eid"
   bind $c.e <KeyPress> "after idle \"resizeCanvasEntry $c.e\""
   focus $c.e
}


# resizeCanvasEntry --
#
# Private proc for editCanvasText
#
proc resizeCanvasEntry {w} {
   set txt [$w get]
   $w config -width [expr [string length $txt]+2]
}


# commitCanvasEntry --
#
# Private proc for editCanvasText
#
proc commitCanvasEntry {c id e eid var} {
   global $var
   set txt [$e get]
   $c itemconfig $id -text $txt
   $c delete $eid
   destroy $e
   set $var $txt
}


# cancelCanvasEntry --
#
# Private proc for editCanvasText
#
proc cancelCanvasEntry {c id e eid} {
   global tmp
   $c itemconfig $id -text $tmp(canvasentry_orig)
   $c delete $eid
   destroy $e
}

