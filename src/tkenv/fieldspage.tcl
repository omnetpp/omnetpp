#=================================================================
#  FIELDSPAGE.TCL - part of
#
#                     OMNeT++/OMNEST
#            Discrete System Simulation in C++
#
#=================================================================

#----------------------------------------------------------------#
#  Copyright (C) 1992-2017 Andras Varga
#
#  This file is distributed WITHOUT ANY WARRANTY. See the file
#  `license' for details on this and other legal matters.
#----------------------------------------------------------------#


#
# Creates a Fields page for a tabbed inspector window.
#
proc createFieldsPage {f w} {
    global treeroots
    global B2 B3

    # create treeview with scrollbars
    canvas $f.tree -bg white
    addScrollbars $f.tree

    set tree $f.tree
    set treeroots($tree) [opp_null]

    Tree:init $tree fieldsPage:getNodeInfo

    bind $tree <Double-1> {
        focus %W
        set key [Tree:nodeat %W %x %y]
        fieldsPage:getNodeInfo:inspect %W $key
    }

    bind $tree <Return> {
        set key [Tree:getselection %W]
        fieldsPage:getNodeInfo:inspect %W $key
    }

    bind $tree <Button-$B3> {
        focus %W
        set key [Tree:nodeat %W %x %y]
        if {$key!=""} {
            Tree:setselection %W $key
            fieldsPage:getNodeInfo:popup %W $key %X %Y
        }
    }

    set object [opp_null]
    set treeroots($tree) $object
    Tree:open $tree "0-obj-$object"
}

proc fieldsPage:refresh {insp} {
    global treeroots

    set tree $insp.nb.fields.tree
    if ![winfo exist $tree] {return}

    set object [opp_inspector_getobject $insp]
    if {$treeroots($tree)!=$object} {
        set treeroots($tree) $object
        Tree:open $tree "0-obj-$object"
    }

    Tree:build $tree
}

