#!/usr/bin/wish
#
# Copyright (C) 1997,1998 D. Richard Hipp
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Library General Public
# License as published by the Free Software Foundation; either
# version 2 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Library General Public License for more details.
#
# You should have received a copy of the GNU Library General Public
# License along with this library; if not, write to the
# Free Software Foundation, Inc., 59 Temple Place - Suite 330,
# Boston, MA  02111-1307, USA.
#
# Author contact information:
#   drh@acm.org
#   http://www.hwaci.com/drh/
#
# $Revision$
#
option add *highlightThickness 0

switch $tcl_platform(platform) {
  unix {
    set Tree(font) \
      -adobe-helvetica-medium-r-normal-*-11-80-100-100-p-56-iso8859-1
  }
  windows {
    set Tree(font) \
      -adobe-helvetica-medium-r-normal-*-14-100-100-100-p-76-iso8859-1
  }
}

#
# Create a new tree widget.  $args become the configuration arguments to
# the canvas widget from which the tree is constructed.
#
proc Tree:create {w args} {
  global Tree
  eval canvas $w -bg white $args
  bind $w <Destroy> "Tree:delitem $w /"
  Tree:dfltconfig $w /
  Tree:buildwhenidle $w
  set Tree($w:selection) {}
  set Tree($w:selidx) {}
}

# Initialize a element of the tree.
# Internal use only
#
proc Tree:dfltconfig {w v} {
  global Tree
  set Tree($w:$v:children) {}
  set Tree($w:$v:open) 0
  set Tree($w:$v:icon) {}
  set Tree($w:$v:tags) {}
}

#
# Pass configuration options to the tree widget
#
proc Tree:config {w args} {
  eval $w config $args
}

#
# Insert a new element $v into the tree $w.
#
proc Tree:newitem {w v args} {
  global Tree
  set dir [file dirname $v]
  set n [file tail $v]
  if {![info exists Tree($w:$dir:open)]} {
    error "parent item \"$dir\" is missing"
  }
  set i [lsearch -exact $Tree($w:$dir:children) $n]
  if {$i>=0} {
    error "item \"$v\" already exists"
  }
  lappend Tree($w:$dir:children) $n
  set Tree($w:$dir:children) [lsort $Tree($w:$dir:children)]
  Tree:dfltconfig $w $v
  foreach {op arg} $args {
    switch -exact -- $op {
      -image {set Tree($w:$v:icon) $arg}
      -tags {set Tree($w:$v:tags) $arg}
    }
  }
  Tree:buildwhenidle $w
}

