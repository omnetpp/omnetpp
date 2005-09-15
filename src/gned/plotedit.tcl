#=================================================================
#  PLOTEDIT.TCL - part of
#
#                     OMNeT++/OMNEST
#            Discrete System Simulation in C++
#
#=================================================================

#----------------------------------------------------------------#
#  Copyright (C) 1992-2005 Andras Varga
#
#  This file is distributed WITHOUT ANY WARRANTY. See the file
#  `license' for details on this and other legal matters.
#----------------------------------------------------------------#

#
# Tags used (persistent):
#   key-*        - key in the ned() array
#   module       - main rectange (in fact, 2 rects) of enclosing module
#   background   - item cannot be dragged/right-clicked: background,label etc
#   submod       - rectangle or icon of submodule
#   conn         - connection arrow
#

# printNed --
#
#  DEBUGGING AID.
#  Bound to middle button, prints out ned() elements for the clicked item
#
proc printNed c {
    global ned

    set cid [$c find withtag current]
    if { [lsearch [$c gettags $cid] background]!=-1} {
       set cid ""
    }
    set key [itemKeyFromCid $cid]

    puts ""
    foreach i [lsort [array names ned "$key,*"]] {
      puts "ned($i) = ($ned($i))"
    }
}


# updateGraphicsModeBindings --
#
# Make bindings of mode in $gned(editmode) for current canvas
#
proc updateGraphicsModeBindings {canv_id} {
    global gned canvas
    set c $canvas($canv_id,canvas)

    if {$gned(editmode)=="select/move"} {
        selectOrMoveBindings $c
    } else {
        drawBindings $c
    }
}


#  selectOrMoveBindings --
#
#    Switch editing mode to "select/move".
#
proc selectOrMoveBindings c {
    global gned
    set gned(editmode) "select/move"
    $gned(statusbar).mode config -text "Select, move or resize"
    $gned(graphics-toolbar).select config -relief sunken
    $gned(graphics-toolbar).draw config -relief raised

    $c config -cursor arrow

    # <Double-1> is tricky: must include what <ButtonRelease-1> does!
    bind $c <Delete>           "deleteSelected"
    bind $c <Escape>           "catch {destroy .popup}"
    bind $c <Double-1>         "catch {selectOrMoveEnd $c %x %y}; openSubmodule $c"
    bind $c <Button-3>         "popupMenu $c %X %Y"
    bind $c <Button-1>         "selectOrMoveStart $c %x %y 0"
    bind $c <Control-Button-1> "selectOrMoveStart $c %x %y 1"
    bind $c <Shift-Button-1>   "selectOrMoveStart $c %x %y 1"
    bind $c <B1-Motion>        "selectOrMoveDrag $c %x %y"
    bind $c <ButtonRelease-1>  "catch {selectOrMoveEnd $c %x %y}"
    #bind $c <Insert>           "printNed $c"
    #bind $c <Button-2>         "drawBindings $c"
    #bind $c <Button-2>         "printNed $c"
    bind $c <Button-2>         "$c scan mark %x %y"
    bind $c <B2-Motion>        "$c scan dragto %x %y"

    $c bind submod <Enter>     "$c config -cursor right_ptr"
    $c bind submod <Leave>     "$c config -cursor arrow"
    $c bind module <Enter>     "$c config -cursor right_ptr"
    $c bind module <Leave>     "$c config -cursor arrow"
    $c bind conn   <Enter>     "$c config -cursor hand2"
    $c bind conn   <Leave>     "$c config -cursor arrow"
}

#  drawBindings --
#
#    Switch editing mode to "draw".
#
proc drawBindings c {
    global gned
    set gned(editmode) "draw"
    $gned(statusbar).mode config -text "Draw submodules and connections"
    $gned(graphics-toolbar).select config -relief raised
    $gned(graphics-toolbar).draw config -relief sunken

    $c config -cursor crosshair

    # <Double-1> is tricky: must include what <ButtonRelease-1> does!
    bind $c <Delete>           "deleteSelected"
    bind $c <Escape>           "catch {destroy .popup}"
    bind $c <Button-3>         "popupMenu $c %X %Y"
    bind $c <Double-1>         "drawEnd $c %x %y; openSubmodule $c"
    bind $c <Button-1>         "drawStart $c %x %y"
    bind $c <Control-Button-1> "drawStart $c %x %y"
    bind $c <Shift-Button-1>   "drawStart $c %x %y"
    bind $c <B1-Motion>        "drawDrag $c %x %y"
    bind $c <ButtonRelease-1>  "drawEnd $c %x %y"
    #bind $c <Insert>           "printNed $c"
    #bind $c <Button-2>         "selectOrMoveBindings $c"
    #bind $c <Button-2>         "printNed $c"
    bind $c <Button-2>         "$c scan mark %x %y"
    bind $c <B2-Motion>        "$c scan dragto %x %y"

    $c bind submod <Enter>     "$c config -cursor draft_small"
    $c bind submod <Leave>     "$c config -cursor crosshair"
    $c bind module <Enter>     "$c config -cursor draft_small"
    $c bind module <Leave>     "$c config -cursor crosshair"
    $c bind conn   <Enter>     ""
    $c bind conn   <Leave>     ""
}