#
# Content provider for the inspector treeview.
#
# Possible keys:
#   <depth>-obj-<ptr>
#   <depth>-struct-<ptr>-<descptr>
#   <depth>-group-<ptr>-<descptr>-<groupid>
#   <depth>-field-<ptr>-<descptr>-<fieldid>
#   <depth>-findex-<ptr>-<descptr>-<fieldid>-<index>
#
# groupid is id of a field (ie a fieldid) which has the right group.
# We use that because the group name itself may contain space, hyphen
# and other unwanted characters.
#
# depth is the depth in the displayed tree (0 for root, 1 for its children, etc).
# It is needed to make multiple appearances of the same object unique. Without it,
# if a node appears under itself in the tree, that causes infinite recursion.
# (crash scenario without depth: open the object's "owner" field, then the object
# itself within its owner --> bang!).
#
proc fieldsPage:getNodeInfo {w op {key ""}} {
    global icons treeroots

    set keyargs [split $key "-"]
    set depth [lindex $keyargs 0]
    set keytype [lindex $keyargs 1]
    set obj [lindex $keyargs 2]
    set sd [lindex $keyargs 3]

    switch $op {

        text {
            if [opp_isnull $obj] {return "No object selected"}
            if {$sd!="" && [opp_isnull $sd]} {return "<no descriptor for object>"}

            switch $keytype {
                obj {
                    set name [opp_getobjectfullname $obj]
                    set classname [opp_getobjectshorttypename $obj]
                    return "$name ($classname)"
                }
                struct {
                    set name [opp_classdescriptor name $obj $sd]
                    return $name
                }

                group {
                    set groupid [lindex $keyargs 4]
                    set groupname [opp_classdescriptor $obj $sd fieldproperty $groupid "group"]
                    return "\b$groupname\b"
                }

                field -
                findex {
                    set fieldid [lindex $keyargs 4]
                    set index [lindex $keyargs 5]
                    return [fieldsPage:getNodeInfo:getFieldText $obj $sd $fieldid $index]
                }

                default {
                    error "bad keytype '$keytype'"
                }
            }
        }

        needcheckbox {
            return 0
        }

        options {
            return ""
        }

        icon {
            switch $keytype {
                obj {
                    if [opp_isnull $obj] {return ""}
                    return [opp_getobjecticon $obj]
                }
                struct -
                group {
                    return ""
                }
                field -
                findex {
                    set obj [fieldsPage:getNodeInfo:resolveObject $keyargs]
                    if [opp_isnull $obj] {return ""}
                    return [opp_getobjecticon $obj]
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
                    ## These tooltips are actually not very useful
                    # set fieldid [lindex $keyargs 4]
                    # set tooltip [opp_classdescriptor $obj $sd fieldproperty $fieldid "hint"]
                    # return $tooltip
                    return ""
                }
            }
        }

        haschildren {
            switch $keytype {
                group -
                root {
                    return 1  ;# sure has children
                }
                obj -
                struct -
                field -
                findex {
                    # XXX this can be improved performance-wise (this actually collects all children!)
                    set children [fieldsPage:getNodeInfo $w children $key]
                    return [expr [llength $children]!=0]
                }
            }
        }

        children {
            incr depth
            switch $keytype {
                obj -
                struct {
                    if {$obj==[opp_null]} {return ""}
                    if {$keytype=="obj"} {set sd [opp_getclassdescriptorfor $obj]}
                    if {$sd==[opp_null]} {return ""}

                    set children1 [fieldsPage:getNodeInfo:getFieldsInGroup $depth $obj $sd ""]
                    set children2 [fieldsPage:getNodeInfo:getGroupKeys $depth $obj $sd]
                    return [concat $children1 $children2]
                }

                group {
                    # return fields in the given group
                    set groupname [lindex $keyargs 4]
                    return [fieldsPage:getNodeInfo:getFieldsInGroup $depth $obj $sd $groupname]
                }

                field {
                    set fieldid [lindex $keyargs 4]
                    set isarray [opp_classdescriptor $obj $sd fieldisarray $fieldid]
                    if {$isarray} {
                        # expand array: enumerate all indices
                        return [fieldsPage:getNodeInfo:getElementsInArray $depth $obj $sd $fieldid]
                    }
                    set iscompound [opp_classdescriptor $obj $sd fieldiscompound $fieldid]
                    if {$iscompound} {
                        # return children on this class/struct
                        set ispoly [opp_classdescriptor $obj $sd fieldiscobject $fieldid]
                        set fieldptr [opp_classdescriptor $obj $sd fieldstructpointer $fieldid]
                        if [opp_isnull $fieldptr] {return ""}
                        if {$ispoly} {
                            return [fieldsPage:getNodeInfo $w children "$depth-obj-$fieldptr"]
                        } else {
                            set fielddesc [opp_classdescriptor $obj $sd fieldstructdesc $fieldid]
                            if {$fielddesc==""} {return ""}  ;# nothing known about it
                            set tmpkey "$depth-struct-$fieldptr-$fielddesc"
                            return [fieldsPage:getNodeInfo $w children $tmpkey]
                        }
                    }
                    return ""
                }

                findex {
                    set fieldid [lindex $keyargs 4]
                    set index [lindex $keyargs 5]
                    set iscompound [opp_classdescriptor $obj $sd fieldiscompound $fieldid]
                    if {$iscompound} {
                        # return children on this class/struct
                        set ispoly [opp_classdescriptor $obj $sd fieldiscobject $fieldid]
                        set fieldptr [opp_classdescriptor $obj $sd fieldstructpointer $fieldid $index]
                        if [opp_isnull $fieldptr] {return ""}
                        if {$ispoly} {
                            return [fieldsPage:getNodeInfo $w children "$depth-obj-$fieldptr"]
                        } else {
                            set fielddesc [opp_classdescriptor $obj $sd fieldstructdesc $fieldid]
                            if {$fielddesc==""} {return ""}  ;# nothing known about it
                            set tmpkey "$depth-struct-$fieldptr-$fielddesc"
                            return [fieldsPage:getNodeInfo $w children $tmpkey]
                        }
                    }
                    return ""
                }

                root {
                    return "0-obj-$obj"
                }

                default {
                    error "bad keytype '$keytype'"
                }
            }
        }

        root {
            # add an extra level to make the root object appear in the tree as well
            #return "0-obj-$treeroots($w)"
            return "0-root-$treeroots($w)"
        }

        selectionchanged {
            switch $keytype {
                obj {
                    if [opp_isnotnull $obj] {
                        fieldsPage:getNodeInfo:objectSelected $w $obj
                    }
                }
                field -
                findex {
                    set obj [fieldsPage:getNodeInfo:resolveObject $keyargs]
                    if [opp_isnotnull $obj] {
                        fieldsPage:getNodeInfo:objectSelected $w $obj
                    }
                }
            }
        }
    }
}

#
# Helper proc for fieldsPage:getNodeInfo.
# Collects field groups, and converts them to keys.
#
proc fieldsPage:getNodeInfo:getGroupKeys {depth obj sd} {
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
            lappend children "$depth-group-$obj-$sd-$groups($groupname)"
        }
    }
    return $children
}


