#==========================================================================
#  NETWPROPS.TCL -
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


proc editNetworkProps {key} {
    global gned ned canvas

    # create dialog with OK and Cancel buttons
    createOkCancelDialog .netprops "Network Properties"

    # add notebook pages
    notebook .netprops.f.nb
    pack .netprops.f.nb -expand 1 -fill both
    notebook_addpage .netprops.f.nb general "General"
    notebook_addpage .netprops.f.nb parameters "Parameters"

    label-entry .netprops.f.nb.general.name  "Name:"
    label-entry .netprops.f.nb.general.type  "Type:"
    label-entry .netprops.f.nb.general.like "Like:"
    label-check .netprops.f.nb.general.link "Type link:"  "Define type if not exist" def
    label-entry .netprops.f.nb.general.on    "On:"
    label-text  .netprops.f.nb.general.comment "Comments:" 2

    label-text  .netprops.f.nb.parameters.params "Parameters:" 7

    pack .netprops.f.nb.general.name  -expand 0 -fill x -side top
    pack .netprops.f.nb.general.type  -expand 0 -fill x -side top
    pack .netprops.f.nb.general.like  -expand 0 -fill x -side top
    pack .netprops.f.nb.general.link  -expand 0 -fill x -side top
    pack .netprops.f.nb.general.on  -expand 0 -fill x -side top
    pack .netprops.f.nb.general.comment  -expand 0 -fill x -side top

    pack .netprops.f.nb.parameters.params  -expand 0 -fill x -side top

    set typename $ned($key,type-name)
    if {$ned($key,like-name)!=""} {
        if {[itemKeyFromName $ned($key,like-name) module]==""} {
            set ned(def) 0
        } else { set ned(def) 1}
    } else {
       if {$typename!="" && [itemKeyFromName $typename module]==""} {
         set ned(def) 0
       } else {set ned(def) 1}
    }

    .netprops.f.nb.general.name.e  insert 0 $ned($key,name)
    .netprops.f.nb.general.type.e  insert 0 $ned($key,type-name)
    .netprops.f.nb.general.like.e  insert 0 $ned($key,like-name)
    .netprops.f.nb.general.on.e    insert 0 $ned($key,on)
    .netprops.f.nb.general.comment.t  insert 1.0 $ned($key,comment)

    .netprops.f.nb.parameters.params.t insert 1.0 $ned($key,parameters)

    focus .netprops.f.nb.general.name.e

    # exec the dialog, extract its contents if OK was pressed, then delete dialog
    if {[execOkCancelDialog .netprops] == 1} {
       set ned($key,name)       [.netprops.f.nb.general.name.e get]
       set typename             [.netprops.f.nb.general.type.e get]
       set likename             [.netprops.f.nb.general.like.e get]
       set ned($key,on)         [.netprops.f.nb.general.on.e   get]
       set ned($key,comment)    [.netprops.f.nb.general.comment.t get 1.0 end]

       set ned($key,parameters) [.netprops.f.nb.parameters.params.t get 1.0 end]

#       set ned($key,type-name) $typename
#       if {$typename!="" && [itemKeyFromName $typename module]==""} {
#            if {$ned(def) == 1} {
#                set modk [addItem module]
#                set ned($modk,name) $typename
#                set ned($modk,parentkey) $ned($key,parentkey)
#                openModuleOnNewCanvas $modk
#            }
#       }

       if {$likename!=""} {
           set ned($key,like-name) $likename
           set ned($key,type-name) $typename
           if {[itemKeyFromName $likename module]==""} {
              if {$ned(def) == 1} {
                  set modk [addItem module]
                  set ned($modk,name) $likename
                  set ned($modk,parentkey) $ned($key,parentkey)
                  openModuleOnNewCanvas $modk
              }
           }
       } else {
           set ned($key,like-name) $likename
           set ned($key,type-name) $typename
           if {$typename!="" && [itemKeyFromName $typename module]==""} {
               if {$ned(def) == 1} {
                   set modk [addItem module]
                   set ned($modk,name) $typename
                   set ned($modk,parentkey) $ned($key,parentkey)
                   openModuleOnNewCanvas $modk
               }
           }
       }
    }
    destroy .netprops
    updateTreeManager
}


