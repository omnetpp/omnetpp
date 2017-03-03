#=================================================================
#  TREEMGR.TCL - part of
#
#                     OMNeT++/OMNEST
#            Discrete System Simulation in C++
#
#=================================================================

#----------------------------------------------------------------#
#  Copyright (C) 1992-2017 Andras Varga
#
#  This file is distributed WITHOUT ANY WARRANTY. See the file
#  `license' for details on this and other legal matters.
#----------------------------------------------------------------#



# initTreeManager --
#
#
proc initTreeManager {} {
    global widgets
    global B2 B3

    Tree:init $widgets(manager).tree treeManager:getNodeInfo

    #
    # bindings for the tree
    #
    bind $widgets(manager).tree <Double-1> {
        focus %W
        #treeManager:update
        set key [Tree:nodeat %W %x %y]
        if {$key!=""} {
            # Tree:toggle %W $key
            treeManager:doubleClick $key
        }
    }

    bind $widgets(manager).tree <Button-$B3> {
        focus %W
        #treeManager:update
        set key [Tree:nodeat %W %x %y]
        if {$key!=""} {
            Tree:setselection %W $key
            treeManager:popup $key %X %Y
        }
    }
}


# treeManager:update --
#
# Redraws the manager window (left side of main window).
#
proc treeManager:update {} {
    global widgets config

    Tree:build $widgets(manager).tree
    $widgets(manager).tree xview moveto 0
}

# treeManager:getNodeInfo --
#
# This user-supplied function gets called by the tree widget to get info about
# tree nodes. The widget itself only stores the state (open/closed) of the
# nodes, everything else comes from this function.
#
# We use the object pointer as tree element key.
#
proc treeManager:getNodeInfo {w op {key {}}} {
    global icons

    set ptr $key
    switch $op {

      text {
        set id [opp_getobjectid $ptr]
        if {$id!=""} {set id " (id=$id)"}
        return "[opp_getobjectfullname $ptr] ([opp_getobjectshorttypename $ptr])$id"
      }

      needcheckbox {
        return 0
      }

      options {
        return ""
      }

      icon {
        return [opp_getobjecticon $ptr]
      }

      haschildren {
        return [opp_haschildobjects $ptr]
      }

      children {
        return [opp_getchildobjects $ptr]
      }

      root {
        return [opp_object_simulation]
      }

      selectionchanged {
        mainWindow:selectionChanged "" $ptr
      }
    }
}


#------------------------------
# Bindings for the tree manager
#------------------------------

proc treeManager:doubleClick {key} {
    # $key is the object pointer
    set ptr $key
    set base [opp_getobjectbaseclass $ptr]
    if {$base == "cModule" || $base == "cSimpleModule"} {
        opp_inspector_setobject .network $ptr
    } else {
        opp_inspect $ptr
    }
}

proc treeManager:popup {key x y} {
    global ned

    # $key is the object pointer
    set ptr $key
    set popup [createInspectorContextMenu "" $ptr]
    tk_popup $popup $x $y
}