#
# Helper proc for fieldsPage:getNodeInfo.
# Return fields in the given group; groupname may be "" (meaning no group).
#
proc fieldsPage:getNodeInfo:getFieldsInGroup {depth obj sd groupid} {
    set children {}
    if {$groupid!=""} {
        set groupname [opp_classdescriptor $obj $sd fieldproperty $groupid "group"]
    } else {
        set groupname ""
    }
    set numfields [opp_classdescriptor $obj $sd fieldcount]
    for {set i 0} {$i<$numfields} {incr i} {
        if {$groupname==[opp_classdescriptor $obj $sd fieldproperty $i "group"]} {
            lappend children "$depth-field-$obj-$sd-$i"
        }
    }
    return $children
}

#
# Helper proc for fieldsPage:getNodeInfo.
# Expands array by enumerating all indices, and returns the list of corresponding keys.
#
proc fieldsPage:getNodeInfo:getElementsInArray {depth obj sd fieldid} {
    set children {}
    set n [opp_classdescriptor $obj $sd fieldarraysize $fieldid]
    for {set i 0} {$i<$n} {incr i} {
        lappend children "$depth-findex-$obj-$sd-$fieldid-$i"
    }
    return $children
}

#
# Helper proc for fieldsPage:getNodeInfo. Produces text for a field.
#
proc fieldsPage:getNodeInfo:getFieldText {obj sd fieldid index} {

    set typename [opp_classdescriptor $obj $sd fieldtypename $fieldid]
    set isarray [opp_classdescriptor $obj $sd fieldisarray $fieldid]
    set iscompound [opp_classdescriptor $obj $sd fieldiscompound $fieldid]
    set ispoly [opp_classdescriptor $obj $sd fieldiscobject $fieldid]
    set isobject [opp_classdescriptor $obj $sd fieldiscownedobject $fieldid]
    set iseditable [opp_classdescriptor $obj $sd fieldiseditable $fieldid]

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

    # "editable" flag
    if {$iseditable} {
        set typenametext " \[...\] $typenametext"
    }

    if {$iscompound} {
        # if it's an object, try to say something about it...
        if {$ispoly} {
            set fieldobj [opp_classdescriptor $obj $sd fieldstructpointer $fieldid $index]
            if [opp_isnull $fieldobj] {return "$name = \bNULL\b$typenametext"}
            if {!$isobject || [opp_getobjectowner $fieldobj]==$obj} {
                set fieldobjname [opp_getobjectfullname $fieldobj]
            } else {
                set fieldobjname [opp_getobjectfullpath $fieldobj]
            }
            set fieldobjname [opp_getobjectfullname $fieldobj]
            set fieldobjclassname [opp_getobjectshorttypename $fieldobj]
            set fieldobjinfo [opp_getobjectinfostring $fieldobj]
            if {$fieldobjinfo!=""} {
                set fieldobjinfotext ": $fieldobjinfo"
            } else {
                set fieldobjinfotext ""
            }
            return "$name = \b($fieldobjclassname) $fieldobjname$fieldobjinfotext\b$typenametext"
        } else {
            # a value can be generated via operator<<
            if [catch {set value [opp_classdescriptor $obj $sd fieldvalue $fieldid $index]} err] {set value "<!> Error: $err"}
            if {$value==""} {
                return "$name$typenametext"
            } else {
                return "$name = \b$value\b$typenametext"
            }
        }
    } else {
        # plain field, return "name = value" text
        if [catch {set value [opp_classdescriptor $obj $sd fieldvalue $fieldid $index]} err] {set value "<!> Error: $err"}
        set enumname [opp_classdescriptor $obj $sd fieldproperty $fieldid "enum"]
        if {$enumname!=""} {
            append typename " - enum $enumname"
        }
        #if {$typename=="string"} {set value "\"$value\""}
        if {$typename=="string"} {set value "'$value'"}
        if {$value==""} {
            return "$name$typenametext"
        } else {
            return "$name = \b$value\b$typenametext"
        }
    }
}

#
# If the given key (in split form) identifies an object (cClassDescriptor
# isCObject), returns its pointer. Otherwise returns [opp_null].
#
#
proc fieldsPage:getNodeInfo:resolveObject {keyargs} {
    set depth [lindex $keyargs 0]
    set keytype [lindex $keyargs 1]

    if {$keytype=="field" || $keytype=="findex"} {
        set obj [lindex $keyargs 2]
        set sd [lindex $keyargs 3]
        set fieldid [lindex $keyargs 4]
        set index [lindex $keyargs 5]
        set isobject [opp_classdescriptor $obj $sd fieldiscownedobject $fieldid]
        set isarray [opp_classdescriptor $obj $sd fieldisarray $fieldid]

        if {$isobject && (!$isarray || $index!="")} {
            return [opp_classdescriptor $obj $sd fieldstructpointer $fieldid $index]
        }
    }
    return [opp_null]
}


