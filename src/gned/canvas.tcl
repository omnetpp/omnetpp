#=================================================================
#  CANVAS.TCL - part of
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


# goBack --
#
# Implements the toolbar "Back" button
#
proc goBack {} {
    global gned canvas

    # find first open canvas starting at end of list
    set canv_id ""
    while {[llength $gned(history)]>0 && $canv_id==""} {
        set canv_id [lindex $gned(history) end]
        set gned(history) [lreplace $gned(history) end end]
        if {![info exist canvas($canv_id,tab)]} {set canv_id ""}
    }

    # switch to that canvas
    if {$canv_id!=""} {
        set old_canv_id $gned(canvas_id)
        if {[lindex $gned(fwd-history) 0]!=$old_canv_id} {
            set gned(fwd-history) [concat $old_canv_id $gned(fwd-history)]
        }
        switchToCanvas $canv_id
        set gned(history) [lreplace $gned(history) end end]
    }
}

# goForward --
#
# Implements the toolbar "Forward" button
#
proc goForward {} {
    global gned canvas

    # find first open canvas starting at beginning of list
    set canv_id ""
    while {[llength $gned(fwd-history)]>0 && $canv_id==""} {
        set canv_id [lindex $gned(fwd-history) 0]
        set gned(fwd-history) [lreplace $gned(fwd-history) 0 0]
        if {![info exist canvas($canv_id,tab)]} {set canv_id ""}
    }

    # switch to that canvas
    if {$canv_id!=""} {
        switchToCanvas $canv_id
    }
}


# openModuleOnCanvas --
#
# Switch to module's canvas or create a new canvas for it.
# Return: id of canvas
#
proc openModuleOnCanvas {modkey} {
    set canv_id [canvasIdFromItemKey $modkey]

    if {$canv_id==""} {
        set canv_id [openModuleOnNewCanvas $modkey]
    } else {
        switchToCanvas $canv_id
    }
    return $canv_id
}


# openUnnamedCanvas --
#
# This procedure is called when the user tries to close the last canvas.
# One canvas must always be open, so we open/create an "unnamed" module
# on a new canvas.
# Return value: canvas id
#
proc openUnnamedCanvas {} {
    # Look for an unnamed, non-dirty ned file. Add a module to it
    # (if it doesn't already have one) and open it on a new canvas.
    global ned

    set nedfilekey ""
    set modkey ""

    # find file
    foreach key $ned(0,childrenkeys) {
        if {![info exist ned($key,being-deleted)] && $ned($key,aux-isunnamed) && !$ned($key,aux-isdirty)} {
            set nedfilekey $key
        }
    }

    # now get a module
    if {$nedfilekey!=""} {
        foreach key $ned($nedfilekey,childrenkeys) {
            if {$ned($key,type)=="module" && ![info exist ned($key,being-deleted)]} {
                set modkey $key
            }
        }
        if {$modkey==""} {
            set modkey [addItem module $nedfilekey]
        }
    } else {
        set nedfilekey [addItem nedfile 0]
        set modkey [addItem module $nedfilekey]
    }

    set canv_id [openModuleOnCanvas $modkey]
    return $canv_id

}


