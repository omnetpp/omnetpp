#=================================================================
#  TREE.TCL - part of
#
#                     OMNeT++/OMNEST
#            Discrete System Simulation in C++
#
#   Losely based on:
#      tree.tcl, Copyright (C) 1997,1998 D. Richard Hipp
#      Author contact information:
#        drh@acm.org
#        http://www.hwaci.com/drh/
#      tree.tcl falls under the GNU Library General Public License
#
#=================================================================

#----------------------------------------------------------------#
#  Copyright (C) 1992-2017 Andras Varga
#
#  This file is distributed WITHOUT ANY WARRANTY. See the file
#  `license' for details on this and other legal matters.
#----------------------------------------------------------------#

#
# Changes by Andras Varga:
# - Tree:init to take content provider procedure
# - texts may contain "\b" to turn *bold* on/off
# - multi-line text accepted (beware when mixing with bold:
#   on each "\b", text jumps back to the top!)
# - multi-line texts can be opened/closed (if there're child nodes
#   as well, they open/close together with the text)
# - added keyboard navigation and proc Tree:view
# - can act as checkboxtree (checkbox can be turned on/off per tree node)
#

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
# Initialize a new tree widget. $w must be a previously created Tk canvas, and
# f a content provider procedure (like treeManager:getNodeInfo).
#
proc Tree:init {w f} {
  global Tree
  set Tree($w:function) $f
  set v [$Tree($w:function) $w root]
  set Tree($w:$v:open) 0
  set Tree($w:selection) {}
  set Tree($w:selidx) {}
  set Tree($w:lastid) 0
  set Tree($w:boldfg) blue4
  #set Tree($w:foreground) black
  #set Tree($w:selectbg) skyblue
  #set Tree($w:selectfg) black
  set Tree($w:foreground) [ttk::style lookup Treeview -foreground]
  set Tree($w:selectbg) [ttk::style lookup Treeview -background {focus selected}]
  set Tree($w:selectfg) [ttk::style lookup Treeview -foreground {focus selected}]

  # forget tree checked state to force re-read
  foreach i [array names Tree "$w:*:checked"] {
      unset Tree($i)
  }

  Tree:buildwhenidle $w
  $w config -takefocus 1 -highlightcolor gray -highlightthickness 1
  Tree:addbindings $w
}

#
# Change the selection to the indicated item
#
proc Tree:setselection {w v} {
  global Tree
  set Tree($w:selection) $v
  $Tree($w:function) $w selectionchanged $v
  Tree:build $w
  Tree:drawselection $w
  Tree:view $w $v
}

#
# Retrieve the current selection
#
proc Tree:getselection w {
  global Tree
  return $Tree($w:selection)
}

#
# Scroll the canvas so that the given node becomes visible,
# provided it's currently displayed at all.
#
proc Tree:view {w v} {
  set bbox [$w bbox "node-$v"]
  if {$bbox!=""} {
    set done 0
    while {!$done} {
      set again 1
      set y1 [lindex $bbox 1]
      set y2 [lindex $bbox 3]
      set cy1 [$w canvasy 0]
      set cy2 [expr $cy1 + [winfo height $w]]
      if {$y1 < $cy1} {
          # scroll up
          $w yview scroll -1 units
          update idletasks
      } elseif {$y2 > $cy2 && ($y2-$y1 < $cy2-$cy1)} {
          # scroll down
          $w yview scroll 1 units
          update idletasks
      } else {
          set done 1
      }
    }
  }
}

#
# Open a branch of a tree
#
proc Tree:open {w v} {
  global Tree
  set isopen 0
  catch {set isopen $Tree($w:$v:open)}
  if {!$isopen && [$Tree($w:function) $w haschildren $v]} {
    set Tree($w:$v:open) 1
    Tree:build $w
  }
}