set mouse(mode) ""
set mouse(startingX) 0
set mouse(startingY) 0
set mouse(lastX) 0
set mouse(lastY) 0
set mouse(hasmoved) 0
set mouse(arrow) 0
set mouse(rect) 0
set mouse(src-key) 0
set mouse(xside) ""
set mouse(yside) ""
set mouse(tweaked-items) {}
set mouse(conns-to-redraw) {}
set mouse(reanchor-src) 1

# popupMenu --
#
#  Bound to right-click. Pops up local menu for item
#
proc popupMenu {c x y} {

    set cid [$c find withtag current]
    if { [lsearch [$c gettags $cid] background] != -1 } {
       set cid ""
    }

    if {$cid==""} return

    set key [itemKeyFromCid $cid]
    if {$key==""} return

    catch {destroy .popup}
    _cancelLabelEdit $c

    menu .popup -tearoff 0
    foreach i {
      {command -command "editDrawOptions $key" -label {Appearance...} -underline 0}
      {command -command "editProps $key" -label {Properties...} -underline 0}
      {command -command "displayCodeForItem $key" -label {Show NED code...} -underline 0}
      {separator}
      {command -command "renameOnCanvas $key" -label {Rename...} -underline 0}
      {command -command "deleteItem $key" -label {Delete} -underline 1}
    } {
       eval .popup add $i
    }
    .popup post $x $y
}


# propertiesSelected --
#
#  Bound to the Props icon on the toolbar, pops up Properties dialog
#  for selected item. Error if more than one is selected
#
proc propertiesSelected {c} {
    global gned

    set keys [selectedItems]

    if {[llength $keys]==0} {
       tk_messageBox -icon info -type ok -title Info -message "Select an item first."
    } elseif {[llength $keys]>1} {
       tk_messageBox -icon info -type ok -title Info -message "You can set only one item's properties at a time. Sorry."
    } else {
       editProps $keys
    }
}

# drawOptionsSelected --
#
#  Bound to the DrawOptions icon on the toolbar, pops up DrawOptions dialog
#  for selected item. Error if more than one is selected
#
proc drawOptionsSelected {c} {
    global gned

    set keys [selectedItems]

    if {[llength $keys]==0} {
       tk_messageBox -icon info -type ok -title Info -message "Select an item first."
    } elseif {[llength $keys]>1} {
       tk_messageBox -icon info -type ok -title Info -message "You can set only one item's drawing options at a time. Sorry."
    } else {
       editDrawOptions $keys
    }
}


# openSubmodule --
#
#   Bound to double-click on a submodule, opens child module
#   in another canvas.
#
proc openSubmodule c {
   global ned

   set cid [$c find withtag current]
   set key [itemKeyFromCid $cid]

   if {$key==""} {
      return
   }

   if {$ned($key,type)=="submod"} {
      if {$ned($key,like-name)==""} {
         set typename $ned($key,type-name)
      } else {
         set typename $ned($key,like-name)
         tk_messageBox -title GNED -icon info -type ok \
              -message "This submodule is defined using \"like\" -- any module that has the same gates as $typename can be configured to appear here at runtime."
      }
      if {$typename=="" || $typename=="Unknown"} {
         tk_messageBox -title "Info" -icon info -type ok \
              -message "Module type for this submodule is not yet filled in.\
                       Please set the module type in the following dialog."
         editSubmoduleProps $key
      } elseif {[itemKeyFromName $typename module]!=""} {
         set modkey [itemKeyFromName $typename module]
         openModuleOnCanvas $modkey
      } elseif {[itemKeyFromName $typename simple]!=""} {
         # if {[tk_messageBox -title GNED -icon question -type yesno \
         #     -message "This is a simple module of type \"$typename\". Do you want to open the type declaration?"]=="yes"
         # } {
         #    ...
         # }
         set modkey [itemKeyFromName $typename simple]
         editModuleProps $modkey
      } else {
         tk_messageBox -title "Error" -icon warning -type ok \
              -message "Module type \"$typename\" is unknown. If \"$typename\" is an\
                       existing simple module or compound module type, please open the NED file\
                       which contain its definition. If this is a new simulation model\
                       you're working on, you can create this module type with the\
                       New Simple Module or New Compound Module function."
      }
   }
}


# deleteSelected --
#
#   Deletes submods/conns marked with the selected tag.
#
proc deleteSelected {} {
    global ned gned

    set keys [selectedItems]

    if {$keys!=""} {markCanvasDirty}

    foreach key $keys {
       if {[info exist ned($key,type)]} {
            if {$ned($key,type)!="module"} {
                deleteItem $key
            }
       }
    }

    updateTreeManager
}


