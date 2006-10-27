#=================================================================
#  FIELDSPAGE.TCL - part of
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


#
# Creates a Fields page for a tabbed inspector window.
#
proc inspector_createfields2page {w} {
    global treeroots
    set nb $w.nb
    notebook_addpage $nb fields2 {Fields}

    if {![regexp {\.(ptr.*)-([0-9]+)} $w match object type]} {
        error "window name $w doesn't look like an inspector window"
    }
    set treeroots($nb.fields2.tree) $object

    # create treeview with scrollbars
    scrollbar $nb.fields2.vsb -command "$nb.fields2.tree yview"
    scrollbar $nb.fields2.hsb -command "$nb.fields2.tree xview" -orient horiz
    canvas $nb.fields2.tree -bg white -relief sunken -bd 2
    $nb.fields2.tree config -yscrollcommand "$nb.fields2.vsb set" -xscrollcommand "$nb.fields2.hsb set"

    Tree:init $nb.fields2.tree getFieldNodeInfo

    grid $nb.fields2.tree $nb.fields2.vsb -sticky news
    grid $nb.fields2.hsb  x               -sticky news
    grid rowconfig $nb.fields2 0 -weight 1
    grid columnconfig $nb.fields2 0 -weight 1

    bind $nb.fields2.tree <Button-1> {
        catch {destroy .popup}
        focus %W
        set key [Tree:nodeat %W %x %y]
        if {$key!=""} {
            Tree:setselection %W $key
        }
    }

    bind $nb.fields2.tree <Double-1> {
        catch {destroy .popup}
        focus %W
        set key [Tree:nodeat %W %x %y]
        getFieldNodeInfo_inspect %W $key
    }

    bind $nb.fields2.tree <Return> {
        catch {destroy .popup}
        set key [Tree:getselection %W]
        getFieldNodeInfo_inspect %W $key
   }

    refresh_fields2page $w   ;#FIXME this should be done from C++
}

proc refresh_fields2page {w} {
    set nb $w.nb
    set tree $nb.fields2.tree
    set tmp(treeroot) $w

    Tree:build $tree
    $tree xview moveto 0
}

