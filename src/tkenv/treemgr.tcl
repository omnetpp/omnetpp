#==========================================================================
#  TREEMGR.TCL -
#            part of the Tkenv, the Tcl/Tk graphical topology editor of
#                            OMNeT++
#
#   By Andras Varga
#
#==========================================================================

#----------------------------------------------------------------#
#  Copyright (C) 1992-2003 Andras Varga
#
#  This file is distributed WITHOUT ANY WARRANTY. See the file
#  `license' for details on this and other legal matters.
#----------------------------------------------------------------#


# icons used in the tree view
set treeicons(cCompoundModule) {compound_vs}
set treeicons(cSimpleModule)   {simple_vs}
set treeicons(cMessage)        {message_vs}
set treeicons(cQueue)          {queue_vs}
set treeicons(cLinkedList)     {queue_vs}
set treeicons(cArray)          {container_vs}
set treeicons(cHead)           {container_vs}
set treeicons(cBag)            {container_vs}
set treeicons(cMessageHeap)    {container_vs}
set treeicons(cChannel)        {chan_vs}
set treeicons(cStatistic)      {stat_vs}
set treeicons(cOutVector)      {outvect_vs}


# initTreeManager --
#
#
proc initTreeManager {} {
    global widgets

    Tree:init $widgets(manager).tree

    #
    # bindings for the tree
    #
    bind $widgets(manager).tree <Button-1> {
        catch {destroy .popup}
        # $key is the object pointer -- updateTreeManager is necessary to
        # absolutely rule out segfault by accessing an invalid pointer!
        updateTreeManager
        set key [Tree:nodeat %W %x %y]
        if {$key!=""} {
            Tree:setselection %W $key
        }
    }

    bind $widgets(manager).tree <Double-1> {
        # $key is the object pointer -- updateTreeManager is necessary to
        # absolutely rule out segfault by accessing an invalid pointer!
        updateTreeManager
        set key [Tree:nodeat %W %x %y]
        if {$key!=""} {
            # Tree:toggle %W $key
            treemanagerDoubleClick $key
        }
    }

    bind $widgets(manager).tree <Button-3> {
        # $key is the object pointer -- updateTreeManager is necessary to
        # absolutely rule out segfault by accessing an invalid pointer!
        updateTreeManager
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
proc getNodeInfo {w op {key {}}} {
    global icons treeicons

    # $key is the object pointer
    set ptr $key

    switch $op {
      root {
        return [opp_object_simulation]
      }

      text {
        return "[opp_getobjectfullname $ptr] ([opp_getobjectclassname $ptr])"
      }

      options {
        return ""
      }

      icon {
        set class [opp_getobjectbaseclass $key]
        if [info exists treeicons($class)] {
           return $icons($treeicons($class))
        } else {
           return $icons(cogwheel_vs)
        }
      }

      children {
        return [opp_getchildobjects $ptr]
      }

      haschildren {
        return [expr [llength [opp_getchildobjects $ptr]]!=0]
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



