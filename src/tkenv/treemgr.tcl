#=================================================================
#  TREEMGR.TCL - part of
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



# initTreeManager --
#
#
proc initTreeManager {} {
    global widgets

    Tree:init $widgets(manager).tree getNodeInfo

    #
    # bindings for the tree
    #
    bind $widgets(manager).tree <Button-1> {
        catch {destroy .popup}
        focus %W
        #updateTreeManager
        set key [Tree:nodeat %W %x %y]
        if {$key!=""} {
            Tree:setselection %W $key
        }
    }

    bind $widgets(manager).tree <Double-1> {
        focus %W
        #updateTreeManager
        set key [Tree:nodeat %W %x %y]
        if {$key!=""} {
            # Tree:toggle %W $key
            treemanagerDoubleClick $key
        }
    }

    bind $widgets(manager).tree <Button-3> {
        focus %W
        #updateTreeManager
        set key [Tree:nodeat %W %x %y]
        if {$key!=""} {
            Tree:setselection %W $key
            treemanagerPopup $key %X %Y
        }
    }
}


# updateTreeManager --
#
# Redraws the manager window (left side of main window).
#
proc updateTreeManager {} {
    global widgets config

    # spare work if we're not displayed
    if {$config(display-treeview)==0} {return}

    Tree:build $widgets(manager).tree
    $widgets(manager).tree xview moveto 0
}

# getNodeInfo --
#
# This user-supplied function gets called by the tree widget to get info about
# tree nodes. The widget itself only stores the state (open/closed) of the
# nodes, everything else comes from this function.
#
# We use the object pointer as tree element key.
#
proc getNodeInfo {w op {key {}}} {
    global icons

    set ptr $key
    switch $op {

      text {
        set id [opp_getobjectid $ptr]
        if {$id!=""} {set id " (id=$id)"}
        return "[opp_getobjectfullname $ptr] ([opp_getobjectclassname $ptr])$id"
      }

      options {
        return ""
      }

      icon {
        return [get_icon_for_object $ptr]
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
    }
}


#------------------------------
# Bindings for the tree manager
#------------------------------

proc treemanagerDoubleClick {key} {
    # $key is the object pointer
    opp_inspect $key "(default)"
}

proc treemanagerPopup {key x y} {
    global ned

    catch {destroy .popup}
    menu .popup -tearoff 0

    # $key is the object pointer
    foreach i [opp_supported_insp_types $key] {
       .popup add command -label "Inspect $i" -command "opp_inspect $key \"$i\""
    }

    .popup post $x $y
}



