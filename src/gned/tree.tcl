#=================================================================
#  TREE.TCL - part of
#
#                     OMNeT++/OMNEST
#            Discrete System Simulation in C++
#
#   Loosely based on:
#      tree.tcl, Copyright (C) 1997,1998 D. Richard Hipp
#      Author contact information:
#        drh@acm.org
#        http://www.hwaci.com/drh/
#      tree.tcl falls under the GNU Library General Public License
#
#=================================================================

#----------------------------------------------------------------#
#  Copyright (C) 1992-2005 Andras Varga
#
#  This file is distributed WITHOUT ANY WARRANTY. See the file
#  `license' for details on this and other legal matters.
#----------------------------------------------------------------#


#
# Bitmaps used to show which parts of the tree can be opened.
#
set maskdata "#define solid_width 9\n#define solid_height 9"
append maskdata {
  static unsigned char solid_bits[] = {
   0xff, 0x01, 0xff, 0x01, 0xff, 0x01, 0xff, 0x01, 0xff, 0x01, 0xff, 0x01,
   0xff, 0x01, 0xff, 0x01, 0xff, 0x01
  };
}
set data "#define open_width 9\n#define open_height 9"
append data {
  static unsigned char open_bits[] = {
   0xff, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x7d, 0x01, 0x01, 0x01,
   0x01, 0x01, 0x01, 0x01, 0xff, 0x01
  };
}
image create bitmap Tree:openbm -data $data -maskdata $maskdata \
  -foreground black -background white
set data "#define closed_width 9\n#define closed_height 9"
append data {
  static unsigned char closed_bits[] = {
   0xff, 0x01, 0x01, 0x01, 0x11, 0x01, 0x11, 0x01, 0x7d, 0x01, 0x11, 0x01,
   0x11, 0x01, 0x01, 0x01, 0xff, 0x01
  };
}
image create bitmap Tree:closedbm -data $data -maskdata $maskdata \
  -foreground black -background white


#
# Initialize a new tree widget. $w must be a previously created Tk canvas.
#
proc Tree:init {w} {
  global Tree
  set v [getNodeInfo $w root]
  set Tree($w:$v:open) 0
  set Tree($w:selection) {}
  set Tree($w:selidx) {}
  Tree:buildwhenidle $w
}

#
# Change the selection to the indicated item
#
proc Tree:setselection {w v} {
  global Tree
  set Tree($w:selection) $v
  Tree:drawselection $w
}

#
# Retrieve the current selection
#
proc Tree:getselection w {
  global Tree
  return $Tree($w:selection)
}

#
# Open a branch of a tree
#
proc Tree:open {w v} {
  global Tree
  if {[info exists Tree($w:$v:open)] && $Tree($w:$v:open)==0 && [getNodeInfo $w haschildren $v]} {
    set Tree($w:$v:open) 1
    Tree:build $w
  }
}

#
# Close a branch of a tree
#
proc Tree:close {w v} {
  global Tree
  if {[info exists Tree($w:$v:open)] && $Tree($w:$v:open)==1} {
    set Tree($w:$v:open) 0
    Tree:build $w
  }
}

#
# Toggle a branch of a tree
#
proc Tree:toggle {w v} {
  global Tree
  if {[info exists Tree($w:$v:open)] && $Tree($w:$v:open)==1} {
    set Tree($w:$v:open) 0
  } else {
    set Tree($w:$v:open) 1
  }
  Tree:build $w
}

#
# Return the full pathname of the label for widget $w that is located
# at real coordinates $x, $y
#
proc Tree:nodeat {w x y} {
  set x [$w canvasx $x]
  set y [$w canvasy $y]
  foreach m [$w find overlapping $x $y $x $y] {
    foreach tag [$w gettags $m] {
      if [string match "node-*" $tag] {
        regexp -- {node-(.*)} $tag match v
        return $v
      }
    }
  }
  return ""
}

#
# Draw the tree on the canvas
#
proc Tree:build {w} {
  global Tree
  $w delete all
  catch {unset Tree($w:buildpending)}
  set Tree($w:y) 30
  Tree:buildlayer $w [getNodeInfo $w root] 10
  $w config -scrollregion [$w bbox all]
  Tree:drawselection $w
}

#
# Internal use only.
# Build a single layer of the tree on the canvas.  Indent by $in pixels
#
proc Tree:buildlayer {w v in} {
  global Tree fonts
  if {$v==[getNodeInfo $w root]} {
    set vx {}
  } else {
    set vx $v
  }
  set start [expr $Tree($w:y)-10]
  foreach c [getNodeInfo $w children $v] {
    set y $Tree($w:y)
    incr Tree($w:y) 17
    $w create line $in $y [expr $in+10] $y -fill gray50
    set text [getNodeInfo $w text $c]
    set options [getNodeInfo $w options $c]
    set icon [getNodeInfo $w icon $c]
    set x [expr $in+12]
    if {[string length $icon]>0} {
      set tags "node-$c"
      set k [$w create image $x $y -image $icon -anchor w -tags $tags]
      incr x 20
    }
    set tags [list "node-$c" "text-$c"]
    set j [$w create text $x $y -text $text -font $fonts(normal) -anchor w -tags $tags]
    eval $w itemconfig $j $options
    if [getNodeInfo $w haschildren $c] {
      if {[info exists Tree($w:$c:open)] && $Tree($w:$c:open)} {
         set j [$w create image $in $y -image Tree:openbm]
         $w bind $j <1> "set Tree($w:$c:open) 0; Tree:build $w"
         Tree:buildlayer $w $c [expr $in+18]
      } else {
         set j [$w create image $in $y -image Tree:closedbm]
         $w bind $j <1> "set Tree($w:$c:open) 1; Tree:build $w"
      }
    }
  }
  set j [$w create line $in $start $in [expr $y+1] -fill gray50 ]
  $w lower $j
}

#
# Internal use only.
# Draw the selection highlight
#
proc Tree:drawselection w {
  global Tree
  if {[string length $Tree($w:selidx)]} {
    $w delete $Tree($w:selidx)
  }
  set v $Tree($w:selection)
  set bbox [$w bbox "text-$v"]
  if {[llength $bbox]==4} {
    set i [eval $w create rectangle $bbox -fill skyblue -outline {{}}]
    set Tree($w:selidx) $i
    $w lower $i
  } else {
    set Tree($w:selidx) {}
  }
}

#
# Internal use only
# Call Tree:build the next time we're idle
#
proc Tree:buildwhenidle w {
  global Tree
  if {![info exists Tree($w:buildpending)]} {
    set Tree($w:buildpending) 1
    after idle "Tree:build $w"
  }
}

