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

proc getFieldNodeInfo {w op {key {}}} {
    global icons treeroots

    set separatebaseclasses 1

    # key: objectptr-descriptorptr-fieldnum-index
    set obj ""
    set sd ""
    set fieldnum ""
    set index ""
    regexp {^fld-(ptr.*)-(ptr.*)-([^-]*)-([^-]*)$} $key dummy obj sd fieldnum index
    #puts "DBG --> $obj -- $sd -- field=$fieldnum -- index=$index"

    switch $op {

        text {
            if {$obj==[opp_object_nullpointer]} {return "<object is NULL>"}
            if {$sd==[opp_object_nullpointer]} {return "<no descriptor for object>"}

            if {$fieldnum==""} {
                # no specific field -- return class name
                set name [opp_classdescriptor $obj $sd name]
                return $name
            }
            set typename [opp_classdescriptor $obj $sd fieldtypename $fieldnum]
            set type [opp_classdescriptor $obj $sd fieldtype $fieldnum]
            set name [opp_classdescriptor $obj $sd fieldname $fieldnum]
            if {$index!=""} {
                append name "\[$index\]"
            } elseif {$type=="basic array" || $type=="struct array"} {
                set size [opp_classdescriptor $obj $sd fieldarraysize $fieldnum]
                append name "\[$size\]"
            }
            if {$type=="struct" || $type=="struct array"} {
                append name " {...}"
            }
            set value [opp_classdescriptor $obj $sd fieldvalue $fieldnum $index]
            regsub -all "\n" $value "; " value
            regsub -all "   +" $value "  " value
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
            return ""
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
            if {$obj==[opp_object_nullpointer] || $sd==[opp_object_nullpointer]} {
                return ""
            }

            if {$fieldnum==""} {
                # no field given -- so return field list
                set children {}
                set fromfield 0
                set numfields [opp_classdescriptor $obj $sd fieldcount]

                set baseclassdesc [opp_classdescriptor $obj $sd baseclassdesc]
                if {$baseclassdesc!=[opp_object_nullpointer] && $separatebaseclasses} {
                    # display base class fields separately
                    lappend children "fld-$obj-$baseclassdesc--"
                    set fromfield [opp_classdescriptor $obj $baseclassdesc fieldcount]
                }

                # assemble fields list
                for {set i $fromfield} {$i<$numfields} {incr i} {
                    lappend children "fld-$obj-$sd-$i-"
                }
                return $children
            }
            set type [opp_classdescriptor $obj $sd fieldtype $fieldnum]
            if {$type=="basic"} {
                return ""
            } elseif {$type=="struct"} {
                set fieldptr [opp_classdescriptor $obj $sd fieldstructpointer $fieldnum]
                set fielddesc [opp_classdescriptor $obj $sd fieldstructdesc $fieldnum]
                if {$fielddesc==""} {
                    return ""  ;# nothing known about it
                } else {
                    # resolve children now, otherwise we'd dislay the type as an extra level
                    set key "fld-$fieldptr-$fielddesc--"
                    return [getFieldNodeInfo $w children $key]
                }
            } elseif {$index!=""} {
                # array already expanded, but we may expand further if each element is a struct
                if {$type=="struct array"} {
                    set fieldptr [opp_classdescriptor $obj $sd fieldstructpointer $fieldnum $index]
                    set fielddesc [opp_classdescriptor $obj $sd fieldstructdesc $fieldnum]
                    if {$fielddesc==""} {
                        return ""  ;# nothing known about it
                    } else {
                        # resolve children now, otherwise we'd dislay the type as an extra level
                        set key "fld-$fieldptr-$fielddesc--"
                        return [getFieldNodeInfo $w children $key]
                    }
                } else {
                    return ""
                }
            } elseif {$type=="basic array" || $type=="struct array"} {
                # expand array: enumerate all indices
                set n [opp_classdescriptor $obj $sd fieldarraysize $fieldnum]
                set children {}
                for {set i 0} {$i<$n} {incr i} {
                    lappend children "fld-$obj-$sd-$fieldnum-$i"
                }
                return $children
            } else {
                error "wrong fieldtype $type"
            }
        }

        root {
            set obj $treeroots($w)
            set desc [opp_getclassdescriptor $obj]
            return "fld-$obj-$desc--"
        }
    }
}

#============================

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
    set key "fld-$obj-$desc--"

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
    # key: objectptr-descriptorptr-fieldnum-index
    set obj ""
    set sd ""
    set fieldnum ""
    set index ""
    regexp {^fld-(ptr.*)-(ptr.*)-([^-]*)-([^-]*)$} $key dummy obj sd fieldnum index
    #puts "DBG --> $obj -- $sd -- field=$fieldnum -- index=$index"

    if {$obj==[opp_object_nullpointer]} {return "<object is NULL>"}
    if {$sd==[opp_object_nullpointer]} {return "<no descriptor for object>"}

    set declname [opp_classdescriptor $obj $sd name]

    if {$fieldnum==""} {
        return [list treeView $declname value "" type "" on ""]
    }

    set typename [opp_classdescriptor $obj $sd fieldtypename $fieldnum]
    set type [opp_classdescriptor $obj $sd fieldtype $fieldnum]
    set name [opp_classdescriptor $obj $sd fieldname $fieldnum]
    if {$index!=""} {
        append name "\[$index\]"
    } elseif {$type=="basic array" || $type=="struct array"} {
        set size [opp_classdescriptor $obj $sd fieldarraysize $fieldnum]
        append name "\[$size\]"
    }
    #if {$type=="struct" || $type=="struct array"} {
    #    append name " {...}"
    #}
    set value [opp_classdescriptor $obj $sd fieldvalue $fieldnum $index]
    regsub -all "\n" $value "; " value
    regsub -all "   +" $value "  " value
    if {$typename=="string"} {set value "\"$value\""}

    return [list treeView $name value $value type $typename on $declname]
}

