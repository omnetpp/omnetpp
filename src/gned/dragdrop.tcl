#==========================================================================
#  DRAGDROP.TCL -
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


set defaultparvalue(string) {""}
set defaultparvalue(numeric) 1
set {defaultparvalue(numeric const)} 1


proc dragAndDropStart {typekey x y} {
    global mouse ned

    # only compound/simple modules can be dragged
    set mouse(typekey) ""
    if {$typekey!=""} {
       set type $ned($typekey,type)
       if {$type=="module" || $type=="simple"} {
          set mouse(typekey) $typekey
       }
    }
    set mouse(dragging) 0
    set mouse(startingX) $x
    set mouse(startingY) $y
}


proc dragAndDropMotion {x y} {
    global mouse icons

    if {$mouse(typekey)==""} {
       return
    } elseif {$mouse(dragging)} {
       # move window
       catch {wm geometry .draggedIcon "+[expr $x-10]+[expr $y-7]"}
    } elseif {abs($x-$mouse(startingX))>5 || abs($y-$mouse(startingY))>5} {
       # create window which will be dragged
       set mouse(dragging) 1
       catch {destroy .draggedIcon}
       toplevel .draggedIcon -relief solid -bd 2 -bg #8080ff
       wm overrideredirect .draggedIcon true
       wm positionfrom .draggedIcon program
       wm geometry .draggedIcon "20x14+[expr $x-10]+[expr $y-7]"
    }
}


proc dragAndDropFinish {x y} {
    global mouse gned canvas

    # was it drag&drop?
    if {$mouse(typekey)=="" || !$mouse(dragging)} {
       return
    }

    # was it dropped on the canvas?
    catch {destroy .draggedIcon}
    set w [winfo containing $x $y]
    set canv_id $gned(canvas_id)
    if {$w!=$canvas($canv_id,canvas)} {

       tk_messageBox -type ok -icon info -title GNED \
                     -message "You can only drop simple and compound modules on the canvas."
       return

       ## animate cancel... (code doesn't work)
       #if {$mouse(dragging)} {
       #   set n 30
       #   set dx [expr ($x-$mouse(startingX))/$n]
       #   set dy [expr ($y-$mouse(startingY))/$n]
       #   for {set i 0} {$i<$n} {incr i} {
       #      wm geometry .draggedIcon "+[expr $x-$i*$dx-10]+[expr $y-$i*$dy-10]"
       #      update idletasks
       #   }
       #   destroy .draggedIcon
       #}
    }

    # create module
    set typekey $mouse(typekey)
    set canvx [expr $x - [winfo rootx $w]]
    set canvy [expr $y - [winfo rooty $w]]

    createSubmoduleOnCanvas $typekey $canvx $canvy
}


# createSubmoduleOnCanvas --
#
# Create a new submodule on the current canvas.
# Called when the user drags a module type on the canvas
# (dragAndDropFinish).
#
proc createSubmoduleOnCanvas {typekey {canvx {}} {canvy {}}} {
    global ned defaultparvalue gned canvas

    if {$canvx==""} {set canvx [expr 100+100*rand()]}
    if {$canvy==""} {set canvy [expr 100+100*rand()]}

    # find parent module key
    set modkey $canvas($gned(canvas_id),module-key)

    # find submods
    set submodskey [getChildrenWithType $modkey submods]
    if {[llength $submodskey]==0} {
        set submodskey [addItem submods $modkey]
    } elseif {[llength $submodskey]>1} {
        error "Internal error: more than one 'submods'"
    }

    # create submodule
    set key [addItem submod $submodskey]

    # set type
    set ned($key,type-name) $ned($typekey,name)

    # find a default name
    set name $ned($typekey,name)
    if [regexp {^[A-Z][a-z0-9_]} $name] {
        set name "[string tolower [string range $name 0 0]][string range $name 1 end]"
    }
    renameItem $key $name

    # add parameters
    set parskey [getChildrenWithType $typekey params]
    if {$parskey!=""} {
        set sparskey [addItem substparams $key]
        foreach parkey [getChildrenWithType $parskey param] {
            set sparkey [addItem substparam $sparskey]
            set ned($sparkey,name) $ned($parkey,name)
            catch {set ned($sparkey,value) $defaultparvalue($ned($parkey,datatype))}
            set ned($sparkey,right-comment) " TBD\n"
            # copy over comments too?
        }
    }

    # add gatesizes
    set gateskey [getChildrenWithType $typekey gates]
    if {$gateskey!=""} {
        set gsizkey {}
        foreach gkey [getChildrenWithType $gateskey gate] {
            if {$ned($gkey,isvector)} {
                if {$gsizkey==""} {
                   set gsizkey [addItem gatesizes $key]
                }
                set gskey [addItem gatesize $gsizkey]
                set ned($gskey,name) $ned($gkey,name)
                set ned($gskey,size) 1
                set ned($gskey,right-comment) " TBD\n"
                # copy over comments too?
            }
        }
    }

    # add machines
    set machskey [getChildrenWithType $typekey machines]
    if {$machskey!=""} {
        set smachskey [addItem substmachines $key]
        foreach machkey [getChildrenWithType $machskey machine] {
            set smachkey [addItem substmachine $smachskey]
            set ned($smachkey,value) $ned($machkey,name)
            set ned($smachkey,right-comment) " TBD\n"
            # copy over comments too?
        }
    }

    # set coords
    set x1 [expr $canvx-20]
    set y1 [expr $canvy-14]
    set x2 [expr $canvx+20]
    set y2 [expr $canvy+14]

    set ned($key,x-pos)  [expr int(($x1+$x2)/2)]
    set ned($key,y-pos)  [expr int(($y1+$y2)/2)]
    set ned($key,x-size) [expr int($x2-$x1)]
    set ned($key,y-size) [expr int($y2-$y1)]

    # draw on canvas
    drawItem $key
    selectItem $key
    markCanvasDirty
}