# renameOnCanvas --
#
# Called when the user renames an item on the canvas.
#
proc renameOnCanvas {key} {
    global gned canvas ned

    # TBD assert that item is on currently open canvas

    set canv_id $gned(canvas_id)
    set c $canvas($canv_id,canvas)
    set cid $ned($key,label-cid)

    editCanvasLabel $c $cid "_doRenameItem $key"
}


# resetModuleBounds --
#
# Adjust size and position of enclosing module, according to the
# submodules in it. Should behave the same way as Tkenv.
#
proc resetModuleBounds {} {
    global ned gned canvas

    set canv_id $gned(canvas_id)
    set c $canvas($canv_id,canvas)
    set modkey $canvas($canv_id,module-key)

    # set display string data to default
    set ned($modkey,disp-xpos)  {}
    set ned($modkey,disp-ypos)  {}
    set ned($modkey,disp-xsize) {}
    set ned($modkey,disp-ysize) {}

    # redraw parent module and connections
    redrawItem $modkey
    foreach key [connKeysOfItem $modkey] {
        _redrawArrow $c $key
    }

    markCanvasDirty

    showTextOnceDialog "resetModuleBounds"
}

# refreshModuleBounds --
#
# Adjust size and position of enclosing module, according to the
# submodules in it. Should behave the same way as Tkenv.
#
proc refreshModuleBounds {} {
    global ned gned canvas

    set canv_id $gned(canvas_id)
    set c $canvas($canv_id,canvas)
    set modkey $canvas($canv_id,module-key)

    # redraw parent module and connections
    redrawItem $modkey
    foreach key [connKeysOfItem $modkey] {
        _redrawArrow $c $key
    }
}

# selectOrMoveStart --
#
#   Bound to button-down in "select/move" mode.
#
proc selectOrMoveStart {c x y ctrl} {
    global mouse canvas gned ned config

    # take scrolling into account
    set x [expr int([$c canvasx $x])]
    set y [expr int([$c canvasy $y])]

    # cancel popup menu and label editing
    catch {destroy .popup}
    _cancelLabelEdit $c

    if {$config(snaptogrid)} {
       snapToGrid x y
    }

    set mouse(startingX) $x
    set mouse(startingY) $y
    set mouse(lastX) $x
    set mouse(lastY) $y
    set mouse(hasmoved) 0

    set cid [$c find withtag current]
    if { [lsearch [$c gettags $cid] background]!=-1} {
       set cid ""
    }
    set key [itemKeyFromCid $cid]

    #
    # now handle different cases depending on what was clicked
    #
    if {$key==""} {

       #
       # Click or Ctrl-Click in empty area: select (or extend selection)
       #
       set mouse(mode) "select"
       if { ! $ctrl} {
          # clear old selection
          deselectAllItems
       }
       set mouse(rect) [$c create rectangle $x $y $x $y ]

    } elseif {$ctrl} {

       #
       # Click on selected object: deselect it
       # Click on non-selected object: select it
       #
       if {$ned($key,aux-isselected)} {
           deselectItem $key
       } else {
           selectItem $key
       }
       set mouse(mode) "nothing"

    } elseif {$ned($key,aux-isselected) && [llength [selectedItems]]>=2} {

       #
       # Click on selected object: move selection
       #   if all selected objects are connections: reanchor them
       #
       set allconns 1
       foreach key [selectedItems] {
          if {$ned($key,type)!="conn"} {
             set allconns 0
          }
       }

       if {$allconns} {
          set mouse(mode) "reanchor"
       } else {
          set mouse(mode) "move"
       }

    } else {
       #
       # Click on non-selected object (or a single selected object):
       #       selection only that item then move/resize it
       #
       if {!$ned($key,aux-isselected)} {
          deselectAllItems
          selectItem $key
       }

       set mouse(mode) ""

       set r 8

       #
       # connection arrow clicked?
       #
       if {$ned($key,type)=="conn"} {
           set arrowcid $ned($key,arrow-cid)
           set cc [_getCoords $c $arrowcid]

           if {abs($x-[lindex $cc 0])<(2*$r) && abs($y-[lindex $cc 1])<(2*$r)} {
               set mouse(mode) "endp-reanchor"
               set mouse(xside) 0
               set mouse(yside) 0
               set mouse(reanchor-src) 1
           } elseif {abs($x-[lindex $cc 2])<(2*$r) && abs($y-[lindex $cc 3])<(2*$r)} {
               set mouse(mode) "endp-reanchor"
               set mouse(xside) 1
               set mouse(yside) 1
               set mouse(reanchor-src) 0
           } else {
               set mouse(mode) "reanchor"
           }
       }


       #
       # parent module clicked?
       #
       if {$ned($key,type)=="module"} {
           set rectcid $ned($key,rect-cid)
           set cc [_getCoords $c $rectcid]

           # enlarge "handles" size by half-width of invisible border
           set r [expr $r+3]

           if {abs($x-[lindex $cc 2])<$r || abs($y-[lindex $cc 3])<$r} {
               # bottom or right side
               set mouse(mode) "resize"
               set mouse(xside) 1
               set mouse(yside) 1
           } else {
               set mouse(mode) "nothing"
           }
       }

       #
       # submodule clicked?
       #
       if {$ned($key,type)=="submod"} {
           set rectcid $ned($key,rect-cid)
           set cc [_getCoords $c $rectcid]

           # enlarge "handles" size by half-width of invisible border
           if {$ned($key,type)=="module"} {
               set r [expr $r+6]
           }

           if {[info exist ned($key,icon-cid)] && $ned($key,icon-cid)!=""} {
               set mouse(mode) "move"
           } elseif {abs($x-[lindex $cc 0])<$r && abs($y-[lindex $cc 1])<$r} {
               # top/left
               set mouse(mode) "resize"
               set mouse(xside) 0
               set mouse(yside) 0
           } elseif {abs($x-[lindex $cc 0])<$r && abs($y-[lindex $cc 3])<$r} {
               # bottom/left
               set mouse(mode) "resize"
               set mouse(xside) 0
               set mouse(yside) 1
           } elseif {abs($x-[lindex $cc 2])<$r && abs($y-[lindex $cc 1])<$r} {
               # top/right
               set mouse(mode) "resize"
               set mouse(xside) 1
               set mouse(yside) 0
           } elseif {abs($x-[lindex $cc 2])<$r && abs($y-[lindex $cc 3])<$r} {
               # bottom/right
               set mouse(mode) "resize"
               set mouse(xside) 1
               set mouse(yside) 1
           } else {
               set mouse(mode) "move"
           }
       }
    }

    #
    # Fill in the mouse(tweaked-items) and mouse(conns-to-redraw)
    # variables with the appropriate keys. We'll use them during
    # mouse drag.
    #
    switch -- $mouse(mode) {
       "move" {
           set mouse(tweaked-items) {}
           set mouse(conns-to-redraw) {}
           foreach i [selectedItems] {
               if {$ned($i,type)=="submod"} {
                   lappend mouse(tweaked-items) $i
                   set mouse(conns-to-redraw) [concat $mouse(conns-to-redraw) [connKeysOfItem $i]]
               }
           }
       }
       "resize" {
           set mouse(tweaked-items)  $key
           set mouse(conns-to-redraw) [connKeysOfItem $key]
       }
       "reanchor" {
           set mouse(tweaked-items)  [selectedItems]
           set mouse(conns-to-redraw) {}
       }
       "endp-reanchor" {
           set mouse(tweaked-items)  [selectedItems]
           set mouse(conns-to-redraw) {}
       }
       "select" {
           set mouse(tweaked-items)  {}
           set mouse(conns-to-redraw) {}
       }
       "nothing" {
           set mouse(tweaked-items)  {}
           set mouse(conns-to-redraw) {}
       }
       default {
           debug "unknown mouse mode '$mouse(mode)'"
       }
    }
}