#
# Delete element $v from the tree $w.  If $v is /, then the widget is
# deleted.
#
proc Tree:delitem {w v} {
  global Tree
  if {![info exists Tree($w:$v:open)]} return
  if {[string compare $v /]==0} {
    # delete the whole widget
    catch {destroy $w}
    foreach t [array names Tree $w:*] {
      unset Tree($t)
    }
  }
  foreach c $Tree($w:$v:children) {
    catch {Tree:delitem $w $v/$c}
  }
  unset Tree($w:$v:open)
  unset Tree($w:$v:children)
  unset Tree($w:$v:icon)
  set dir [file dirname $v]
  set n [file tail $v]
  set i [lsearch -exact $Tree($w:$dir:children) $n]
  if {$i>=0} {
    set Tree($w:$dir:children) [lreplace $Tree($w:$dir:children) $i $i]
  }
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

# Internal use only.
# Draw the tree on the canvas
proc Tree:build w {
  global Tree
  $w delete all
  catch {unset Tree($w:buildpending)}
  set Tree($w:y) 30
  Tree:buildlayer $w / 10
  $w config -scrollregion [$w bbox all]
  Tree:drawselection $w
}

# Internal use only.
# Build a single layer of the tree on the canvas.  Indent by $in pixels
proc Tree:buildlayer {w v in} {
  global Tree
  if {$v=="/"} {
    set vx {}
  } else {
    set vx $v
  }
  set start [expr $Tree($w:y)-10]
  foreach c $Tree($w:$v:children) {
    set y $Tree($w:y)
    incr Tree($w:y) 17
    $w create line $in $y [expr $in+10] $y -fill gray50
    set icon $Tree($w:$vx/$c:icon)
    set taglist x
    foreach tag $Tree($w:$vx/$c:tags) {
      lappend taglist $tag
    }
    set x [expr $in+12]
    if {[string length $icon]>0} {
      set k [$w create image $x $y -image $icon -anchor w -tags $taglist]
      incr x 20
      set Tree($w:tag:$k) $vx/$c
    }
    set j [$w create text $x $y -text $c -font $Tree(font) \
                                -anchor w -tags $taglist]
    set Tree($w:tag:$j) $vx/$c
    set Tree($w:$vx/$c:tag) $j
    if {[string length $Tree($w:$vx/$c:children)]} {
      if {$Tree($w:$vx/$c:open)} {
         set j [$w create image $in $y -image Tree:openbm]
         $w bind $j <1> "set Tree($w:$vx/$c:open) 0; Tree:build $w"
         Tree:buildlayer $w $vx/$c [expr $in+18]
      } else {
         set j [$w create image $in $y -image Tree:closedbm]
         $w bind $j <1> "set Tree($w:$vx/$c:open) 1; Tree:build $w"
      }
    }
  }
  set j [$w create line $in $start $in [expr $y+1] -fill gray50 ]
  $w lower $j
}

# Open a branch of a tree
#
proc Tree:open {w v} {
  global Tree
  if {[info exists Tree($w:$v:open)] && $Tree($w:$v:open)==0
      && [info exists Tree($w:$v:children)]
      && [string length $Tree($w:$v:children)]>0} {
    set Tree($w:$v:open) 1
    Tree:build $w
  }
}

proc Tree:close {w v} {
  global Tree
  if {[info exists Tree($w:$v:open)] && $Tree($w:$v:open)==1} {
    set Tree($w:$v:open) 0
    Tree:build $w
  }
}

# Internal use only.
# Draw the selection highlight
proc Tree:drawselection w {
  global Tree
  if {[string length $Tree($w:selidx)]} {
    $w delete $Tree($w:selidx)
  }
  set v $Tree($w:selection)
  if {[string length $v]==0} return
  if {![info exists Tree($w:$v:tag)]} return
  set bbox [$w bbox $Tree($w:$v:tag)]
  if {[llength $bbox]==4} {
    set i [eval $w create rectangle $bbox -fill skyblue -outline {{}}]
    set Tree($w:selidx) $i
    $w lower $i
  } else {
    set Tree($w:selidx) {}
  }
}

# Internal use only
# Call Tree:build then next time we're idle
proc Tree:buildwhenidle w {
  global Tree
  if {![info exists Tree($w:buildpending)]} {
    set Tree($w:buildpending) 1
    after idle "Tree:build $w"
  }
}

#
# Return the full pathname of the label for widget $w that is located
# at real coordinates $x, $y
#
proc Tree:labelat {w x y} {
  set x [$w canvasx $x]
  set y [$w canvasy $y]
  global Tree
  foreach m [$w find overlapping $x $y $x $y] {
    if {[info exists Tree($w:tag:$m)]} {
      return $Tree($w:tag:$m)
    }
  }
  return ""
}

#################
#
# The remainder is code that demonstrates the use of the Tree
# widget.
#
. config -bd 3 -relief flat
frame .f -bg white
pack .f -fill both -expand 1
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
frame .f.mb -bd 2 -relief raised
pack .f.mb -side top -fill x
menubutton .f.mb.file -text File -menu .mb.file.menu
catch {
  menu .f.mb.file.menu
  .f.mb.file.menu add command -label Quit -command exit
}
menubutton .f.mb.edit -text Edit
menubutton .f.mb.view -text View
menubutton .f.mb.help -text Help
pack .f.mb.file .f.mb.edit .f.mb.view .f.mb.help -side left -padx 10
Tree:create .f.w -width 150 -height 400 -yscrollcommand {.f.sb set}
scrollbar .f.sb -orient vertical -command {.f.w yview}
pack .f.w -side left -fill both -expand 1 -padx 5 -pady 5
pack .f.sb -side left -fill y
frame .f.c -height 400 -width 400 -bg white
pack .f.c -side left -fill both -expand 1
label .f.c.l -width 40 -text {} -bg [.f.c cget -bg]
pack .f.c.l -expand 1
foreach z {1 2 3} {
  Tree:newitem .f.w /dir$z -image idir
  foreach x {1 2 3 4 5 6} {
    Tree:newitem .f.w /dir$z/file$x -image ifile
  }
  Tree:newitem .f.w /dir$z/subdir -image idir
  foreach y {1 2} {
    Tree:newitem .f.w /dir$z/subdir/file$y -image ifile
  }
  foreach zz {1 2 3 4} {
    Tree:newitem .f.w /dir$z/subdir/ssdir$zz -image idir
    Tree:newitem .f.w /dir$z/subdir/ssdir$zz/file1  ;# No icon!
    Tree:newitem .f.w /dir$z/subdir/ssdir$zz/file2 -image ifile
  }
}
.f.w bind x <1> {
  set lbl [Tree:labelat %W %x %y]
  Tree:setselection %W $lbl
  .f.c.l config -text $lbl
}
.f.w bind x <Double-1> {
  Tree:open %W [Tree:labelat %W %x %y]
}
update
