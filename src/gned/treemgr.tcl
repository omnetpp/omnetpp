#==========================================================================
#  TREEMGR.TCL -
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
            treemanagerPopup $key %X %Y
        }
    }

    #
    # bindings for the resize bar
    #
    bind $gned(manager).resize <Button-1> {
        global mouse
        set mouse(x) %x
    }

    bind $gned(manager).resize <ButtonRelease-1> {
        global mouse
        set dx [expr %x-$mouse(x)]

        set width [$gned(manager).tree cget -width]
        set width [expr $width+$dx]
        $gned(manager).tree config -width $width
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

proc treemanagerDoubleClick {key} {
    global ned

    set type $ned($key,type)
    if {$type=="module"} {
        openModuleOnNewCanvas $key
    } else {
        tk_messageBox -icon warning -type ok \
            -message "Opening a '$type' on canvas is not implemented yet."
    }
}

proc treemanagerPopup {key x y} {
    global ned

    catch {destroy .popup}
    menu .popup -tearoff 0
    switch $ned($key,type) {
        nedfile {nedfilePopup $key}
        module  {modulePopup $key}
        default {defaultPopup $key}
    }
    .popup post $x $y
}

proc nedfilePopup {key} {
    global ned
    # FIXME:
    foreach i {
      {command -command "editProps $key" -label {Save} -underline 0}
      {command -command "editProps $key" -label {Close} -underline 0}
      {separator}
      {command -command "deleteItem $key; updateTreeManager" -label {Delete} -underline 0}
    } {
       eval .popup add $i
    }
}

proc modulePopup {key} {
    global ned
    # FIXME:
    foreach i {
      {command -command "editProps $key" -label {Open on canvas} -underline 1}
      {command -command "editProps $key" -label {Close its canvas} -underline 1}
      {separator}
      {command -command "deleteItem $key; updateTreeManager" -label {Delete} -underline 0}
    } {
       eval .popup add $i
    }
}

proc defaultPopup {key} {
    global ned
    # FIXME:
    foreach i {
      {command -command "deleteItem $key; updateTreeManager" -label {Delete} -underline 0}
    } {
       eval .popup add $i
    }
}

#-------------- temp solution: -----------------

set files [glob -nocomplain -- {icons/*_vs.gif}]
foreach f $files {
  set name [string tolower [file tail [file rootname $f]]]
  if [catch {image type $name}] {
     puts -nonewline "$name "
     image create photo $name -file $f
  }
}
puts ""


# getNodeInfo --
#
# This user-supplied function gets called by the tree widget to get info about
# tree nodes. The widget itself only stores the state (open/closed) of the
# nodes, everything else comes from this function.
#
proc getNodeInfo {w op {key {}}} {
    global ned ddesc

    switch $op {

      root {
        return 0
      }

      text {
        #DBG:
        # return "$key:$ned($key,type):($ned($key,childrenkeys))"

        if [info exist ned($key,name)] {
          return "$ned($key,type) $ned($key,name)"
        } else {
          return "$ned($key,type)"
        }
      }

      icon {
        set type $ned($key,type)
        if [info exist ddesc($type,treeicon)] {
          return $ddesc($type,treeicon)
        } else {
          return $ddesc(root,treeicon)
        }
      }

      parent {
        return $ned($key,parentkey)
      }

      children {
        # FIXME: ordering!
        return $ned($key,childrenkeys)
      }

      haschildren {
        # DBG:
        return [expr [llength $ned($key,childrenkeys)]!=0]

        set type $ned($key,type)
        if {$type=="root" || $type=="nedfile"} {
          return [expr [llength $ned($key,childrenkeys)]!=0]
        } else {
          return 0
        }
      }
    }
}


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