# selectOrMoveDrag --
#
#   Bound to mouse dragging in "select/move" mode.
#
proc selectOrMoveDrag {c x y} {
    global mouse canvas gned ned config

    # take scrolling into account
    set x [expr int([$c canvasx $x])]
    set y [expr int([$c canvasy $y])]

    if {$config(snaptogrid)} {
       if {$mouse(mode)!="reanchor" && $mouse(mode)!="endp-reanchor"} {
          snapToGrid x y
       }
    }

    set mouse(hasmoved) 1

    if {$mouse(mode) == "select"} {
       #
       # adjust rubber rectangle
       #
       $c coords $mouse(rect) $mouse(startingX) $mouse(startingY) $x $y

    } elseif {$mouse(mode) == "move"} {

       markCanvasDirty

       set byX [expr $x-$mouse(lastX)]
       set byY [expr $y-$mouse(lastY)]
       set mouse(hasmoved) 1

       foreach key $mouse(tweaked-items) {
          foreach i [array names ned "$key,*-cid"] {
             $c move $ned($i) $byX $byY
          }
       }

    } elseif {$mouse(mode) == "resize"} {

       markCanvasDirty

       set byX [expr $x-$mouse(lastX)]
       set byY [expr $y-$mouse(lastY)]
       set mouse(hasmoved) 1

       foreach key $mouse(tweaked-items) {
          if {$ned($key,type)=="submod"} {
             _resizeRect $c $ned($key,rect-cid) $mouse(xside) $mouse(yside) $byX $byY 2
             _adjustSubmod $c $key
          } elseif {$ned($key,type)=="module"} {
             _resizeRect $c $ned($key,rect-cid) $mouse(xside) $mouse(yside) $byX $byY 32
             _adjustModule $c $key
          }
       }

    } elseif {$mouse(mode) == "reanchor"} {

       markCanvasDirty

       set byX [expr $x-$mouse(lastX)]
       set byY [expr $y-$mouse(lastY)]
       set mouse(hasmoved) 1

       foreach key $mouse(tweaked-items) {

          #!!!!!!!!!!
          if {$ned($key,disp-drawmode) == "a"} {
              set ned($key,disp-drawmode) "m"
          }

          _moveAnchoring $c $ned($key,src-ownerkey) \
                         ned($key,disp-src-anchor-x) ned($key,disp-src-anchor-y) $byX $byY
          _moveAnchoring $c $ned($key,dest-ownerkey) \
                         ned($key,disp-dest-anchor-x) ned($key,disp-dest-anchor-y) $byX $byY
          _redrawArrow $c $key
       }

    } elseif {$mouse(mode) == "endp-reanchor"} {

       markCanvasDirty

       set byX [expr $x-$mouse(lastX)]
       set byY [expr $y-$mouse(lastY)]
       set mouse(hasmoved) 1

       foreach key $mouse(tweaked-items) {

          #!!!!!!!!!!
          if {$ned($key,disp-drawmode) == "a"} {
              set ned($key,disp-drawmode) "m"
          }

          if $mouse(reanchor-src) {
             _moveAnchoring $c $ned($key,src-ownerkey) \
                         ned($key,disp-src-anchor-x) ned($key,disp-src-anchor-y) $byX $byY
          } else {
             _moveAnchoring $c $ned($key,dest-ownerkey) \
                         ned($key,disp-dest-anchor-x) ned($key,disp-dest-anchor-y) $byX $byY
          }
          _redrawArrow $c $key
       }
    } elseif {$mouse(mode) == "nothing"} {
       # nothing
    } else {
       debug "unknown mouse mode '$mouse(mode)'"
    }

    foreach key $mouse(conns-to-redraw) {
       _redrawArrow $c $key
    }

    set mouse(lastX) $x
    set mouse(lastY) $y

    return
}