#
# Content provider for the inspector treeview.
#
# Possible keys:
#   obj-<ptr>
#   struct-<ptr>-<descptr>
#   group-<ptr>-<descptr>-<groupid>
#   field-<ptr>-<descptr>-<fieldid>
#   findex-<ptr>-<descptr>-<fieldid>-<index>
#
# groupid is id of a field (ie a fieldid) which has the right group.
# We use that because the group name itself may contain space, hyphen
# and other unwanted characters.
#
proc getFieldNodeInfo {w op {key ""}} {
    global icons treeroots

    set keyargs [split $key "-"]
    set keytype [lindex $keyargs 0]
    set obj [lindex $keyargs 1]
    set sd [lindex $keyargs 2]

    switch $op {

        text {
            if [opp_isnull $obj] {return "<object is NULL>"}
            if [opp_isnull $sd] {return "<no descriptor for object>"}

            switch $keytype {
                obj {
                    set name [opp_getobjectfullname $obj]
                    set classname [opp_getobjectclassname $obj]
                    return "$name ($classname)"
                }
                struct {
                    set name [opp_classdescriptor name $obj $sd]
                    return $name
                }

                group {
                    set groupid [lindex $keyargs 3]
                    set groupname [opp_classdescriptor $obj $sd fieldproperty $groupid "group"]
                    return "\b$groupname\b"
                }

                field -
                findex {
                    set fieldid [lindex $keyargs 3]
                    set index [lindex $keyargs 4]
                    return [getFieldNodeInfo_getFieldText $obj $sd $fieldid $index]
                }

                default {
                    error "bad keytype '$keytype'"
                }
            }
        }

        options {
            return ""
        }

        icon {
            switch $keytype {
                obj {
                    return [get_icon_for_object $obj]
                }
                struct -
                group {
                    return ""
                }
                field -
                findex {
                    set fieldptr [getFieldNodeInfo_resolveObject $keyargs]
                    if [opp_isnull $fieldptr] {return ""}
                    return [get_icon_for_object $fieldptr]
                }
            }
        }

        tooltip {
            switch $keytype {
                obj -
                struct -
                group {
                    return ""
                }
                field -
                findex {
                    set fieldid [lindex $keyargs 3]
                    set tooltip [opp_classdescriptor $obj $sd fieldproperty $fieldid "tooltip"]
                    return $tooltip
                }
            }
        }

        haschildren {
            # FIXME can be improved performance-wise
            set children [getFieldNodeInfo $w children $key]
            if {$children==""} {
                return 0
            } else {
                return 1
            }
        }

        children {
            switch $keytype {
                obj -
                struct {
                    if {$obj==[opp_null]} {return ""}
                    if {$keytype=="obj"} {set sd [opp_getclassdescriptorfor $obj]}
                    if {$sd==[opp_null]} {return ""}

                    set children1 [getFieldNodeInfo_getFieldsInGroup $obj $sd ""]
                    set children2 [getFieldNodeInfo_getGroupKeys $obj $sd]
                    return [concat $children1 $children2]
                }

                group {
                    # return fields in the given group
                    set groupname [lindex $keyargs 3]
                    return [getFieldNodeInfo_getFieldsInGroup $obj $sd $groupname]
                }

                field {
                    set fieldid [lindex $keyargs 3]
                    set isarray [opp_classdescriptor $obj $sd fieldisarray $fieldid]
                    if {$isarray} {
                        # expand array: enumerate all indices
                        return [getFieldNodeInfo_getElementsInArray $obj $sd $fieldid]
                    }
                    set iscompound [opp_classdescriptor $obj $sd fieldiscompound $fieldid]
                    if {$iscompound} {
                        # return children on this class/struct
                        set isobject [opp_classdescriptor $obj $sd fieldisobject $fieldid]
                        set fieldptr [opp_classdescriptor $obj $sd fieldstructpointer $fieldid]
                        if [opp_isnull $fieldptr] {return ""}
                        if {$isobject} {
                            return [getFieldNodeInfo $w children "obj-$fieldptr"]
                        } else {
                            set fielddesc [opp_classdescriptor $obj $sd fieldstructdesc $fieldid]
                            if {$fielddesc==""} {return ""}  ;# nothing known about it
                            set tmpkey "struct-$fieldptr-$fielddesc"
                            return [getFieldNodeInfo $w children $tmpkey]
                        }
                    }
                    return ""
                }

                findex {
                    set fieldid [lindex $keyargs 3]
                    set index [lindex $keyargs 4]
                    set iscompound [opp_classdescriptor $obj $sd fieldiscompound $fieldid]
                    if {$iscompound} {
                        # return children on this class/struct
                        set isobject [opp_classdescriptor $obj $sd fieldisobject $fieldid]
                        set fieldptr [opp_classdescriptor $obj $sd fieldstructpointer $fieldid $index]
                        if [opp_isnull $fieldptr] {return ""}
                        if {$isobject} {
                            return [getFieldNodeInfo $w children "obj-$fieldptr"]
                        } else {
                            set fielddesc [opp_classdescriptor $obj $sd fieldstructdesc $fieldid]
                            if {$fielddesc==""} {return ""}  ;# nothing known about it
                            set tmpkey "struct-$fieldptr-$fielddesc"
                            return [getFieldNodeInfo $w children $tmpkey]
                        }
                    }
                    return ""
                }

                root {
                    return "obj-$obj"
                }

                default {
                    error "bad keytype '$keytype'"
                }
            }
        }

        root {
            # add an extra level to make the root object appear in the tree as well
            #return "obj-$treeroots($w)"
            return "root-$treeroots($w)"
        }
    }
}

#
# Helper proc for getFieldNodeInfo.
# Collects field groups, and converts them to keys.
#
proc getFieldNodeInfo_getGroupKeys {obj sd} {
    # collect list of groups
    set numfields [opp_classdescriptor $obj $sd fieldcount]
    for {set i 0} {$i<$numfields} {incr i} {
        set fieldgroup [opp_classdescriptor $obj $sd fieldproperty $i "group"]
        set groups($fieldgroup) $i
    }

    # convert them to keys
    set children {}
    foreach groupname [lsort [array names groups]] {
        if {$groupname!=""} {
            lappend children "group-$obj-$sd-$groups($groupname)"
        }
    }
    return $children
}


