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
# Possible keys:
#   obj-<ptr>
#   struct-<ptr>-<descptr>
#   group-<ptr>-<descptr>-<groupname>
#   field-<ptr>-<descptr>-<fieldid>
#   findex-<ptr>-<descptr>-<fieldid>-<index>
#
proc getFieldNodeInfo {w op {key ""}} {
    global icons treeroots

    set keyargs [split $key "-"]
    set keytype [lindex $keyargs 0]
    set obj [lindex $keyargs 1]
    set sd [lindex $keyargs 2]

    switch $op {

        text {
            if {$obj==[opp_object_nullpointer]} {return "<object is NULL>"}
            if {$sd==[opp_object_nullpointer]} {return "<no descriptor for object>"}

            switch $keytype {
                obj {
                    set name [opp_object_fullname $obj]
                    set classname [opp_object_classname $obj]
                    return "$name ($classname)"
                }
                struct {
                    set name [opp_classdescriptor name $obj $sd]
                    return $keyargs
                }

                group {
                    set groupname [lindex $keyargs 3]
                    return "\b$groupname\b"
                }

                field -
                findex {
                    set fieldid [lindex $keyargs 3]
                    set index [lindex $keyargs 4]
                    set typename [opp_classdescriptor $obj $sd fieldtypename $fieldid]
                    set isarray [opp_classdescriptor $obj $sd fieldisarray $fieldid]
                    set iscompound [opp_classdescriptor $obj $sd fieldiscompound $fieldid]
                    set name [opp_classdescriptor $obj $sd fieldname $fieldid]
                    if {$index!=""} {
                        append name "\[$index\]"
                    } elseif {$isarray} {
                        set size [opp_classdescriptor $obj $sd fieldarraysize $fieldid]
                        append name "\[$size\]"
                    }
                    if {$iscompound} {
                        append name " {...}"
                    }
                    set value [opp_classdescriptor $obj $sd fieldvalue $fieldid $index]
                    #FIXME display enumname too!
                    if {$typename=="string"} {set value "\"$value\""}
                    if {$value==""} {
                        return "$name ($typename)"
                    } else {
                        return "$name = \b$value\b ($typename)"
                    }
                }

                default {
                    error "bad keytype '$keytype'"
                }
            }


            if {$group=="" && $fieldid==""} {
                # no specific field -- return class name
                set name [opp_classdescriptor $obj $sd name]
                return $name
            }
            if {$group!="" && $fieldid==""} {
                # only group given -- return that
                return "\b$group\b"
            }
            set typename [opp_classdescriptor $obj $sd fieldtypename $fieldid]
            set isarray [opp_classdescriptor $obj $sd fieldisarray $fieldid]
            set iscompound [opp_classdescriptor $obj $sd fieldiscompound $fieldid]
            set name [opp_classdescriptor $obj $sd fieldname $fieldid]
            if {$index!=""} {
                append name "\[$index\]"
            } elseif {$isarray} {
                set size [opp_classdescriptor $obj $sd fieldarraysize $fieldid]
                append name "\[$size\]"
            }
            if {$iscompound} {
                append name " {...}"
            }
            set value [opp_classdescriptor $obj $sd fieldvalue $fieldid $index]
            #FIXME display enumname too!
            if {$typename=="string"} {set value "\"$value\""}
            if {$value==""} {
                return "$name ($typename)"
            } else {
                return "$name = \b$value\b ($typename)"
            }
        }

        options {
            return ""
        }

        icon {
            switch $keytype {
                obj {
                    #FIXME this never gets called...
                    return [get_icon_for_object $obj]
                }
                struct -
                group -
                field -
                findex {
                    return ""
                }
            }
        }

        haschildren {
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
                    if {$obj==[opp_object_nullpointer]} {return ""}
                    if {$keytype=="obj"} {set sd [opp_getclassdescriptorfor $obj]}
                    if {$sd==[opp_object_nullpointer]} {return ""}

                    # collect list of groups
                    set numfields [opp_classdescriptor $obj $sd fieldcount]
                    for {set i 0} {$i<$numfields} {incr i} {
                        set fieldgroup [opp_classdescriptor $obj $sd fieldproperty $i "group"]
                        set groups($fieldgroup) 1
                    }
                    set children ""
                    foreach fieldgroup [lsort [array names groups]] {
                        if {$fieldgroup!=""} {
                            lappend children "group-$obj-$sd-$fieldgroup"
                        }
                    }

                    # plus fields with no groups
                    for {set i 0} {$i<$numfields} {incr i} {
                        set fieldgroup [opp_classdescriptor $obj $sd fieldproperty $i "group"]
                        if {$fieldgroup==""} {
                            lappend children "field-$obj-$sd-$i"
                        }
                    }
                    return $children
                }

                group {
                    # return fields in the given group
                    set groupname [lindex $keyargs 3]
                    set children ""
                    set numfields [opp_classdescriptor $obj $sd fieldcount]
                    for {set i 0} {$i<$numfields} {incr i} {
                        if {$groupname==[opp_classdescriptor $obj $sd fieldproperty $i "group"]} {
                            lappend children "field-$obj-$sd-$i"
                        }
                    }
                    return $children
                }

                field {
                    set fieldid [lindex $keyargs 3]
                    set isarray [opp_classdescriptor $obj $sd fieldisarray $fieldid]
                    if {$isarray} {
                        # expand array: enumerate all indices
                        set n [opp_classdescriptor $obj $sd fieldarraysize $fieldid]
                        set children ""
                        for {set i 0} {$i<$n} {incr i} {
                            lappend children "findex-$obj-$sd-$fieldid-$i"
                        }
                        return $children
                    }
                    set iscompound [opp_classdescriptor $obj $sd fieldiscompound $fieldid]
                    if {$iscompound} {
                        # return children on this class/struct
                        set isobject [opp_classdescriptor $obj $sd fieldisobject $fieldid]
                        set fieldptr [opp_classdescriptor $obj $sd fieldstructpointer $fieldid]
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

                default {
                    error "bad keytype '$keytype'"
                }
            }
        }

        root {
            return "obj-$treeroots($w)"
        }
    }
}

