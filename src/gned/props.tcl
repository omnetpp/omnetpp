#==========================================================================
#  PROPS.TCL -
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


proc editProps {key} {
    global ned

    set type $ned($key,type)
    if {$type=="module"} {
       editModuleProps $key
    } elseif {$type=="simple"} {
       editModuleProps $key
    } elseif {$type=="submod"} {
       editSubmoduleProps $key
    } elseif {$type=="conn"} {
       editConnectionProps $key
    } elseif {$type=="channel"} {
       editChannelProps $key
    } elseif {$type=="imports"} {
       editImportProps $key
    } elseif {$type=="network"} {
       editNetworkProps $key
    } else {
       tk_messageBox -icon error -type ok -title GNED \
          -message "No 'Properties' dialog for a $type."
       return
    }
    updateTreeManager
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

