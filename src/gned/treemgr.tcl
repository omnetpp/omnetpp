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
    global gned

    Tree:init $gned(manager).tree

    #
    # bindings for the tree
    #
    bind $gned(manager).tree <Button-1> {
        catch {destroy .popup}
        set key [Tree:nodeat %W %x %y]
        if {$key!=""} {
            Tree:setselection %W $key
        }
        dragAndDropStart $key %X %Y
    }

    bind $gned(manager).tree <B1-Motion> {
        dragAndDropMotion %X %Y
    }

    bind $gned(manager).tree <ButtonRelease-1> {
        dragAndDropFinish %X %Y
    }

    bind $gned(manager).tree <Double-1> {
        set key [Tree:nodeat %W %x %y]
        if {$key!=""} {
            # Tree:toggle %W $key
            treemanagerDoubleClick $key
        }
    }

    bind $gned(manager).tree <Button-3> {
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
    global gned

    Tree:build $gned(manager).tree
}

# getNodeInfo --
#
# This user-supplied function gets called by the tree widget to get info about
# tree nodes. The widget itself only stores the state (open/closed) of the
# nodes, everything else comes from this function.
#
proc getNodeInfo {w op {key {}}} {
    global ned ned_desc icons

    switch $op {

      root {
        return 0
      }

      text {
        return [itemLabel $key]
      }

      options {
        if [info exist ned($key,aux-isdirty)] {
          if {$ned($key,aux-isdirty)} {
             return "-fill #ff0000"
          } else {
             return ""
          }
        }
      }

      icon {
        set type $ned($key,type)
        if [info exist ned_desc($type,treeicon)] {
          return $icons($ned_desc($type,treeicon))
        } else {
          return $icons($ned_desc(root,treeicon))
        }
      }

      parent {
        return $ned($key,parentkey)
      }

      children {
        return $ned($key,childrenkeys)
      }

      haschildren {
        return [expr [llength $ned($key,childrenkeys)]!=0]

        ## OLD CODE: only allow top-level components (modules, channels etc.) to be displayed
        # set type $ned($key,type)
        # if {$type=="root" || $type=="nedfile"} {
        #   return [expr [llength $ned($key,childrenkeys)]!=0]
        # } else {
        #   return 0
        #}
      }
    }
}


#------------------------------
# Bindings for the tree manager
#------------------------------

proc treemanagerDoubleClick {key} {
    global ned gned

    if {$ned($key,type)=="nedfile"} {
        Tree:toggle $gned(manager).tree $key
    } elseif {$ned($ned($key,parentkey),type)=="nedfile"} {
        # top-level item
        if {$ned($key,type)=="module"} {
            openModuleOnCanvas $key
        } else {
            editProps $key
        }
    } else {
        Tree:toggle $gned(manager).tree $key
    }
}

proc treemanagerPopup {key x y} {
    global ned

    catch {destroy .popup}
    menu .popup -tearoff 0

    if {$ned($key,type)=="nedfile"} {
        nedfilePopup $key
    } elseif {$ned($ned($key,parentkey),type)=="nedfile"} {
        toplevelComponentPopup $key
    } else {
        defaultPopup $key
    }
    .popup post $x $y
}

proc nedfilePopup {key} {
    global ned

    foreach i {
      {cascade -menu .popup.newmenu -label {New} -underline 0}
      {separator}
      {command -command "fileSave $key" -label {Save} -underline 0}
      {command -command "fileSaveAs $key" -label {Save As...} -underline 1}
      {command -command "fileCloseNedfile $key" -label {Close} -underline 0}
      {separator}
      {command -command "displayCodeForItem $key" -label {Show NED code...} -underline 0}
      {separator}
      {command -command "moveUpItem $key; updateTreeManager" -label {Move up} -underline 5}
      {command -command "moveDownItem $key; updateTreeManager" -label {Move down} -underline 5}
    } {
       eval .popup add $i
    }

    menu .popup.newmenu -tearoff 0
    foreach i {
      {command -command "fileNewComponent imports $key" -label {Import} -underline 0}
      {command -command "fileNewComponent channel $key" -label {Channel} -underline 0}
      {command -command "fileNewComponent simple $key"  -label {Simple module}  -underline 0}
      {command -command "fileNewComponent module $key"  -label {Compound module}  -underline 0}
      {command -command "fileNewComponent network $key" -label {Network} -underline 0}
    } {
       eval .popup.newmenu add $i
    }
}