#
# Close a branch of a tree
#
proc Tree:close {w v} {
  global Tree
  set isopen 0
  catch {set isopen $Tree($w:$v:open)}
  if {$isopen} {
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
  return [Tree:nodeatcc $w $x $y]
}

#
# Return the full pathname of the label for widget $w that is located
# at canvas coordinates $x, $y
#
proc Tree:nodeatcc {w x y} {
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
# Returns the tooltip for the given node
#
proc Tree:gettooltip {w v} {
  global Tree
  if {$v==""} {return ""}
  return [$Tree($w:function) $w tooltip $v]
}

#
# Returns the name of the variable that's bound to the checkbox state
# for the given node. May not exist. Needs to be declared "global" to be accessed.
#
proc Tree:getcheckvar {w v} {
  return "Tree($w:$v:checked)"
}

#
# Returns the list of nodes for which checkbox state is kept.
#
proc Tree:getcheckvars {w} {
  global Tree
  set result {}
  foreach i [array names Tree "$w:*:checked"] {
    regsub -- "$w:(.*):checked" $i "\\1" v
    lappend result $v
  }
  return $result
}

#
# Draw the tree on the canvas
#
proc Tree:build {w} {
  global Tree
  $w delete all
  catch {unset Tree($w:buildpending)}
  set Tree($w:y) 30
  Tree:buildlayer $w [$Tree($w:function) $w root] 10
  Tree:drawselection $w
  $w config -scrollregion [$w bbox all]

  ## attempt to prevent scrolling when nodes don't fill the canvas. doesn't work.
  #set bbox [$w bbox all]
  #set h [winfo height $w]
  #if {[lindex $bbox 3]<$h} {lset bbox 3 $h}
  #$w config -scrollregion $bbox
}

#
# Internal use only.
# Build a single layer of the tree on the canvas.  Indent by $in pixels
#
proc Tree:buildlayer {w v in} {
  global Tree
  if {$v==[$Tree($w:function) $w root]} {
    set vx {}
  } else {
    set vx $v
  }
  set start [expr $Tree($w:y)-12]
  set y $Tree($w:y)
  foreach c [$Tree($w:function) $w children $v] {
    set y $Tree($w:y)
    $w create line $in $y [expr $in+10] $y -fill gray50

    # get data
    set needcheckbox [$Tree($w:function) $w needcheckbox $c]
    set text [$Tree($w:function) $w text $c]
    set options [$Tree($w:function) $w options $c]
    set icon [$Tree($w:function) $w icon $c]

    # draw checkbox, icon and text
    set x [expr $in+12]
    if {$needcheckbox} {
        set tag "_$Tree($w:lastid)"
        incr Tree($w:lastid)
        set cb $w.$tag
        checkbutton $cb -padx 0 -pady 0 -bg [$w cget -bg] -takefocus 0 -activebackground [$w cget -bg] -variable Tree($w:$c:checked) -command [list Tree:checkstatechanged $w $c]
        $w create window $x $y -window $cb -anchor w
        incr x [winfo reqwidth $cb]
    }
    if {[string length $icon]>0} {
      set tags [list "node-$c" "tooltip"]
      set k [$w create image $x $y -image $icon -anchor w -tags $tags]
      incr x 20
    }
    set tags [list "node-$c" "text-$c" "tooltip"]
    set ismultiline [expr [string first "\n" $text]!=-1]
    set isopen 0
    if {$ismultiline && [info exists Tree($w:$c:open)]} {set isopen $Tree($w:$c:open)}
    set j [Tree:createtext $w $x $y $text $isopen $tags]
    eval $w itemconfig $j $options

    # draw helper line for keyboard navigation
    set bbox [$w bbox $j]
    set top [lindex [$w bbox $j] 1]
    set bottom [lindex [$w bbox $j] 3]
    $w create line 0 $top 0 $bottom -tags [list "node-$c" "helper"] -fill ""
    set Tree($w:y) [expr $bottom + 10]

    # draw [+] or [-] symbols
    if {$ismultiline || [$Tree($w:function) $w haschildren $c]} {
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
# Displays the given text. "\b" charachers switch *bold* on/off. Returns tag.
#
proc Tree:createtext {w x y txt isopen tags} {
    global Tree

    if {!$isopen} {regsub -all "\n" $txt " \\ " txt}

    set tag "_$Tree($w:lastid)"
    incr Tree($w:lastid)
    lappend tags $tag

    # position center of 1st line on y given coord (we use "nw" achor)
    incr y -6

    set bold 0
    foreach txtfrag [split $txt "\b"] {
        #set font [expr {$bold ? "BoldFont" : "TkDefaultFont"}]
        set font TkDefaultFont
        set color [expr {$bold ? "$Tree($w:boldfg)" : "$Tree($w:foreground)"}]
        set id [$w create text $x $y -text $txtfrag -anchor nw -font $font -fill $color -tags $tags]
        set x [lindex [$w bbox $id] 2]
        set bold [expr !$bold]
    }


    return $tag
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
  $w itemconfigure "text-$v" -fill $Tree($w:selectfg);
  set bbox [$w bbox "text-$v"]
  if {[llength $bbox]==4} {
    lset bbox 1 [expr [lindex $bbox 1]-1]
    lset bbox 2 [expr [lindex $bbox 2]+1]
    lset bbox 3 [expr [lindex $bbox 3]+1]
    set i [eval $w create rectangle $bbox -fill $Tree($w:selectbg) -outline {{}}]
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

#
# Internal use only.
# Add keyboard bindings to the tree widget
#
proc Tree:addbindings w {
  bind $w <Button-1> {
      focus %W
      set key [Tree:nodeat %W %x %y]
      if {$key!=""} {
          Tree:setselection %W $key
      }
  }
  bind $w <Up> {Tree:up %W}
  bind $w <Down> {Tree:down %W}
  bind $w <Return> {Tree:togglestate %W}
  bind $w <space> {Tree:toggleorcheck %W}
  bind $w <Left> {Tree:togglestate %W}
  bind $w <Right> {Tree:togglestate %W}
}

#
# Internal use only.
# Move selection to the element above the selected one
#
proc Tree:up w {
  set sel [Tree:getselection $w]
  if {$sel!=""} {
    set bbox [$w bbox "node-$sel"]
    set y [expr [lindex $bbox 1]-5]
    set nodeabove [Tree:nodeatcc $w 0 $y]
    if {$nodeabove!=""} {
        Tree:setselection $w $nodeabove
    }
  }
}

#
# Internal use only.
# Move selection to the element below the selected one
#
proc Tree:down w {
  set sel [Tree:getselection $w]
  if {$sel!=""} {
    set bbox [$w bbox "node-$sel"]
    set y [expr [lindex $bbox 3]+5]
    set nodebelow [Tree:nodeatcc $w 0 $y]
    if {$nodebelow!=""} {
        Tree:setselection $w $nodebelow
    }
  }
}

#
# Internal use only.
# If the node has a checkbox, check/uncheck it; otherwise toggle open/closed state
#
proc Tree:toggleorcheck w {
  global Tree
  set v [Tree:getselection $w]
  if {[info exist Tree($w:$v:checked)]} {
    set Tree($w:$v:checked) [expr !$Tree($w:$v:checked)]
    Tree:checkstatechanged $w $v
  } else {
    Tree:toggle $w $v
  }
}

#
# Internal use only.
# Opens the selected node if it's closed, and closes it if it's open
#
proc Tree:togglestate w {
  global Tree
  set v [Tree:getselection $w]
  if {$v!=""} {
    Tree:toggle $w $v
  }
}

#
# Internal use only.
# Called when a checkbox state changed.
#
proc Tree:checkstatechanged {w v} {
  global Tree
  set state $Tree($w:$v:checked)
  Tree:checksubtree $w $v $state
}

# Internal use only: helper for checkstatechanged
proc Tree:checksubtree {w v state} {
  global Tree
  set Tree($w:$v:checked) $state
  foreach c [$Tree($w:function) $w children $v] {
      Tree:checksubtree $w $c $state
  }
}

# Internal use only: initialize checkbox state for the whole subtree
proc Tree:readsubtreecheckboxstate {w v} {
  global Tree
  $Tree($w:function) $w needcheckbox $v   ;# this forces reading the checkbox state
  foreach c [$Tree($w:function) $w children $v] {
      Tree:readsubtreecheckboxstate $w $c
  }
}