# selectOrMoveEnd --
#
#   Bound to mouse button release in "select/move" mode.
#
proc selectOrMoveEnd {c x y} {
    global mouse gned ned config

    # take scrolling into account
    set x [expr int([$c canvasx $x])]
    set y [expr int([$c canvasy $y])]

    if {$config(snaptogrid)} {
       snapToGrid x y
    }

    if {$mouse(mode) == "resize" || $mouse(mode) == "move"} {

       # update size and position data in ned()
       foreach key $mouse(tweaked-items) {

          if {$ned($key,type)=="module"} {
             # parent module
             set cc [$c coords $ned($key,rect2-cid)]
             #set ned($key,disp-xpos)  [expr int([lindex $cc 0])]
             #set ned($key,disp-ypos)  [expr int([lindex $cc 1])]
             set ned($key,disp-xsize) [expr int([lindex $cc 2])-int([lindex $cc 0])]
             set ned($key,disp-ysize) [expr int([lindex $cc 3])-int([lindex $cc 1])]
          } else {
             # submodules, connections
             set centsiz [_getCenterAndSize $c $key]
             set ned($key,disp-xpos)  [expr int([lindex $centsiz 0])]
             set ned($key,disp-ypos)  [expr int([lindex $centsiz 1])]
             set ned($key,disp-xsize) [expr int([lindex $centsiz 2])]
             set ned($key,disp-ysize) [expr int([lindex $centsiz 3])]
          }
       }

       # refresh parent module, and adjust scrollbars
       refreshModuleBounds
       adjustCanvasScrolling $c

    } elseif {$mouse(mode) == "reanchor" || $mouse(mode) == "endp-reanchor"} {

       # nothing to do

    } elseif {$mouse(mode) == "select"} {
       #
       # Finish selection. Select items enclosed in dragged rectangle.
       #
       $c delete $mouse(rect)

       set cids [$c find enclosed $mouse(startingX) $mouse(startingY) $x $y]
       foreach cid $cids {
          set key [itemKeyFromCid $cid]
          if {$key!=""} {
              selectItem $key
          }
       }
    } elseif {$mouse(mode) == "nothing"} {
       # nothing
    } else {
       debug "unknown mouse mode '$mouse(mode)'"
    }

    # label editing:
    # only if a module or submodule label was clicked
    if {$mouse(startingX)==$x && $mouse(startingY)==$y && [llength [selectedItems]]==1} {
       set key [selectedItems]
       if {$ned($key,type)=="module" || $ned($key,type)=="submod"} {
          set cid [$c find withtag current]
          if {$cid==$ned($key,label-cid)} {
             editCanvasLabel $c $cid "_doRenameItem $key"
          }
       }
    }
}