proc toplevelComponentPopup {key} {
    global ned

    .popup add command -command "editProps $key" -label {Properties...} -underline 0
    if {$ned($key,type)=="module"} {
      .popup add command -command "openModuleOnCanvas $key" -label {Open on canvas} -underline 0
    }
    .popup add command -command "displayCodeForItem $key" -label {Show NED code...} -underline 0
    if {$ned($key,type)=="module" || $ned($key,type)=="simple"} {
       .popup add separator
       .popup add command -command "createSubmoduleOnCanvas $key" -label {Drop an instance onto the canvas} -underline 1
    }
    foreach i {
      {separator}
      {command -command "moveUpItem $key; updateTreeManager" -label {Move up} -underline 5}
      {command -command "moveDownItem $key; updateTreeManager" -label {Move down} -underline 5}
      {separator}
      {command -command "markNedFileOfItemDirty $key; deleteItem $key; updateTreeManager" -label {Delete}}
    } {
       eval .popup add $i
    }
}

proc defaultPopup {key} {
    global ned

    foreach i {
      {command -command "displayCodeForItem $key" -label {Show NED fragment...} -underline 0}
      {separator}
      {command -command "moveUpItem $key; updateTreeManager" -label {Move up} -underline 5}
      {command -command "moveDownItem $key; updateTreeManager" -label {Move down} -underline 5}
      {separator}
      {command -command "markNedFileOfItemDirty $key; deleteItem $key; updateTreeManager" -label {Delete}}
    } {
       eval .popup add $i
    }
}

proc moveUpItem {key} {
    global ned

    set parentkey $ned($key,parentkey)
    set l $ned($parentkey,childrenkeys)

    set pos [lsearch -exact $l $key]
    if {$pos>0} {
        # swap with prev item
        set prevpos [expr $pos-1]
        set prevkey [lindex $l $prevpos]
        set ned($parentkey,childrenkeys) \
            [lreplace $l $prevpos $pos $key $prevkey]

        # nedfile changed
        if {$parentkey!=0} {
            markNedFileOfItemDirty $parentkey
            updateTreeManager
        }
    }
}

proc moveDownItem {key} {
    global ned

    set parentkey $ned($key,parentkey)
    set l $ned($parentkey,childrenkeys)

    set pos [lsearch -exact $l $key]
    if {$pos<[expr [llength $l]-1]} {
        # swap with next item
        set nextpos [expr $pos+1]
        set nextkey [lindex $l $nextpos]
        set ned($parentkey,childrenkeys) \
            [lreplace $l $pos $nextpos $nextkey $key]

        # nedfile changed
        if {$parentkey!=0} {
            markNedFileOfItemDirty $parentkey
            updateTreeManager
        }
    }
}

proc moveItemToTop {key} {
    global ned

    set parentkey $ned($key,parentkey)
    set l $ned($parentkey,childrenkeys)

    set pos [lsearch -exact $l $key]
    if {$pos>0} {
        # delete, and insert as first
        set l [lreplace $l $pos $pos]
        set l [concat [list $key] $l]
        set ned($parentkey,childrenkeys) $l

        # nedfile changed
        if {$parentkey!=0} {
            markNedFileOfItemDirty $parentkey
            updateTreeManager
        }
    }
}

