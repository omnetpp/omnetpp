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
# switch to module's canvas or create a new canvas for it
#
proc openModuleOnCanvas {modkey} {
    set canv_id [canvasIdFromItemKey $modkey]

    if {$canv_id==""} {
        openModuleOnNewCanvas $modkey
    } else {
        switchToCanvas $canv_id
    }
}

# openModuleOnNewCanvas --
#
# create a new canvas and display the given component on it
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
    set canvas($canv_id,changed)     0
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
}

# closeCanvas --
#
# Close the canvas given by canvas id. Returns 1 if successful.
#
proc closeCanvas {id} {
    #FIXME:
    puts "dbg: closeCanvas to be fixed"
    switchToCanvas $id
    closeCurrentCanvas

    # success not guaranteed here!!!
    # pretend success:
    return 1
}

# switchToCanvas --
#
# Activate the given canvas (as if its tab was clicked by the user)
#
proc switchToCanvas {id} {
    global gned canvas ned
    set w .omnetpp

    # unmap old canvas
    if {$gned(canvas_id)!=""} {
        set canv_id $gned(canvas_id)
        $canvas($canv_id,tab) config -relief flat -bg #d9d9d9
        if {$canvas($canv_id,mode)=="textedit"} {
            grid forget $canvas($canv_id,textedit)
        } elseif {$canvas($canv_id,mode)=="graphics"} {
            grid forget $canvas($canv_id,canvas)
        } else {
            error "invalid mode"
        }
    }

    # set global vars
    set gned(canvas_id) $id
    set gned(canvas)    $canvas($id,canvas)
    set gned(tab)       $canvas($id,tab)

    # map new canvas/textedit and update tabs
    set canv_id $id
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

    # cannot close last canvas
    if {![info exist canvas($newid,canvas)]} {
         tk_messageBox -icon warning -type ok -title GNED \
            -message "One canvas must always be open."
         return
    }

    # switch to the other canvas
    switchToCanvas $newid

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
