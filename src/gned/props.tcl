#==========================================================================
#  PROPS.TCL -
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

proc editProps {key} {
    global ned

    tk_messageBox -icon warning -type ok -title GNED \
       -message "Nope. Until I update the dialogs to work with the new\
                 internal data structure, you can use the NED source view\
                 if you want to edit parameters, gates etc."
    return

    if {$ned($key,type)=="module"} {
       editModuleProps $key
    } elseif {$ned($key,type)=="submod"} {
       editSubmoduleProps $key
    } elseif {$ned($key,type)=="conn"} {
       editConnectionProps $key
    }
    updateTreeManager
}

proc editModuleProps {key} {
    global gned ned

    # create dialog with OK and Cancel buttons
    createOkCancelDialog .modprops "Module Properties"

    # add notebook pages
    notebook .modprops.f.nb
    pack .modprops.f.nb -expand 1 -fill both
    notebook_addpage .modprops.f.nb general "General"
    notebook_addpage .modprops.f.nb pars  "Parameters"
    notebook_addpage .modprops.f.nb gates "Gates"
    notebook_addpage .modprops.f.nb mach  "Machines"

    # add entry fields
    label-entry .modprops.f.nb.general.name "Name:"
    label-text  .modprops.f.nb.general.comment "Comments:" 5
    pack .modprops.f.nb.general.name  -expand 0 -fill x -side top
    pack .modprops.f.nb.general.comment -expand 1 -fill both -side top

    label-text  .modprops.f.nb.pars.pars "Parameters:" 7
    pack .modprops.f.nb.pars.pars  -expand 1 -fill both -side top

    label-text  .modprops.f.nb.gates.gates "Gates:" 7
    pack .modprops.f.nb.gates.gates -expand 1 -fill both -side top

    label-entry .modprops.f.nb.mach.name "Host:"
    pack .modprops.f.nb.mach.name  -expand 0 -fill x -side top

    focus .modprops.f.nb.general.name.e

    .modprops.f.nb.general.name.e insert 0 $ned($key,name)
    .modprops.f.nb.general.comment.t insert 1.0 $ned($key,comment)
    .modprops.f.nb.pars.pars.t insert 1.0 $ned($key,parameters)
    .modprops.f.nb.gates.gates.t insert 1.0 $ned($key,gates)
    .modprops.f.nb.mach.name.e insert 0 $ned($key,machines)

    # exec the dialog, extract its contents if OK was pressed, then delete dialog
    if {[execOkCancelDialog .modprops] == 1} {
        set ned($key,comment) [.modprops.f.nb.general.comment.t get 1.0 end]
        set ned($key,parameters) [.modprops.f.nb.pars.pars.t get 1.0 end]
        set ned($key,gates) [.modprops.f.nb.gates.gates.t get 1.0 end]
        set ned($key,machines) [.modprops.f.nb.mach.name.e get]

        if {$ned($key,name)!=[.modprops.f.nb.general.name.e get]} {
           foreach i [array names ned "*,is-submod"] {
             regsub -- ",is-submod" $i "" mkey
             if {$ned($mkey,type-name)== $ned($key,name)} {
                if {$ned($ned($mkey,module-ownerkey),parentkey) == $ned($key,parentkey)} {
                   set ned($mkey,type-name) [.modprops.f.nb.general.name.e get]
                }
             }
           }
        }
        redrawItem $key
        set ned($key,name) [.modprops.f.nb.general.name.e get]
        $gned(tab) config -text $ned($key,name)
        $gned(canvas) itemconfigure $ned($key,label-cid) -text $ned($key,name)
        adjustWindowTitle
   }
    destroy .modprops
}