# openModuleOnNewCanvas --
#
# Create a new canvas and display the given component on it.
# Return: id of new canvas
#
proc openModuleOnNewCanvas {modkey} {
    global gned ned canvas fonts

    # check type of item
    set type $ned($modkey,type)
    if {$type!="module"} {
        tk_messageBox -icon warning -type ok \
                      -message "Opening a '$type' on canvas is not implemented."
        return
    }

    # allocate ID for new canvas
    incr gned(canvas_lastid)
    set canv_id $gned(canvas_lastid)

    # create widgets and register in $canvas()
    set canv .main.f.c$canv_id
    set txt  .main.f.t$canv_id
    set tab  .main.f.tabs.c.f.tab$canv_id

    set canvas($canv_id,canvas)      $canv
    set canvas($canv_id,textedit)    $txt
    set canvas($canv_id,mode)        "graphics"
    set canvas($canv_id,tab)         $tab
    set canvas($canv_id,module-key)  $modkey

    # create editor
    canvas $canv -background #a0e0a0  -relief ridge -border 2
    text   $txt  -background wheat2   -relief ridge -border 2  -wrap none -font $fonts(fixed)
    catch {$txt config -undo true}
    configureEditor $txt

    # create tab
    button $tab -command "switchToCanvas $canv_id" -relief solid \
                -highlightthickness 0 -height 1 -pady 0 -borderwidth 1
    pack $tab -expand 0 -fill none -side left

    # bindings for the canvas
    updateGraphicsModeBindings $canv_id
    bind $canv <Control-x> editCut
    bind $canv <Control-c> editCopy
    bind $canv <Control-v> editPaste
    bind $canv <Delete>    editDelete

    # bindings for the textedit
    #bind $txt  <Control-x> "tk_textCut $txt"
    #bind $txt  <Control-c> "tk_textCopy $txt"
    #bind $txt  <Control-v> "tk_textPaste $txt"

    # draw the module on the canvas
    global hack_y
    set hack_y 0
    drawItemRecursive $modkey $canv_id

    # show the canvas -- must be near the end because it needs the module name
    switchToCanvas $canv_id

    # some dirty code to update "<" ">" buttons that scroll the 'tabs' strip
    # (must be done after the switchToCanvas call that sets the tab label!)
    update idletasks
    .main.f.tabs.c config -height [winfo height .main.f.tabs.c.f]
    .main.f.tabs.c config -scrollregion "0 0 [winfo width .main.f.tabs.c.f] 0"
    .main.f.tabs.c xview moveto 1.0

    return $canv_id
}


# switchToCanvas --
#
# Activate the given canvas (as if its tab was clicked by the user)
#
proc switchToCanvas {canv_id} {
    global gned canvas ned

    # unmap old canvas
    if {$gned(canvas_id)!=""} {
        set old_canv_id $gned(canvas_id)

        # add to history
        if {$old_canv_id!=$canv_id} {
            lappend gned(history) $old_canv_id
        }

        # now unmap old canvas
        $canvas($old_canv_id,tab) config -relief flat
        if {$canvas($old_canv_id,mode)=="textedit"} {
            grid forget $canvas($old_canv_id,textedit)
        } elseif {$canvas($old_canv_id,mode)=="graphics"} {
            grid forget $canvas($old_canv_id,canvas)
        } else {
            error "invalid mode"
        }
    }

    # set global vars
    set gned(canvas_id) $canv_id
    set gned(canvas)    $canvas($canv_id,canvas)
    set gned(tab)       $canvas($canv_id,tab)

    # map new canvas/textedit and update tabs
    $canvas($canv_id,tab) config -relief solid
    setCanvasMode $canvas($canv_id,mode)
    updateGraphicsModeBindings $canv_id
}

# setCanvasMode --
#
# Display either the canvas or the textedit for the current canvas
# (as if the user clicked on the [Graphics] or the [NED] button)
#
proc setCanvasMode {mode} {
    global gned canvas ned

    set canv_id $gned(canvas_id)

    # forget old mapping
    if {$canvas($canv_id,mode)=="textedit"} {
        catch {grid forget $canvas($canv_id,textedit)}
    } elseif {$canvas($canv_id,mode)=="graphics"} {
        catch {grid forget $canvas($canv_id,canvas)}
    }

    # make new mapping
    if {$mode=="textedit"} {
        set c $canvas($canv_id,textedit)
        $gned(switchtographics-button) config -relief flat
        $gned(switchtotextedit-button) config -relief raised

        # swap toolbars
        pack $gned(textedit-toolbar) -side left -expand 0 -fill both
        pack forget $gned(graphics-toolbar)

        # enable/disable icons on common toolbar
        #foreach i {cut copy paste} {
        #    $gned(horiz-toolbar).$i config -state active
        #}
        #if {$gned(supports-undo)==1} {
        #    foreach i {undo redo} {
        #        $gned(horiz-toolbar).$i config -state active
        #    }
        #}

    } elseif {$mode=="graphics"} {
        set c $canvas($canv_id,canvas)
        adjustCanvasScrolling $c
        $gned(switchtographics-button) config -relief raised
        $gned(switchtotextedit-button) config -relief flat

        # swap toolbars
        pack forget $gned(textedit-toolbar)
        pack $gned(graphics-toolbar) -side left -expand 0 -fill both

        # enable/disable icons on common toolbar
        #foreach i {cut copy paste undo redo} {
        #    $gned(horiz-toolbar).$i config -state disabled
        #}

    } else {
        error "invalid mode $mode"
    }

    set hsb  .main.f.hsb
    set vsb  .main.f.vsb
    $hsb config -command "$c xview"
    $vsb config -command "$c yview"
    $c config -xscrollcommand "$hsb set" -yscrollcommand "$vsb set"
    grid $c -in .main.f.grid -row 0 -column 0 -rowspan 1 -columnspan 1 -sticky news
    focus $c

    set canvas($canv_id,mode) $mode

    adjustWindowTitle
}