# drawStart --
#
#   Bound to mouse button down in "draw" mode.
#
proc drawStart {c x y} {
    global mouse gned ned config

    # take scrolling into account
    set x [expr int([$c canvasx $x])]
    set y [expr int([$c canvasy $y])]

    # cancel popup menu and label editing
    catch {destroy .popup}
    _cancelLabelEdit $c

    if {$config(snaptogrid)} {
       snapToGrid x y
    }

    set cid [$c find withtag current]
    if { [lsearch [$c gettags $cid] background] != -1 } {
       set cid ""
    }
    set key [itemKeyFromCid $cid]

    if {$key=="" || ($ned($key,type)!="module" && $ned($key,type)!="submod")} {
       #
       # Click outside submodules: new submodule
       #
       set mouse(mode) "submod"
       set mouse(startingX) $x
       set mouse(startingY) $y
       set mouse(rect) [$c create rect $x $y $x $y]
    } else {
       #
       # Click in submodule: new connection
       #
       set mouse(mode) "conn"
       set mouse(startingX) $x
       set mouse(startingY) $y
       set mouse(arrow) [$c create line $x $y $x $y -arrow last]
       set mouse(src-key) $key
    }
}

# drawDrag --
#
#   Bound to mouse dragging in "draw" mode.
#
proc drawDrag {c x y} {
    global mouse gned config

    # take scrolling into account
    set x [expr int([$c canvasx $x])]
    set y [expr int([$c canvasy $y])]

    if {$config(snaptogrid)} {
       snapToGrid x y
    }

    if {$mouse(mode) == "submod"} {
       $c coords $mouse(rect) $mouse(startingX) $mouse(startingY) $x $y
    } else {
       $c coords $mouse(arrow) $mouse(startingX) $mouse(startingY) $x $y
    }
}

# drawEnd --
#
#   Bound to mouse button release in "draw" mode.
#
proc drawEnd {c x y} {
    global mouse gned canvas ned config

    # take scrolling into account
    set x [expr int([$c canvasx $x])]
    set y [expr int([$c canvasy $y])]

    if {$config(snaptogrid)} {
       snapToGrid x y
    }

    if {$mouse(mode) == "submod"} {
       if {abs($x-$mouse(startingX))<5 || abs($y-$mouse(startingY))<5} {
          #
          # just a click in open area: switch over to select mode
          #
          selectOrMoveBindings $c
       } else {
          #
          # finish dragging out submodule
          #
          set modkey $canvas($gned(canvas_id),module-key)
          set submodskey [getChildrenWithType $modkey submods]
          if {[llength $submodskey]==0} {
              set submodskey [addItem submods $modkey]
          } elseif {[llength $submodskey]>1} {
              error "Internal error: more than one 'submods'"
          }
          set key [addItemWithUniqueName submod $submodskey]

          set cc [$c coords $mouse(rect)]
          set x1 [lindex $cc 0]
          set y1 [lindex $cc 1]
          set x2 [lindex $cc 2]
          set y2 [lindex $cc 3]

          set ned($key,disp-xpos)  [expr int(($x1+$x2)/2)]
          set ned($key,disp-ypos)  [expr int(($y1+$y2)/2)]
          set ned($key,disp-xsize) [expr int($x2-$x1)]
          set ned($key,disp-ysize) [expr int($y2-$y1)]

          drawItem $key
          markCanvasDirty
          updateTreeManager

          # refresh parent module, and adjust scrollbars
          refreshModuleBounds
          adjustCanvasScrolling $c

          if {$ned($modkey,disp-xsize)=="" && $ned($modkey,disp-ysize)==""} {
              showTextOnceDialog "compoundAutoResized"
          }
       }
       $c delete $mouse(rect)
       set mouse(rect) ""

    } elseif { $mouse(mode) == "conn" } {
       if {abs($x-$mouse(startingX))<5 && abs($y-$mouse(startingY))<5} {
          #
          # just a click: ignore it
          #
          selectOrMoveBindings $c
       } else {
          #
          # finish dragging out connection
          #

          # try to find destination submodule
          set destcid [$c find closest $x $y 3 $mouse(arrow)]
          if { [lsearch [$c gettags $destcid] background] != -1 } {
               set destcid ""
          }

          set destkey [itemKeyFromCid $destcid]
          if {$destkey!="" && ($ned($destkey,type)=="module" || $ned($destkey,type)=="submod")} {

              if {$mouse(src-key)!=$destkey || [_confirmSelfConnection]} {

                   set modkey $canvas($gned(canvas_id),module-key)
                   set connskey [getChildrenWithType $modkey conns]
                   if {[llength $connskey]==0} {
                       set connskey [addItem conns $modkey]

                       set ned($connskey,nocheck) $config(noportcheck)

                   } elseif {[llength $connskey]>1} {
                       error "Internal error: more than one 'conns'"
                   }
                   set key [addItem conn $connskey]

                   set ned($key,src-ownerkey) $mouse(src-key)
                   set ned($key,dest-ownerkey) $destkey

                   markCanvasDirty

                   set l_coords [$c coords $mouse(arrow)]
                   set s_coords [_getCoords $c $ned($mouse(src-key),rect-cid)]
                   set d_coords [_getCoords $c $destcid]

                   set an_sr_x [expr int(100*([lindex $l_coords 0]-[lindex $s_coords 0])/ \
                                             ([lindex $s_coords 2]-[lindex $s_coords 0])+0.5)]
                   set an_sr_y [expr int(100*([lindex $l_coords 1]-[lindex $s_coords 1])/ \
                                             ([lindex $s_coords 3]-[lindex $s_coords 1])+0.5)]
                   set an_dt_x [expr int(100*([lindex $l_coords 2]-[lindex $d_coords 0])/ \
                                             ([lindex $d_coords 2]-[lindex $d_coords 0])+0.5)]
                   set an_dt_y [expr int(100*([lindex $l_coords 3]-[lindex $d_coords 1])/ \
                                             ([lindex $d_coords 3]-[lindex $d_coords 1])+0.5)]

                   if {$ned($destkey,type)=="module"} {
                      _snapAnchor an_dt_x 10
                      _snapAnchor an_dt_y 10
                   }
                   if {$ned($mouse(src-key),type)=="module"} {
                      _snapAnchor an_sr_x 10
                      _snapAnchor an_sr_y 10
                   }
                   set ned($key,disp-src-anchor-x) $an_sr_x
                   set ned($key,disp-src-anchor-y) $an_sr_y
                   set ned($key,disp-dest-anchor-x) $an_dt_x
                   set ned($key,disp-dest-anchor-y) $an_dt_y
                   if {$config(connmodeauto)} {
                      set ned($key,disp-drawmode) "a"
                   } else {
                      set ned($key,disp-drawmode) "m"
                   }

                   #  set srcgate to "in" and destgate to "out" in case of connections to a parent-module
                   set submodkey $ned($key,src-ownerkey)
                   if {$ned($submodkey,type)!="submod"} {
                      set ned($key,srcgate) {in}
                   }

                   set submodkey $ned($key,dest-ownerkey)
                   if {$ned($submodkey,type)!="submod"} {
                      set ned($key,destgate) {out}
                   }

                   if {$config(autoextend)} {
                      set_plusplus $key src
                      set_plusplus $key dest
                   }

                   drawItem $key
                   updateTreeManager
              }
          }
       }
       $c delete $mouse(arrow)
       set mouse(arrow) ""
    }
}