proc editSubmoduleProps {key} {
    global gned ned

    # create dialog with OK and Cancel buttons
    createOkCancelDialog .submprops "Submodule Properties"

    # add notebook pages
    notebook .submprops.f.nb
    pack .submprops.f.nb -expand 1 -fill both
    notebook_addpage .submprops.f.nb general "General"
    notebook_addpage .submprops.f.nb pars "Parameters"
    notebook_addpage .submprops.f.nb gates "Gate sizes"

    # add entry fields and focus on first one
    label-entry .submprops.f.nb.general.name "Name:"
    label-entry .submprops.f.nb.general.type "Type:"
    label-entry .submprops.f.nb.general.like "Like:"
    label-check .submprops.f.nb.general.link "Type link:"  "Define type if not exist" def
    label-entry .submprops.f.nb.general.vs "Vector size:"
    label-entry .submprops.f.nb.general.on "On:"
    label-text  .submprops.f.nb.general.comment "Comments:" 4

    pack .submprops.f.nb.general.name  -expand 0 -fill x -side top
    pack .submprops.f.nb.general.type  -expand 0 -fill x -side top
    pack .submprops.f.nb.general.like  -expand 0 -fill x -side top
    pack .submprops.f.nb.general.link -expand 0 -fill x -side top
    pack .submprops.f.nb.general.vs  -expand 0 -fill x -side top
    pack .submprops.f.nb.general.on  -expand 0 -fill x -side top
    pack .submprops.f.nb.general.comment -expand 1 -fill both -side top

    label-text  .submprops.f.nb.pars.pars "Parameters:" 7
    pack .submprops.f.nb.pars.pars  -expand 1 -fill both -side top

    label-text  .submprops.f.nb.gates.gsiz "Gate sizes:" 7
    pack .submprops.f.nb.gates.gsiz -expand 1 -fill both -side top

    focus .submprops.f.nb.general.name.e

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

    .submprops.f.nb.general.name.e insert 0 $ned($key,name)
    .submprops.f.nb.general.type.e insert 0 $ned($key,type-name)
    .submprops.f.nb.general.like.e insert 0 $ned($key,like-name)
    .submprops.f.nb.general.vs.e insert 0 $ned($key,vectorsize)
    .submprops.f.nb.general.on.e insert 0 $ned($key,on)
    .submprops.f.nb.general.comment.t insert 1.0 $ned($key,comment)
    .submprops.f.nb.pars.pars.t insert 1.0 $ned($key,parameters)
    .submprops.f.nb.gates.gsiz.t insert 1.0 $ned($key,gatesizes)

    focus .submprops.f.nb.general.name.e

    # exec the dialog, extract its contents if OK was pressed, then delete dialog
    if {[execOkCancelDialog .submprops] == 1} {
        set ned($key,name) [.submprops.f.nb.general.name.e get]
        set typename [.submprops.f.nb.general.type.e get]
        set likename [.submprops.f.nb.general.like.e get]
        set ned($key,vectorsize) [.submprops.f.nb.general.vs.e get]
        set ned($key,on) [.submprops.f.nb.general.on.e get]
        set ned($key,comment) [.submprops.f.nb.general.comment.t get 1.0 end]
        set ned($key,parameters) [.submprops.f.nb.pars.pars.t get 1.0 end]
        set ned($key,gatesizes) [.submprops.f.nb.gates.gsiz.t get 1.0 end]
        $gned(canvas) itemconfigure $ned($key,label-cid) -text $ned($key,name)

        if {$likename!=""} {
           set ned($key,like-name) $likename
           set ned($key,type-name) $typename
           if {[itemKeyFromName $likename module]==""} {
              if {$ned(def) == 1} {
                  set modk [addItem module]
                  set ned($modk,name) $likename
                  set ned($modk,parentkey) $ned($ned($key,module-ownerkey),parentkey)
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
                   set ned($modk,parentkey) $ned($ned($key,module-ownerkey),parentkey)
                   openModuleOnNewCanvas $modk
               }
           }
        }
    }
    destroy .submprops
}