# adjustCanvasScrolling --
#
# Set up proper scrolling region for a given canvas
#
proc adjustCanvasScrolling {c} {
    set bbox [$c bbox all]
    if {$bbox==""} {
        $c config -scrollregion "0 0 0 0"
        return
    }

    set x1  [lindex $bbox 0]
    set y1  [lindex $bbox 1]
    set x2  [lindex $bbox 2]
    set y2  [lindex $bbox 3]
    set w   [expr $x2 - $x1]
    set h   [expr $y2 - $y1]

    set x1new  [expr $x1<0 ? $x1 : 0]
    set y1new  [expr $y1<0 ? $y1 : 0]

    set x2new  [expr $x2 + $w]
    set y2new  [expr $y2 + $h]

    $c config -scrollregion [list $x1new $y1new $x2new $y2new]
}


# closeCurrentCanvas --
#
# Close the current canvas. Module data are not affected.
#
proc closeCurrentCanvas {} {
    global gned canvas ned

    set canv_id $gned(canvas_id)

    # must be in graphics mode (because data structure must be up-to-date)
    if {$canvas($canv_id,mode)=="textedit" && [textCanvasContainsChanges $canv_id]} {
         tk_messageBox -icon warning -type ok -title GNED \
            -message "Switch back to graphics mode first! The NED source has been edited, \
                      and your changes must be backparsed before the canvas can be closed."
         return
    }

    # actually close the canvas
    destroyCanvas $canv_id
}


# textCanvasContainsChanges --
#
# check if a text mode canvas contains unsaved edits.
#
proc textCanvasContainsChanges {canv_id} {
    global gned canvas ned

    # check if there were edits in the source
    set nedcode [$canvas($canv_id,textedit) get 1.0 end]
    set originalnedcode $canvas($canv_id,nedsource)

    # the text widget seems to add a newline, so we'll take this into account
    if {[string compare $nedcode "$originalnedcode\n"]==0} {
        return 0
    } else {
        return 1
    }
}


# destroyCanvas --
#
# Destroy the canvas given with its id.
#
proc destroyCanvas {canv_id} {
    global canvas gned

    debug "destroyCanvas $canv_id"

    # find canvas that will be current after closing this one
    # select previous one, then if it fails, try next one
    for {set newid [expr $canv_id-1]} {$newid>0} {incr newid -1} {
        if [info exist canvas($newid,canvas)] break
    }
    if {![info exist canvas($newid,canvas)]} {
        for {set newid [expr $canv_id+1]} {$newid<=$gned(canvas_lastid)} {incr newid} {
            if [info exist canvas($newid,canvas)] break
        }
    }

    # switch to new canvas; make sure there's always at least one canvas open
    if {![info exist canvas($newid,canvas)]} {
        # openUnnamedCanvas includes switching to the new canvas
        set newid [openUnnamedCanvas]
    } else {
        switchToCanvas $newid
    }

    if {$canv_id==$newid} {
        return
    }

    # destroy widgets and variables associated with old canvas
    destroy $canvas($canv_id,canvas)
    destroy $canvas($canv_id,textedit)
    destroy $canvas($canv_id,tab)
    foreach i [array names canvas "$canv_id,*"] {
        unset canvas($i)
    }

    # adjust scrolling of tabs area
    update idletasks
    .main.f.tabs.c config -scrollregion "0 0 [winfo width .main.f.tabs.c.f] 0"
    .main.f.tabs.c xview moveto 1.0
}


proc adjustWindowTitle {} {
    global gned ned canvas

    set modkey $canvas($gned(canvas_id),module-key)
    set fkey $ned($modkey,parentkey)

    # update window title
    if {$ned($fkey,filename)!=""} {
       wm title . "GNED - $ned($fkey,filename) - $ned($modkey,name)"
    } else {
       wm title . "GNED - $ned($modkey,name) (not saved yet)"
    }

    # update tab
    $canvas($gned(canvas_id),tab) config -text $ned($modkey,name)

}