#
# Helper proc for getFieldNodeInfo.
# Return fields in the given group; groupname may be "" (meaning no group).
#
proc getFieldNodeInfo_getFieldsInGroup {obj sd groupid} {
    set children {}
    if {$groupid!=""} {
        set groupname [opp_classdescriptor $obj $sd fieldproperty $groupid "group"]
    } else {
        set groupname ""
    }
    set numfields [opp_classdescriptor $obj $sd fieldcount]
    for {set i 0} {$i<$numfields} {incr i} {
        if {$groupname==[opp_classdescriptor $obj $sd fieldproperty $i "group"]} {
            lappend children "field-$obj-$sd-$i"
        }
    }
    return $children
}

#
# Helper proc for getFieldNodeInfo.
# Expands array by enumerating all indices, and returns the list of corresponding keys.
#
proc getFieldNodeInfo_getElementsInArray {obj sd fieldid} {
    set children {}
    set n [opp_classdescriptor $obj $sd fieldarraysize $fieldid]
    for {set i 0} {$i<$n} {incr i} {
        lappend children "findex-$obj-$sd-$fieldid-$i"
    }
    return $children
}

#
# Helper proc for getFieldNodeInfo. Produces text for a field.
#
proc getFieldNodeInfo_getFieldText {obj sd fieldid index} {

    set typename [opp_classdescriptor $obj $sd fieldtypename $fieldid]
    set isarray [opp_classdescriptor $obj $sd fieldisarray $fieldid]
    set iscompound [opp_classdescriptor $obj $sd fieldiscompound $fieldid]
    set isobject [opp_classdescriptor $obj $sd fieldisobject $fieldid]

    # field name can be overridden with @label property
    set name [opp_classdescriptor $obj $sd fieldproperty $fieldid "label"]
    if {$name==""} {set name [opp_classdescriptor $obj $sd fieldname $fieldid]}

    # if it's an unexpanded array, return "name[size]" immediately
    if {$isarray && $index==""} {
        set size [opp_classdescriptor $obj $sd fieldarraysize $fieldid]
        return "$name\[$size\] ($typename)"
    }

    # when showing array elements, omit name and just show "[index]" instead
    if {$index!=""} {
        set name "\[$index\]"
    }

    # we'll want to print the field type, except for expanded array elements
    # (no need to repeat it, as it's printed in the "name[size]" node already)
    if {$index==""} {
        set typenametext " ($typename)"
    } else {
        set typenametext ""
    }

    if {$iscompound} {
        # if it's an object, try to say something about it...
        if {$isobject} {
            set fieldobj [opp_classdescriptor $obj $sd fieldstructpointer $fieldid $index]
            if [opp_isnull $fieldobj] {return "$name = \bNULL\b$typenametext"}
            set fieldobjname [opp_getobjectfullname $fieldobj]
            set fieldobjclassname [opp_getobjectclassname $fieldobj]
            set fieldobjinfo [opp_getobjectinfostring $fieldobj]
            return "$name = \b($fieldobjclassname) $fieldobjname: $fieldobjinfo\b$typenametext"
        } else {
            # a value can be generated via operator<<
            set value [opp_classdescriptor $obj $sd fieldvalue $fieldid $index]
            if {$value==""} {
                return "$name$typenametext"
            } else {
                return "$name = \b$value\b$typenametext"
            }
        }
    } else {
        # plain field, return "name = value" text
        set value [opp_classdescriptor $obj $sd fieldvalue $fieldid $index]
        set enumname [opp_classdescriptor $obj $sd fieldproperty $fieldid "enum"]
        if {$enumname!=""} {
            append typename " - enum $enumname"
            set symbolicname [opp_getnameforenum $enumname $value]
            set value "$symbolicname ($value)"
        }
        if {$typename=="string"} {set value "\"$value\""}
        if {$value==""} {
            return "$name$typenametext"
        } else {
            return "$name = \b$value\b$typenametext"
        }
    }
}

#
# If the given key (in split form) identifies an object (cClassDescriptor
# isObject), returns its pointer. Otherwise returns [opp_null].
#
#
proc getFieldNodeInfo_resolveObject {keyargs} {
    set keytype [lindex $keyargs 0]

    if {$keytype=="field" || $keytype=="findex"} {
        set obj [lindex $keyargs 1]
        set sd [lindex $keyargs 2]
        set fieldid [lindex $keyargs 3]
        set index [lindex $keyargs 4]
        set isobject [opp_classdescriptor $obj $sd fieldisobject $fieldid]
        set isarray [opp_classdescriptor $obj $sd fieldisarray $fieldid]

        if {$isobject && (!$isarray || $index!="")} {
            return [opp_classdescriptor $obj $sd fieldstructpointer $fieldid $index]
        }
    }
    return [opp_null]
}