proc editConnectionProps {key} {
    global gned ned canvas

    # create dialog with OK and Cancel buttons
    createOkCancelDialog .connprops "Connection Properties"

    # add notebook pages
    notebook .connprops.f.nb
    pack .connprops.f.nb -expand 1 -fill both
    notebook_addpage .connprops.f.nb gates "Gates"
    notebook_addpage .connprops.f.nb props "Properties"
    notebook_addpage .connprops.f.nb loop  "Loop"

    # add entry fields for Gates
    label-entry .connprops.f.nb.gates.srcgate "From gate:"
    label-entry .connprops.f.nb.gates.destgate "To gate:"
    label-entry .connprops.f.nb.gates.condition "Condition:"
    label-text  .connprops.f.nb.gates.comment "Comments:" 2
    # Properties
    label-entry .connprops.f.nb.props.error "Bit error rate:"
    label-entry .connprops.f.nb.props.delay "Prop. delay:"
    label-entry .connprops.f.nb.props.datarate "Data Rate:"
    label-entry .connprops.f.nb.props.channel "Channel name:"
    # Loop
    label-entry .connprops.f.nb.loop.src_index "Source Index:"
    label-entry .connprops.f.nb.loop.dest_index "Destination Index:"
    label-entry .connprops.f.nb.loop.src_gate_index "Source Gate Index:"
    label-entry .connprops.f.nb.loop.dest_gate_index "Destinaton Gate Index:"
    label-entry .connprops.f.nb.loop.for "For Expression:"
    # Gates
    pack .connprops.f.nb.gates.srcgate  -expand 0 -fill x -side top
    pack .connprops.f.nb.gates.destgate  -expand 0 -fill x -side top
    pack .connprops.f.nb.gates.condition  -expand 0 -fill x -side top
    pack .connprops.f.nb.gates.comment -expand 1 -fill both -side top
    # Properties
    pack .connprops.f.nb.props.error  -expand 0 -fill x -side top
    pack .connprops.f.nb.props.delay  -expand 0 -fill x -side top
    pack .connprops.f.nb.props.datarate -expand 0 -fill x -side top
    pack .connprops.f.nb.props.channel -expand 0 -fill x -side top
    # Loop
    pack .connprops.f.nb.loop.src_index  -expand 0 -fill x -side top
    pack .connprops.f.nb.loop.dest_index  -expand 0 -fill x -side top
    pack .connprops.f.nb.loop.src_gate_index  -expand 0 -fill x -side top
    pack .connprops.f.nb.loop.dest_gate_index  -expand 0 -fill x -side top
    pack .connprops.f.nb.loop.for  -expand 0 -fill x -side top

    # Gates
    .connprops.f.nb.gates.srcgate.e insert 0 $ned($key,srcgate)
    .connprops.f.nb.gates.destgate.e insert 0 $ned($key,destgate)
    .connprops.f.nb.gates.condition.e  insert 0 $ned($key,condition)
    .connprops.f.nb.gates.comment.t insert 1.0 $ned($key,comment)
    if {$ned($key,channel-ownerkey)!=""} {
        set chown $ned($key,channel-ownerkey)
       .connprops.f.nb.props.error.e insert 0 $ned($chown,error)
       .connprops.f.nb.props.delay.e insert 0 $ned($chown,delay)
       .connprops.f.nb.props.datarate.e insert 0 $ned($chown,datarate)
       .connprops.f.nb.props.channel.e insert 0 $ned($chown,name)
    } else {
        # Properties
       .connprops.f.nb.props.error.e insert 0 $ned($key,error)
       .connprops.f.nb.props.delay.e insert 0 $ned($key,delay)
       .connprops.f.nb.props.datarate.e insert 0 $ned($key,datarate)
       .connprops.f.nb.props.channel.e insert 0 $ned($key,channel-name)
    }
    # Loop
    .connprops.f.nb.loop.src_index.e  insert 0 $ned($key,src_index)
    .connprops.f.nb.loop.dest_index.e  insert 0 $ned($key,dest_index)
    .connprops.f.nb.loop.src_gate_index.e  insert 0 $ned($key,src_gate_index)
    .connprops.f.nb.loop.dest_gate_index.e insert 0 $ned($key,dest_gate_index)
    .connprops.f.nb.loop.for.e insert 0 $ned($key,for_expression)
    # focus on first one
    focus .connprops.f.nb.gates.srcgate.e

    # exec the dialog, extract its contents if OK was pressed, then delete dialog
    if {[execOkCancelDialog .connprops] == 1} {
        # Gates
        set ned($key,srcgate)  [.connprops.f.nb.gates.srcgate.e get]
        set ned($key,destgate) [.connprops.f.nb.gates.destgate.e get]
        set ned($key,condition) [.connprops.f.nb.gates.condition.e get]
        set ned($key,comment)  [.connprops.f.nb.gates.comment.t get 1.0 end]
        # Loop
        set ned($key,src_index) [.connprops.f.nb.loop.src_index.e get]
        set ned($key,dest_index) [.connprops.f.nb.loop.dest_index.e get]
        set ned($key,src_gate_index) [.connprops.f.nb.loop.src_gate_index.e get]
        set ned($key,dest_gate_index) [.connprops.f.nb.loop.dest_gate_index.e get]
        set ned($key,for_expression) [.connprops.f.nb.loop.for.e get]

        set chname             [.connprops.f.nb.props.channel.e get]
        if {$chname!=""} {
           #megnezem hogy van e ilyen nevu channel
           set chkey [itemKeyFromName $chname channel]
           if {$chkey!=""} {
               set ned($key,channel-ownerkey) $chkey
           } else {
               #kerdest teszek fel
                set reply [tk_messageBox -title "Last chance" -icon warning -type yesno \
                                                  -message "Do you want to Define this channel  now?"]
                if {$reply == "yes"} {
                   # beszurom az uj channelt
                   set chkey [addItem channel]
                   # kitoltom a nevet
                   set ned($chkey,name) $chname
                   # feltoltom ertekekkel
                   set ned($chkey,error)    [.connprops.f.nb.props.error.e get]
                   set ned($chkey,delay)    [.connprops.f.nb.props.delay.e get]
                   set ned($chkey,datarate) [.connprops.f.nb.props.datarate.e get]
                   # kikeresek mindent ahoz hogy be tudja allitani a ownerkeyt
                   set canv_id $gned(canvas_id)
                   set modkey $canvas($canv_id,module-key)
                   set fkey $ned($modkey,parentkey)
                   set ned($chkey,parentkey) $fkey
                   # bejegyzem ehez a connectionhoz tulajdonosnak
                   set ned($key,channel-ownerkey) $chkey
                } else {
                   set ned($key,channel-ownerkey) ""
                   set ned($key,error)    [.connprops.f.nb.props.error.e get]
                   set ned($key,delay)    [.connprops.f.nb.props.delay.e get]
                   set ned($key,datarate) [.connprops.f.nb.props.datarate.e get]
                   set ned($key,channel-name) $chname
                }
           }
        } else {
           set ned($key,channel-ownerkey) ""
           set ned($key,error)    [.connprops.f.nb.props.error.e get]
           set ned($key,delay)    [.connprops.f.nb.props.delay.e get]
           set ned($key,datarate) [.connprops.f.nb.props.datarate.e get]
           set ned($key,channel-name) $chname
        }
    }
    destroy .connprops
}

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