proc displayCodeForItem {key {parentwin {}}} {
    global ned fonts

    if [info exist ned($key,name)] {
        set txt "$ned($key,type) $ned($key,name)"
    } else {
        set txt "$ned($key,type)"
    }


    # open file viewer/editor window
    set w $parentwin.nedcode
    catch {destroy $w}

    # create widgets
    toplevel $w -class Toplevel
    wm focusmodel $w passive
    #wm maxsize $w 1009 738
    wm minsize $w 1 1
    wm overrideredirect $w 0
    wm resizable $w 1 1
    wm title $w "NED code -- $txt"

    frame $w.main
    scrollbar $w.main.sb -borderwidth 1 -command "$w.main.text yview"
    pack $w.main.sb -anchor center -expand 0 -fill y -side right
    text $w.main.text -yscrollcommand "$w.main.sb set" \
         -wrap none -font $fonts(fixed) -relief sunken -bg #a0e0a0
    pack $w.main.text -anchor center -expand 1 -fill both -side left

    frame $w.butt
    button $w.butt.close -text Close -command "destroy $w"
    pack $w.butt.close -anchor n -side right -expand 0

    pack $w.butt -expand 0 -fill x -side bottom -padx 5 -pady 5
    pack $w.main -expand 1 -fill both -side top -padx 5 -pady 5
    focus $w.main.text

    # produce ned code and put it into text widget
    set nedcode [generateNed $key]
    $w.main.text insert end $nedcode
    $w.main.text mark set insert 1.0
    syntaxHighlight $w.main.text 1.0 end

    # set dimensions and disable widget (one can't insert text into a disabled
    # widget even from program)
    set numlines [lindex [split [$w.main.text index end] "."] 0]
    set height [expr $numlines>28 ? 30 : $numlines+2]
    $w.main.text configure -state disabled -height $height -width 60

    # bindings
    bind $w <Key-Escape> "destroy $w"
    focus $w.butt.close
}


# itemLabel --
#
# Produce item label for treeview.
#
proc itemLabel {key} {
    global ned

    if {$ned($key,type)=="conn"} {
        # src and dest module and gate indices
        set src_index ""
        set dest_index ""
        set src_gate_index ""
        set dest_gate_index ""
        if {$ned($key,src-mod-index)!=""}   {set src_index "\[$ned($key,src-mod-index)\]"}
        if {$ned($key,dest-mod-index)!=""}  {set dest_index "\[$ned($key,dest-mod-index)\]"}
        if {$ned($key,src-gate-index)!=""}  {set src_gate_index "\[$ned($key,src-gate-index)\]"}
        if {$ned($key,dest-gate-index)!=""} {set dest_gate_index "\[$ned($key,dest-gate-index)\]"}

        # src and dest gates
        if {$ned($ned($key,src-ownerkey),type)=="module"} {
            set src "$ned($key,srcgate)$src_gate_index"
        } else {
            set src "$ned($ned($key,src-ownerkey),name)$src_index.$ned($key,srcgate)$src_gate_index"
        }
        if {$ned($ned($key,dest-ownerkey),type)=="module"} {
            set dest "$ned($key,destgate)$dest_gate_index"
        } else {
            set dest "$ned($ned($key,dest-ownerkey),name)$dest_index.$ned($key,destgate)$dest_gate_index"
        }

        # direction
        if {$ned($key,arrowdir-l2r)} {
            set arrow "-->"
        } else {
            set arrow "<--"

            set tmp $src
            set src $dest
            set dest $tmp
        }
        return "conn $src $arrow $dest"

    } elseif {$ned($key,type)=="submod"} {
        set str "$ned($key,name): $ned($key,type-name)"
        if {$ned($key,vectorsize)!=""} {
            append str "\[$ned($key,vectorsize)\]"
        }
        if {$ned($key,like-name)!=""} {
            append str " like $ned($key,like-name)"
        }
        return $str

    } elseif {[info exist ned($key,name)] && [info exist ned($key,vectorsize)] && $ned($key,vectorsize)!=""} {
        return "$ned($key,type) $ned($key,name)\[$ned($key,vectorsize)\]"
    } elseif {[info exist ned($key,name)] && [info exist ned($key,size)] && $ned($key,size)!=""} {
        return "$ned($key,type) $ned($key,name)\[$ned($key,size)\]"
    } elseif {[info exist ned($key,name)] && [info exist ned($key,isvector)] && $ned($key,isvector)} {
        return "$ned($key,type) $ned($key,name)\[\]"
    } elseif {[info exist ned($key,name)] && [info exist ned($key,value)]} {
        return "$ned($key,type) $ned($key,name) = $ned($key,value)"
    } elseif [info exist ned($key,name)] {
        return "$ned($key,type) $ned($key,name)"
    } else {
        return "$ned($key,type)"
    }
}
