#==========================================================================
#  PROPS.TCL -
#            part of the GNED, the Tcl/Tk graphical topology editor of
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

    # text widgets seem to add an extra newline -- remove it
    regsub -all "\n$" $comment "" comment

    return [processCommentAfterEdit $comment]
}


proc processCommentAfterEdit {comment} {

    # kill lines with single '-' (would mean a blank line in comment)
    regsub -all "\n-\n" $comment "\n\n" comment
    regsub -all "^-\n" $comment "\n" comment
    regsub -all "\n-$" $comment "\n\n" comment

    # make sure there's a newline at the end
    regsub -all "\(\[^\n\]\)$" $comment "\\1\n" comment
    return $comment
}


# fillTableEditFromNed --
#
# w: table widget
# parentkey: key of the params,gates,substparams,gatesizes etc. item
#
proc fillTableEditFromNed {w parentkey} {
    global ned ned_attlist tablePriv

    set li 0
    foreach key [getChildren $parentkey] {
        foreach attr $ned_attlist($ned($key,type)) {
            set value $ned($key,$attr)
            if [string match "*-comment" $attr] {
                regsub "\n$" $value "" value
            }
            set tablePriv($w,$li,$attr) $value
        }
        incr li
    }
}


# updateNedFromTableEdit --
#
# w:         table widget
# parentkey: key of the params,gates,substparams,gatesizes etc. item
# itemtype:  param, substparam, gatesize etc.
# keyattr:   if this attr!="", table line will be added to ned()
#
proc updateNedFromTableEdit {w parentkey itemtype keyattr} {
    global ned ned_attlist tablePriv

    # count lines in table
    set tblN 0
    for {set li 0} {[info exist tablePriv($w,$li,$keyattr)]} {incr li} {
        if {$tablePriv($w,$li,$keyattr)!=""} {
            incr tblN
        }
    }

    # add or delete $itemtype items to match count in table
    set needN [expr $tblN-[llength [getChildren $parentkey]]]
    if {$needN<0} {
        # delete $needN items
        foreach key [lrange [getChildren $parentkey] 0 [expr -$needN-1]] {
            deleteItem $key
        }
    } elseif {$needN>0} {
        # add $needN items
        for {set i 0} {$i<$needN} {incr i} {
            addItem $itemtype $parentkey
        }
    }

    # copy item attributes from tablePriv() to ned()
    set items [getChildren $parentkey]
    set i 0
    for {set li 0} {[info exist tablePriv($w,$li,$keyattr)]} {incr li} {
        if {$tablePriv($w,$li,$keyattr)!=""} {
            set key [lindex $items $i]
            foreach attr $ned_attlist($itemtype) {
                # set attrs not in tablePriv() to ""
                if [catch {
                   set value $tablePriv($w,$li,$attr)
                }] {
                   set value ""
                }
                set ned($key,$attr) [validateAttrValue $key $attr $value]
            }
            incr i
        }
    }

    # return true if section was empty
    return [expr $tblN==0]
}


# validateAttrValue --
#
# Validates and if wrong, corrects an attribute value in ned()
#
proc validateAttrValue {key attr value} {
    global ned ned_attlist

    # check, depending on attribute name
    if [string match "*-comment" $attr] {
        set value [processCommentAfterEdit $value]
        return $value
    }

    if {$attr=="isvector"} {
        if {[string match "0*" $value] || [string match "n*" $value] || [string match "f*" $value]} {
            set value 0
        } else {
            set value 1
        }
        return $value
    }

    if {$attr=="datatype"} {
        if {$value==""} {
            set value "numeric"
        }
        return $value
    }

    if {$attr=="gatetype"} {
        if [string match "i*" $value] {
            set value "in"
        } else {
            set value "out"
        }
        return $value
    }

    if {$attr=="value"} {
        if {$value==""} {
            set value 0
        }
        return $value
    }

    if {$attr=="name"} {
        if {$value==""} {
            set value "unnamed"
        }

        # remove illegal chars
        regsub -all {[^a-zA-Z0-9_]} $value "" value
        regsub {^[0-9]} $value "_&" value
        return $value
    }

    return $value
}

# assertEntryFilledIn --
#
# Helper proc to be used from execOkCancelDialog validation procs
#
proc assertEntryFilledIn {w what} {
    if {[$w get]==""} {
        focus $w
        error "Please fill in $what!"
    }
}

# assertEntryIsValidName --
#
# Helper proc to be used from execOkCancelDialog validation procs
#
proc assertEntryIsValidName {w what} {
    set value [$w get]
    if ![isNameLegal $value] {
        focus $w
        error "Note: $what should be a legal identifier! \
               (It may contain letters, digits and underscore, may not begin\
               with a number and may not be a reserved word.)"
    }
}