#
# Invoked on hitting Enter or double-clicking in the tree, it opens
# an inspector for the given object.
#
proc getFieldNodeInfo_inspect {w key} {
    set keyargs [split $key "-"]
    set ptr [getFieldNodeInfo_resolveObject $keyargs]
    if [opp_isnotnull $ptr] {
        opp_inspect $ptr "(default)"
    }
}



##
## This alternative version of the Fields table uses a BLT treeview widget.
##
#proc inspector_createfields2pageX {w} {
#    global treeroots
#    set nb $w.nb
#    notebook_addpage $nb fields2 {Fields}
#
#    multicolumnlistbox $nb.fields2.list {
#       {value  Value   200}
#       {type   Type    80}
#       {on     Declared-on}
#    } -width 400 -yscrollcommand "$nb.fields2.vsb set" -xscrollcommand "$nb.fields2.hsb set"
#    scrollbar $nb.fields2.hsb  -command "$nb.fields2.list xview" -orient horiz
#    scrollbar $nb.fields2.vsb  -command "$nb.fields2.list yview"
#    grid $nb.fields2.list $nb.fields2.vsb -sticky news
#    grid $nb.fields2.hsb  x           -sticky news
#    grid rowconfig    $nb.fields2 0 -weight 1 -minsize 0
#    grid columnconfig $nb.fields2 0 -weight 1 -minsize 0
#
#    set tree $nb.fields2.list
#
#    $tree config -flat no -hideroot yes
#    $tree column configure treeView -text "Name" -hide no -width 160 -state disabled
#
#    #set root [$tree insert end ROOT -data {value 42 type struct}]
#    #set one  [$tree insert end {ROOT ONE} -data {value 42 type short}]
#    #set two  [$tree insert end {ROOT TWO} -data {value 42 type double}]
#
#    if {![regexp {\.(ptr.*)-([0-9]+)} $w match object type]} {
#        error "window name $w doesn't look like an inspector window"
#    }
#
#    fillTreeview $tree $object   ;#FIXME should be called from C++ on updates
#}
#
#proc fillTreeview {tree obj} {
#    set desc [opp_getclassdescriptor $obj]
#    set key "fld-$obj-$desc---"
#
#    _doFillTreeview $tree {} $key
#}
#
#proc _doFillTreeview {tree path key} {
#    global icons
#
#    set data [_getFieldDataFor $key]
#    set name [lindex $data 1]
#    set newpath [concat $path [list $name]]
#    #set icon $icons(1pixtransp)
#    #set icon $icons(cogwheel_vs)
#    set icon $icons(node_xs)
#    $tree insert end $newpath -data $data -icons [list $icon $icon] -activeicons [list $icon $icon]
#
#    foreach child [getFieldNodeInfo $tree children $key] {
#        _doFillTreeview $tree $newpath $child
#    }
#}
#
#proc _getFieldDataFor {key} {
#    # key: objectptr-descriptorptr-fieldid-index
#    set obj ""
#    set sd ""
#    set fieldid ""
#    set index ""
#    regexp {^fld-(ptr.*)-(ptr.*)-([^-]*)-([^-]*)$} $key dummy obj sd fieldid index
#    #puts "DBG --> $obj -- $sd -- field=$fieldid -- index=$index"
#
#    if {$obj==[opp_null]} {return "<object is NULL>"}
#    if {$sd==[opp_null]} {return "<no descriptor for object>"}
#
#    set declname [opp_classdescriptor $obj $sd name]
#
#    if {$fieldid==""} {
#        return [list treeView $declname value "" type "" on ""]
#    }
#
#    set typename [opp_classdescriptor $obj $sd fieldtypename $fieldid]
#    set isarray [opp_classdescriptor $obj $sd fieldisarray $fieldid]
#    set name [opp_classdescriptor $obj $sd fieldname $fieldid]
#    if {$index!=""} {
#        append name "\[$index\]"
#    } elseif {$isarray} {
#        set size [opp_classdescriptor $obj $sd fieldarraysize $fieldid]
#        append name "\[$size\]"
#    }
#    set value [opp_classdescriptor $obj $sd fieldvalue $fieldid $index]
#    regsub -all "\n" $value "; " value
#    regsub -all "   +" $value "  " value
#    if {$typename=="string"} {set value "\"$value\""}
#
#    return [list treeView $name value $value type $typename on $declname]
#}