proc editImportProps {key} {
    global gned ned canvas

    # create dialog with OK and Cancel buttons
    createOkCancelDialog .impprops "Import Properties"

    # add notebook pages
    notebook .impprops.f.nb
    pack .impprops.f.nb -expand 1 -fill both
    notebook_addpage .impprops.f.nb imports "Imports"

    label-entry .impprops.f.nb.imports.name  "Name:"
    label-entry .impprops.f.nb.imports.i    "Imports:"

    pack .impprops.f.nb.imports.name  -expand 0 -fill x -side top
    pack .impprops.f.nb.imports.i     -expand 0 -fill x -side top

    .impprops.f.nb.imports.name.e  insert 0 $ned($key,name)
    .impprops.f.nb.imports.name.e  configure -state disabled
    .impprops.f.nb.imports.i.e   insert 0 $ned($key,name)

    focus .impprops.f.nb.imports.name.e

    # exec the dialog, extract its contents if OK was pressed, then delete dialog
    if {[execOkCancelDialog .impprops] == 1} {
       set ned($key,name)     [.impprops.f.nb.imports.i.e   get]

    }
    destroy .impprops
    updateTreeManager
}

proc editChannelProps {key} {
    global gned ned canvas

    # create dialog with OK and Cancel buttons
    createOkCancelDialog .chanprops "Network Properties"

    # add notebook pages
    notebook .chanprops.f.nb
    pack .chanprops.f.nb -expand 1 -fill both
    notebook_addpage .chanprops.f.nb general "General"

    label-entry .chanprops.f.nb.general.name     "Name:"
    label-entry .chanprops.f.nb.general.delay    "Delay:"
    label-entry .chanprops.f.nb.general.error    "Error:"
    label-entry .chanprops.f.nb.general.datarate "Datarate:"
    label-text  .chanprops.f.nb.general.comment  "Comments:" 3

    pack .chanprops.f.nb.general.name     -expand 0 -fill x -side top
    pack .chanprops.f.nb.general.delay    -expand 0 -fill x -side top
    pack .chanprops.f.nb.general.error    -expand 0 -fill x -side top
    pack .chanprops.f.nb.general.datarate -expand 0 -fill x -side top
    pack .chanprops.f.nb.general.comment  -expand 0 -fill x -side top

    .chanprops.f.nb.general.name.e     insert 0 $ned($key,name)
    .chanprops.f.nb.general.delay.e    insert 0 $ned($key,delay)
    .chanprops.f.nb.general.error.e    insert 0 $ned($key,error)
    .chanprops.f.nb.general.datarate.e insert 0 $ned($key,datarate)
    .chanprops.f.nb.general.comment.t  insert 1.0 $ned($key,comment)

    focus .chanprops.f.nb.general.name.e

    # exec the dialog, extract its contents if OK was pressed, then delete dialog
    if {[execOkCancelDialog .chanprops] == 1} {
       set ned($key,name)       [.chanprops.f.nb.general.name.e get]
       set ned($key,delay)      [.chanprops.f.nb.general.delay.e get]
       set ned($key,error)      [.chanprops.f.nb.general.error.e  get]
       set ned($key,datarate)   [.chanprops.f.nb.general.datarate.e get]
       set ned($key,comment)    [.chanprops.f.nb.general.comment.t get 1.0 end]
    }
    destroy .chanprops
}