#============================

#
# This alternative version of the Fields table uses a BLT treeview widget.
#
proc inspector_createfields2pageX {w} {
    global treeroots
    set nb $w.nb
    notebook_addpage $nb fields2 {Fields}

    multicolumnlistbox $nb.fields2.list {
       {value  Value   200}
       {type   Type    80}
       {on     Declared-on}
    } -width 400 -yscrollcommand "$nb.fields2.vsb set" -xscrollcommand "$nb.fields2.hsb set"
    scrollbar $nb.fields2.hsb  -command "$nb.fields2.list xview" -orient horiz
    scrollbar $nb.fields2.vsb  -command "$nb.fields2.list yview"
    grid $nb.fields2.list $nb.fields2.vsb -sticky news
    grid $nb.fields2.hsb  x           -sticky news
    grid rowconfig    $nb.fields2 0 -weight 1 -minsize 0
    grid columnconfig $nb.fields2 0 -weight 1 -minsize 0

    set tree $nb.fields2.list

    $tree config -flat no -hideroot yes
    $tree column configure treeView -text "Name" -hide no -width 160 -state disabled

    #set root [$tree insert end ROOT -data {value 42 type struct}]
    #set one  [$tree insert end {ROOT ONE} -data {value 42 type short}]
    #set two  [$tree insert end {ROOT TWO} -data {value 42 type double}]

    if {![regexp {\.(ptr.*)-([0-9]+)} $w match object type]} {
        error "window name $w doesn't look like an inspector window"
    }

    fillTreeview $tree $object   ;#FIXME should be called from C++ on updates
}

proc fillTreeview {tree obj} {
    set desc [opp_getclassdescriptor $obj]
    set key "fld-$obj-$desc---"

    _doFillTreeview $tree {} $key
}

proc _doFillTreeview {tree path key} {
    global icons

    set data [_getFieldDataFor $key]
    set name [lindex $data 1]
    set newpath [concat $path [list $name]]
    #set icon $icons(1pixtransp)
    #set icon $icons(cogwheel_vs)
    set icon $icons(node_xs)
    $tree insert end $newpath -data $data -icons [list $icon $icon] -activeicons [list $icon $icon]

    foreach child [getFieldNodeInfo $tree children $key] {
        _doFillTreeview $tree $newpath $child
    }
}

proc _getFieldDataFor {key} {
    # key: objectptr-descriptorptr-fieldid-index
    set obj ""
    set sd ""
    set fieldid ""
    set index ""
    regexp {^fld-(ptr.*)-(ptr.*)-([^-]*)-([^-]*)$} $key dummy obj sd fieldid index
    #puts "DBG --> $obj -- $sd -- field=$fieldid -- index=$index"

    if {$obj==[opp_object_nullpointer]} {return "<object is NULL>"}
    if {$sd==[opp_object_nullpointer]} {return "<no descriptor for object>"}

    set declname [opp_classdescriptor $obj $sd name]

    if {$fieldid==""} {
        return [list treeView $declname value "" type "" on ""]
    }

    set typename [opp_classdescriptor $obj $sd fieldtypename $fieldid]
    set isarray [opp_classdescriptor $obj $sd fieldisarray $fieldid]
    set name [opp_classdescriptor $obj $sd fieldname $fieldid]
    if {$index!=""} {
        append name "\[$index\]"
    } elseif {$isarray} {
        set size [opp_classdescriptor $obj $sd fieldarraysize $fieldid]
        append name "\[$size\]"
    }
    #if {$iscompound} {
    #    append name " {...}"
    #}
    set value [opp_classdescriptor $obj $sd fieldvalue $fieldid $index]
    regsub -all "\n" $value "; " value
    regsub -all "   +" $value "  " value
    if {$typename=="string"} {set value "\"$value\""}

    return [list treeView $name value $value type $typename on $declname]
}