# checkNEDFileEdits --
#
# Before-save and before-close check: checks if all canvases belonging
# to this file are in graphics mode (or NED source hasn't been changed).
# If everything is OK, returns 0, otherwise pops up error
# dialog and returns 1.
#
proc nedfileContainsTextEdits {nedfilekey} {
    global canvas ned gned
    foreach i [array names canvas "*,canvas"] {
        regsub -- ",canvas" $i "" loop_canvid
        set loop_modkey $canvas($loop_canvid,module-key)
        set loop_nedfilekey $ned($loop_modkey,parentkey)
        # does this canvas belong to the file to be closed?
        if {$loop_nedfilekey==$nedfilekey} {
            # does canvas contain unsaved text edits?
            if {$canvas($loop_canvid,mode)=="textedit" && [textCanvasContainsChanges $loop_canvid]}  {
                switchToCanvas $loop_canvid
                tk_messageBox -icon warning -type ok -title GNED \
                    -message "Please switch back '$ned($loop_modkey,name)' to graphics mode first! \
                       The NED source has been edited, and your changes must be backparsed before the file can be saved."
                return 1
            }
        }
    }
    return 0
}

# exportCanvasesToPostscript --
#
# Loops through all open canvases and saves the pictures into postscript files
# in the given directory. Also writes an xml file which can be used to
# generate clickable image maps.
#
# This proc is used to implement the -c command-line option.
#
proc exportCanvasesToPostscript {dir imgxmlfile imgsuffix} {
    global canvas ned

    set imgxml ""
    append imgxml "<?xml version=\"1.0\"?>\n"
    append imgxml "<images>\n"

    foreach i [array names canvas "*,canvas"] {
        regsub -- ",canvas" $i "" canvid
        set modkey $canvas($canvid,module-key)
        set nedfilekey $ned($modkey,parentkey)

        set modname $ned($modkey,name)
        set nedfilename $ned($nedfilekey,filename)

        regsub -all -- {[./\\:]} $nedfilename "_" tmp
        set psfile [file join $dir "${modname}__${tmp}.eps"]
        set imgfile "${modname}__${tmp}.$imgsuffix"

        busy "Generating postscript to $psfile..."

        switchToCanvas $canvid
        update idletasks
        doExportCanvasToEPS $canvas($i) $psfile

        set c $canvas($i)
        set bbox [$c bbox all]
        set bbx1 [lindex $bbox 0]
        set bby1 [lindex $bbox 1]
        set bbx2 [lindex $bbox 2]
        set bby2 [lindex $bbox 3]
        set width [expr $bbx2-$bbx1]
        set height [expr $bby2-$bby1]

        # create imagemap
        set submodskey [getChildrenWithType $modkey submods]
        append imgxml "  <image name=\"$modname\" nedfilename=\"$nedfilename\" imgfilename=\"$imgfile\" height=\"$height\" width=\"$width\">\n"
        if {$submodskey!=""} {
            foreach submodkey [getChildren $submodskey] {
                set bbox [$c bbox $ned($submodkey,rect-cid)]
                set submodname $ned($submodkey,name)
                set x1 [expr [lindex $bbox 0] - $bbx1]
                set y1 [expr [lindex $bbox 1] - $bby1]
                set x2 [expr [lindex $bbox 2] - $bbx1]
                set y2 [expr [lindex $bbox 3] - $bby1]
                append imgxml "    <area shape=\"rect\" coords=\"$x1,$y1 $x2,$y2\" name=\"$submodname\"/>\n"
            }
        }
        append imgxml "  </image>\n"
    }
    append imgxml "</images>\n"

    # write imagemap file
    if [catch {
       set f [open $imgxmlfile w]
       puts $f $imgxml
       close $f
    } err] {
       tk_messageBox -icon warning -type ok -message "Error: $err"
       return
    }
}


# doExportCanvasToEPS --
#
# Actually saves canvas contents to EPS file
#
proc doExportCanvasToEPS {c psfile} {
    set bbox [$c bbox all]
    set bbx1 [lindex $bbox 0]
    set bby1 [lindex $bbox 1]
    set bbx2 [lindex $bbox 2]
    set bby2 [lindex $bbox 3]
    set width [expr $bbx2-$bbx1]
    set height [expr $bby2-$bby1]

    # export canvas
    $c postscript -file $psfile -x $bbx1 -y $bby1 -width $width -height $height \
                  -pagex 0 -pagey 0 -pagewidth $width -pageheight $height
}
