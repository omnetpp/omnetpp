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

    # tk_messageBox -icon warning -type ok -title GNED \
       -message "Editing dialogs not ready yet. Sorry!\
                 Until I finish updating the TCL code to work with the new\
                 internal data structure, you can use the NED source view\
                 if you want to edit module parameters, gates etc."
    # return

    if {$ned($key,type)=="module"} {
       editModuleProps $key
    } elseif {$ned($key,type)=="submod"} {
       editSubmoduleProps $key
    } elseif {$ned($key,type)=="conn"} {
       editConnectionProps $key
    }
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


proc getCommentFromText {w} {
    set comment [$w get 1.0 end]
    if {$comment=="\n"} {
        set comment {}
    }
    # kill lines with single '-' (would mean a blank line in comment)
    regsub -all "\n-\n" $comment "\n\n" comment
    regsub -all "^-\n" $comment "\n" comment
    regsub -all "\n-$" $comment "\n\n" comment
    return $comment
}

proc fillSpreadsheetFromNed {w parentkey} {
    global ned ddfields tablePriv

    set li 0
    foreach key [getChildren $parentkey] {
        foreach attr $ddfields($ned($key,type)) {
            set tablePriv($w,$li,$attr) $ned($key,$attr)
        }
        incr li
    }
}

proc updateNedFromSpreadsheet {w parentkey itemtype keyattr} {
    global ned ddfields tablePriv

    # delete old stuff in ned()
    foreach key [getChildren $parentkey] {
        deleteItem $key
    }

    # add items from tablePriv() to ned()
    for {set li 0} {[info exist tablePriv($w,$li,$keyattr)]} {incr li} {
        if {$tablePriv($w,$li,$keyattr)!=""} {
            set key [addItem $itemtype $parentkey]
            foreach attr $ddfields($itemtype) {
                set ned($key,$attr) $tablePriv($w,$li,$attr)
            }
        }
    }
}