proc _confirmSelfConnection {} {
    set ans [tk_messageBox -type yesno -icon warning -title "Confirm" \
        -message "Do you really want to create a connection from the module to itself?"]
    return [expr $ans=="yes"]
}

proc _cancelLabelEdit {c} {
    # taking away the focus will cancel editing if it was active
    focus $c
    $c focus {}
}

proc _getCoords {c cid} {

    if {[$c type $cid]=="image"} {
       set pos [$c coords $cid]
       set x [lindex $pos 0]
       set y [lindex $pos 1]
       set img [$c itemcget $cid -image]
       set hwidth  [expr [image width $img] / 2]
       set hheight [expr [image height $img] / 2]

       set coords "[expr $x-$hwidth] [expr $y-$hheight] \
                   [expr $x+$hwidth] [expr $y+$hheight]"
    } else {
       set coords [$c coords $cid]
    }
    return $coords
}

proc _moveAnchoring {c key xvar yvar dx dy} {
    global ned
    upvar #0 $xvar x
    upvar #0 $yvar y

    if {$ned($key,type)=="module"} {set ismod 1} else {set ismod 0}

    if {!$ismod || ($y==0 || $y==100)} {
       set x [expr int($x+4*$dx+0.5)]
       if {$x<0}   {set x 0}
       if {$x>100} {set x 100}
    }

    if {!$ismod || ($x==0 || $x==100)} {
       set y [expr int($y+4*$dy+0.5)]
       if {$y<0}   {set y 0}
       if {$y>100} {set y 100}
    }
}

proc _snapAnchor {varname r} {
    upvar $varname var

    if {$var<$r}     {set var 0}
    if {$var>100-$r} {set var 100}
}

proc _redrawArrow {c key} {

   global ned
   if {$ned($key,type)!="conn"} {error "item $key is not a conn"}

   set cid $ned($key,arrow-cid)
   set s_coords [_getCoords $c $ned($ned($key,src-ownerkey),rect-cid)]
   set d_coords [_getCoords $c $ned($ned($key,dest-ownerkey),rect-cid)]
   if {[llength [eval list $s_coords $d_coords]]!=8} {
       # something is wrong, better bail out
       return
   }

   # note: the "-" marks are a trick so that empty strings do not cause
   #       trouble. Without them, if some of the achors is "", arrowcoords
   #       would be called with less args then necessary
   set a_coords [eval opp_arrowcoords $s_coords $d_coords  0 1 0 1 \
                         $ned($key,disp-drawmode)- \
                         $ned($key,disp-src-anchor-x)- $ned($key,disp-src-anchor-y)- \
                         $ned($key,disp-dest-anchor-x)- $ned($key,disp-dest-anchor-y)-]
   eval [concat $c coords $cid $a_coords]
}