#
# Invoked on hitting Enter or double-clicking in the tree, it opens
# an inspector for the given object. Or starts editing. Or at least
# opens/closes that tree branch.
#
proc fieldsPage:getNodeInfo:inspect {w key} {
    set keyargs [split $key "-"]
    set ptr [fieldsPage:getNodeInfo:resolveObject $keyargs]
    if [opp_isnotnull $ptr] {
        set insp [winfo parent [winfo parent [winfo parent $w]]]
        inspector:dblClick $insp $ptr
    } elseif [fieldsPage:getNodeInfo:isEditable $w $key] {
        fieldsPage:getNodeInfo:edit $w $key
    } else {
        Tree:toggle $w $key
    }
}

#
# Returns true if a tree node is editable (it's a field and its descriptor says editable)
#
proc fieldsPage:getNodeInfo:isEditable {w key} {
    set keyargs [split $key "-"]
    set keytype [lindex $keyargs 1]
    if {$keytype=="field" || $keytype=="findex"} {
        set obj [lindex $keyargs 2]
        set sd [lindex $keyargs 3]
        set fieldid [lindex $keyargs 4]
        return [opp_classdescriptor $obj $sd fieldiseditable $fieldid]
    }
    return 0
}

#
# Initiates in-place editing of a tree field
#
proc fieldsPage:getNodeInfo:edit {w key} {
    set id [lindex [$w find withtag "text-$key"] 1]
    if {$id!=""} {
        editCanvasLabel $w $id [list fieldsPage:getNodeInfo:setValue $w $key]
    }
}

#
# Sets the value of a tree field after in-place editing
#
proc fieldsPage:getNodeInfo:setValue {w key value} {
    set keyargs [split $key "-"]
    set keytype [lindex $keyargs 1]
    if {$keytype=="field" || $keytype=="findex"} {
        set obj [lindex $keyargs 2]
        set sd [lindex $keyargs 3]
        set fieldid [lindex $keyargs 4]
        set index [lindex $keyargs 5]
        #regexp {^"(.*)"$} $value match value  ;# strip quotes
        if [catch {opp_classdescriptor $obj $sd fieldsetvalue $fieldid $index $value} e] {
            Tree:build $w
            tk_messageBox -parent [winfo toplevel $w] -title "Tkenv" -icon warning -type ok -message "Error: $e"
            return
        }
    }
    Tree:build $w
    opp_refreshinspectors
}

proc fieldsPage:getNodeInfo:objectSelected {w obj} {
    set insp [winfo parent [winfo parent [winfo parent $w]]]
    mainWindow:selectionChanged $insp $obj
}

proc fieldsPage:getNodeInfo:popup {w key x y} {
    set insp [winfo parent [winfo parent [winfo parent $w]]]

    set keyargs [split $key "-"]
    set ptr [fieldsPage:getNodeInfo:resolveObject $keyargs]
    if [opp_isnotnull $ptr] {
        createInspectorContextMenu $insp $ptr
        .popup add separator
        .popup add command -label "Copy" -command [list fieldsPage:getNodeInfo:copy $w $key]
    } else {
        catch {destroy .popup}
        menu .popup -tearoff 0
        if [fieldsPage:getNodeInfo:isEditable $w $key] {
            .popup add command -label "Edit..." -command [list fieldsPage:getNodeInfo:edit $w $key]
        } else {
            .popup add command -label "Edit..." -state disabled
        }
        .popup add separator
        .popup add command -label "Copy" -command [list fieldsPage:getNodeInfo:copy $w $key]

        if [opp_isinspector $insp] {
           set ptr [opp_inspector_getobject $insp]
           if [opp_isnotnull $ptr] {
              set parentptr [opp_getobjectowner $ptr]
              if {[opp_isnotnull $parentptr] && [opp_inspector_supportsobject $insp $parentptr]} {
                  .popup add separator
                  .popup add command -label "Go Up" -command "opp_inspector_setobject $insp $parentptr"
              }
           }
        }

    }

    tk_popup .popup $x $y
}

proc fieldsPage:getNodeInfo:copy {w key} {
    set txt [fieldsPage:getNodeInfo $w text $key]
    regsub -all "\b" $txt "" txt

    # note: next two lines are from tk_textCopy Tk proc.
    clipboard clear -displayof $w
    clipboard append -displayof $w $txt
}

