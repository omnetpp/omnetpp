#==========================================================================
#  CANVAS.TCL -
#            part of the GNED, the Tcl/Tk graphical topology editor of
#                            OMNeT++
#
#   By Andras Varga
#   Additions: Istvan Pataki, Gabor Vincze ({deity|vinczeg}@sch.bme.hu)
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
puts "dbg: openModuleOnCanvas returning $canv_id"
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

puts "dbg: openUnnamedCanvas: FIXME! should not find the currently open canvas!"

    set nedfilekey ""
    set modkey ""

    # find file
    foreach key $ned(0,childrenkeys) {
        if {$ned($key,unnamed) && !$ned($key,dirty)} {
            set nedfilekey $key
        }
    }

    # now get a module
    if {$nedfilekey!=""} {
        foreach key $ned($nedfilekey,childrenkeys) {
            if {$ned($key,type)=="module"} {
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

puts "dbg: openUnnamedCanvas: file $nedfilekey, module $modkey"

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
    set w .omnetpp

    # allocate ID for new canvas
    incr gned(canvas_lastid)
    set canv_id $gned(canvas_lastid)

    # create widgets and register in $canvas()
    set canv $w.main.f.c$canv_id
    set txt  $w.main.f.t$canv_id
    set tab $w.main.f.tabs.tab$canv_id

    set canvas($canv_id,canvas)      $canv
    set canvas($canv_id,textedit)    $txt
    set canvas($canv_id,mode)        "graphics"
    set canvas($canv_id,tab)         $tab
    set canvas($canv_id,module-key)  $modkey

    canvas $canv -background #a0e0a0  -relief ridge -border 2
    text   $txt  -background wheat2   -relief ridge -border 2 -wrap none -font $fonts(fixed)

    button $tab -command "switchToCanvas $canv_id" -relief ridge \
                -bg wheat2 -highlightthickness 0 -height 1
    pack $tab -anchor n -expand 0 -fill none -side left

    # bindings for the canvas
    selectOrMoveBindings $canv
    bind $canv <Control-x> editCut
    bind $canv <Control-c> editCopy
    bind $canv <Control-v> editPaste
    bind $canv <Delete>    editDelete

    # bindings for the textedit
    #bind $txt  <Control-x> "tk_textCut $txt"
    #bind $txt  <Control-c> "tk_textCopy $txt"
    #bind $txt  <Control-v> "tk_textPaste $txt"

    # draw the module on the canvas
    drawItemRecursive $modkey $canv_id

    # show the canvas -- must be the last one because it needs the module name
    switchToCanvas $canv_id
puts "dbg: openModuleOnNewCanvas created $canv_id"
    return $canv_id
}


# switchToCanvas --
#
# Activate the given canvas (as if its tab was clicked by the user)
#
proc switchToCanvas {canv_id} {
    global gned canvas ned
    set w .omnetpp

puts "dbg: switchToCanvas $canv_id entered"

    # unmap old canvas
    if {$gned(canvas_id)!=""} {
        set old_canv_id $gned(canvas_id)
        $canvas($old_canv_id,tab) config -relief flat -bg #d9d9d9
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
    $canvas($canv_id,tab) config -relief ridge -bg wheat2
    setCanvasMode $canvas($canv_id,mode)
}

# setCanvasMode --
#
# Display either the canvas or the textedit for the current canvas
# (as if the user clicked on the [Graphics] or the [NED] button)
#
proc setCanvasMode {mode} {
    global gned canvas ned
    set w .omnetpp

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
        $gned(horiz-toolbar).graph config -relief raised
        $gned(horiz-toolbar).ned config -relief sunken
    } elseif {$mode=="graphics"} {
        set c $canvas($canv_id,canvas)
        adjustCanvasScrolling $c
        $gned(horiz-toolbar).graph config -relief sunken
        $gned(horiz-toolbar).ned config -relief raised
    } else {
        error "invalid mode $mode"
    }

    set hsb  $w.main.f.hsb
    set vsb  $w.main.f.vsb
    $hsb config -command "$c xview"
    $vsb config -command "$c yview"
    $c config -xscrollcommand "$hsb set" -yscrollcommand "$vsb set"
    grid $c -in $w.main.f.grid -row 0 -column 0 -rowspan 1 -columnspan 1 -sticky news
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
    set cwidth  [expr 2*[lindex $bbox 2]]
    set cheight [expr 2*[lindex $bbox 3]]
    $c config -scrollregion "0 0 $cwidth $cheight"
}


# closeCurrentCanvas --
#
# Close the current canvas. Module data are not affected.
#
proc closeCurrentCanvas {} {
    global gned canvas ned

    set canv_id $gned(canvas_id)

    # must be in graphics mode (because data structure must be up-to-date)
    if {$canvas($canv_id,mode)=="textedit"} {
puts "dbg: must check if NED text has in fact changed"
         tk_messageBox -icon warning -type ok -title GNED \
            -message "Switch back to graphics mode first. Changes on the\
                      NED source must be backparsed before canvas can be closed."
         return
    }

    # actually close the canvas
    destroyCanvas $canv_id
}


# destroyCanvas --
#
# Destroy the canvas given with its id.
#
proc destroyCanvas {canv_id} {
    global canvas gned

puts "dbg: destroyCanvas $canv_id"

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

    if {$canv_id==$newid} {error "internal error in destroyCanvas"}

    # destroy widgets and variables associated with old canvas
    destroy $canvas($canv_id,canvas)
    destroy $canvas($canv_id,textedit)
    destroy $canvas($canv_id,tab)
    foreach i [array names canvas "$canv_id,*"] {
        unset canvas($i)
    }
}


proc adjustWindowTitle {} {
    global gned ned canvas

    set modkey $canvas($gned(canvas_id),module-key)
    set fkey $ned($modkey,parentkey)

    # update window title
    if {$ned($fkey,filename)!=""} {
       wm title .omnetpp "GNED - $ned($fkey,filename) - $ned($modkey,name)"
    } else {
       wm title .omnetpp "OMNeT++/GNED - $ned($modkey,name) (not saved yet)"
    }

    # update tab
    $canvas($gned(canvas_id),tab) config -text $ned($modkey,name)

}