proc _resizeRect {c rectcid xs ys dx dy minsize} {

   set cc [$c coords $rectcid]

   set x1 [lindex $cc 0]
   set y1 [lindex $cc 1]
   set x2 [lindex $cc 2]
   set y2 [lindex $cc 3]

   if {$xs==0} {
      set x1 [expr $x1+$dx]
      if {[expr $x1+$minsize]>=$x2} {set x1 [expr $x2-$minsize]}
   }
   if {$xs==1} {
      set x2 [expr $x2+$dx]
      if {[expr $x1+$minsize]>=$x2} {set x2 [expr $x1+$minsize]}
   }
   if {$ys==0} {
      set y1 [expr $y1+$dy]
      if {[expr $y1+$minsize]>=$y2} {set y1 [expr $y2-$minsize]}
   }
   if {$ys==1} {
      set y2 [expr $y2+$dy]
      if {[expr $y1+$minsize]>=$y2} {set y2 [expr $y1+$minsize]}
   }

   $c coords $rectcid $x1 $y1 $x2 $y2
}

proc _adjustSubmod {c key} {
   global ned
   set cc [$c coords $ned($key,rect-cid)]
   set xcenter [expr ([lindex $cc 0]+[lindex $cc 2])/2]
   $c coords $ned($key,label-cid) $xcenter [lindex $cc 3]
}

proc _adjustModule {c key} {
   global ned

   set cc [$c coords $ned($key,rect-cid)]
   set cc2 [list [expr [lindex $cc 0]-6] [expr [lindex $cc 1]-6] \
                 [expr [lindex $cc 2]+6] [expr [lindex $cc 3]+6]]

   eval $c coords $ned($key,rect2-cid) $cc2
   eval $c coords $ned($key,background-cid) $cc2
   $c coords $ned($key,label-cid) [expr [lindex $cc 0]-3] [expr [lindex $cc 1]-3]
}

proc _getCenterAndSize {c key} {
    global ned

    if {[info exist ned($key,icon-cid)] && $ned($key,icon-cid)!=""} {
       set cid $ned($key,icon-cid)
       set img [$c itemcget $cid -image]
       return [concat [$c coords $cid] [image width $img] [image height $img]]
    } elseif {$ned($key,rect-cid)!=""} {
       set cc [$c coords $ned($key,rect-cid)]
       set x1 [lindex $cc 0]
       set y1 [lindex $cc 1]
       set x2 [lindex $cc 2]
       set y2 [lindex $cc 3]

       return [list [expr ($x1+$x2)/2] [expr ($y1+$y2)/2] \
                    [expr $x2-$x1] [expr $y2-$y1]]
    }
    return {}
}


# _doRenameItem --
#
# Called when the user renames an item on the canvas.
#
proc _doRenameItem {key name} {
    global ned

    if {$ned($key,type)=="submod"} {
        # parse name: maybe looks like name[size]
        set size ""
        regexp -- {^(.*)\[(.*)\] *$} $name dummy name size

        if {[isNameLegalAndUnique $key $name]} {
           # rename item and set size
           set ned($key,name) $name
           set ned($key,vectorsize) $size
        } else {
           tk_messageBox -type ok -title GNED -icon warning \
               -message "'$name' is not a legal name (contains invalid characters, is a reserved word, or not unique)"
        }

    } elseif {[info exist ned($key,name)]} {
        if {[isNameLegalAndUnique $key $name]} {
           set ned($key,name) $name
        } else {
           tk_messageBox -type ok -title GNED -icon warning \
               -message "'$name' is not a legal name (contains invalid characters, is a reserved word, or not unique)"
        }
    } else {
        error "item doesn't have a name attribute"
    }

    # update all display elements
    if {$ned($key,type)=="module"} {
        adjustWindowTitle
    }
    markCanvasDirty
    updateTreeManager

    # return current name
    if {$ned($key,type)=="submod" && $ned($key,vectorsize)!=""} {
        set newname "$ned($key,name)\[$ned($key,vectorsize)\]"
    } else {
        set newname $ned($key,name)
    }
    return $newname
}


# markCanvasDirty --
#
# Mark the component (module) on the current canvas as changed.
# If the file just became "dirty", reflect this in the tree manager.
#
proc markCanvasDirty {} {
    global canvas ned gned

    set modkey $canvas($gned(canvas_id),module-key)
    if [markNedFileOfItemDirty $modkey] {updateTreeManager}
}


# snapToGrid --
#
# Modify the value of the passed two variables containing coordinates
# to achieve 'snap to grid' effect
#
proc snapToGrid {xvar yvar} {
    upvar $xvar x
    upvar $yvar y

    set gridsize 8
    incr x [expr $gridsize/2]
    incr y [expr $gridsize/2]
    set x [expr $x - fmod($x,$gridsize)]
    set y [expr $y - fmod($y,$gridsize)]
}
