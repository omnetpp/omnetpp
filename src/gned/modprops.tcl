#==========================================================================
#  MODPROPS.TCL -
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

    # create "General" page
    label-entry .modprops.f.nb.general.name "Name:"
    label-text  .modprops.f.nb.general.comment "Comments:" 5
    pack .modprops.f.nb.general.name  -expand 0 -fill x -side top
    pack .modprops.f.nb.general.comment -expand 1 -fill both -side top

    # create "Parameters" page
    #label-text  .modprops.f.nb.pars.pars "Parameters:" 7
    #pack .modprops.f.nb.pars.pars  -expand 1 -fill both -side top

    # create "Gates" page
    #label-text  .modprops.f.nb.gates.gates "Gates:" 7
    #pack .modprops.f.nb.gates.gates -expand 1 -fill both -side top

    # create "Machines" page
    #label-entry .modprops.f.nb.mach.name "Host:"
    #pack .modprops.f.nb.mach.name  -expand 0 -fill x -side top

    focus .modprops.f.nb.general.name.e

    .modprops.f.nb.general.name.e insert 0 $ned($key,name)
    .modprops.f.nb.general.comment.t insert 1.0 $ned($key,banner-comment)

    #.modprops.f.nb.pars.pars.t insert 1.0 $ned($key,parameters)
    #.modprops.f.nb.gates.gates.t insert 1.0 $ned($key,gates)
    #.modprops.f.nb.mach.name.e insert 0 $ned($key,machines)

    # exec the dialog, extract its contents if OK was pressed, then delete dialog
    if {[execOkCancelDialog .modprops] == 1} {
        set ned($key,banner-comment) [getCommentFromText .modprops.f.nb.general.comment.t]
        #set ned($key,parameters) [.modprops.f.nb.pars.pars.t get 1.0 end]
        #set ned($key,gates) [.modprops.f.nb.gates.gates.t get 1.0 end]
        #set ned($key,machines) [.modprops.f.nb.mach.name.e get]

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


