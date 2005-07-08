#=================================================================
#  FILTER.TCL - part of
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

proc same {ids field default} {
    global vec

    set f $vec([lindex $ids 0],$field)
    foreach id $ids {
        if {[string compare $f $vec($id,$field)]!=0} {
            return $default
        }
    }
    return $f
}


proc generateUniqFilterName {} {
    # find unique name
    set i 0
    while 1 {
        incr i
        if {![opp_nodetype "__vecfilter$i" exists]} break
    }
    return "__vecfilter$i"
}

proc editFilterForVector {ids} {
    global vec

    set filtname ""
    if {[llength $ids]==0} {
        return
    } elseif {[llength $ids]==1} {
        set filtname $vec($ids,filter)
    } elseif {[llength $ids]>1} {
        # check if all of them have the same filter (and fill $filtname)
        set numwithoutfilter 0
        set filtermismatch 0
        foreach id $ids {
            if {$vec($id,filter)==""} {
                incr numwithoutfilter
            } elseif {$filtname==""} {
                set filtname $vec($id,filter)
            } elseif {![opp_compoundfiltertype $vec($id,filter) equals $filtname]} {
                set filtermismatch 1
            }
        }
        if {$filtermismatch} {
            tk_messageBox -type ok -icon info -title "Filters" \
                -message "Not all selected vectors share the same filter settings. The next dialog will apply the same filter settings to all vectors."
        } elseif {$filtname!="" && $numwithoutfilter>0} {
            tk_messageBox -type ok -icon info -title "Filters" \
                -message "Some of the selected vectors have no filter set. The next dialog will apply the same filter settings to all vectors."
        }
    }

    if {$filtname==""} {
        set filtname [generateUniqFilterName]
    }

    if {![opp_nodetype $filtname exists] || [opp_compoundfiltertype $filtname numSubfilters]==1} {
        set ok [editFilterForVectorDialog $filtname .]
    } else {
        set ok [editFilterForVectorAdvancedDialog $filtname .]
    }

    if {$ok} {
        set nsubfilt [opp_compoundfiltertype $filtname numSubfilters]
        if {$nsubfilt==0 || ($nsubfilt==1 && [opp_compoundfiltertype $filtname subfilterType 0]=="")} {
            # remove filter from all vectors
            foreach id $ids {
                if {$vec($id,filter)!=""} {
                    if {$vec($id,filter)!=$filtname} {
                        opp_compoundfiltertype $vec($id,filter) delete
                    }
                    set vec($id,filter) ""
                }
            }
            opp_compoundfiltertype $filtname delete
        } else {
            # every vector gets a *copy* of this filter
            foreach id $ids {
                if {$vec($id,filter)!="" && $vec($id,filter)!=$filtname} {
                    opp_compoundfiltertype $vec($id,filter) delete
                }
                set newname [generateUniqFilterName]
                opp_compoundfiltertype $filtname clone $newname
                set vec($id,filter) $newname
            }
            opp_compoundfiltertype $filtname delete
        }

        #foreach id $ids {
        #    debug "vec$id --> \"$vec($id,filter)\""
        #}

        #set title  ...
        #set prefix ...
        #set mult  [expr [llength $ids]!=1]
        #foreach id $ids {
        #    if $mult {
        #        set vec($id,title) [cut_prefix $vec($id,title) $vec($id,filtpfx)]
        #        set vec($id,title)  "$prefix$vec($id,title)"
        #        set vec($id,filtpfx) $prefix
        #    } else {
        #        set vec($id,title) $title
        #        set vec($id,filtpfx) $prefix
        #    }
        #}

        return 1
    }
}

proc editFilterForVectorDialog {name parentw} {
    global tmp

    # make temporary copy of the filter, fill listboxes with data
    if {[opp_nodetype $name exists]} {
        opp_compoundfiltertype $name clone "%tmp%"
    } else {
        opp_compoundfiltertype_create "%tmp%"
        opp_compoundfiltertype "%tmp%" insertSubfilter 0
    }
    opp_compoundfiltertype "%tmp%" setHidden 1

    set w $parentw.editfilt
    if {$parentw=="."} {set w .editfilt}
    createOkCancelDialog $w "Vector Filter"
    wm geometry $w 400x300

    set sfname [opp_compoundfiltertype "%tmp%" subfilterType 0]

    # filter description
    labelframe $w.f.f0 -text "Filter description"
    label $w.f.f0.desc -justify left
    pack $w.f.f0.desc -expand 0 -fill none -anchor w

    # add entry fields and focus on first one
    labelframe $w.f.f1 -text "Assign filter parameters"
    set plb $w.f.f1.params
    multicolumnlistbox $plb {
        {name   Parameter   60}
        {value  Value       100}
        {descr  Description}
    }
    set tmp(lastfiltercombovalue) ""
    set visibletypes [concat [list ""] [opp_getnodetypes]]
    label-combo $w.f.name "Apply filter:" $visibletypes $sfname "editSubfilter_refreshParamsListbox $w.f.name.e $plb 0 $w.f.f0.desc"

    grid $w.f.f1.params - -sticky news
    button $w.f.f1.edit -text " Change... " -width 8 -command "editSubfilter_changePar $plb"

    grid x $w.f.f1.edit -sticky news -padx 5 -pady 5
    grid columnconfig $w.f.f1 0 -weight 1
    grid rowconfig $w.f.f1 0 -weight 1

    pack $w.f.name -anchor center -side top -expand 0 -fill x -padx 5 -pady 5
    pack $w.f.f0 -anchor center -side top -expand 0 -fill both -padx 5 -pady 5
    pack $w.f.f1 -anchor center -side top -expand 1 -fill both -padx 5 -pady 5

    set tmp(advanced) 0
    button $w.buttons.adv -text " Advanced..." -command "set tmp(advanced) 1; $w.buttons.cancelbutton invoke"
    pack $w.buttons.adv -anchor n -side right -expand 0 -fill none -padx 2

    bind $plb <Double-Button-1> "$w.f.f1.edit invoke"
    bind $plb <Key-Return> "$w.f.f1.edit invoke"

    focus $w.f.name.e

    # exec the dialog, extract its contents if OK was pressed, then delete it
    if {[execOkCancelDialog $w] == 1} {
        set sfname [string trim [$w.f.name.e get]]
        opp_compoundfiltertype "%tmp%" setSubfilter 0 $sfname ""
        foreach row [multicolumnlistbox_getrownames $plb] {
            set data [multicolumnlistbox_getrow $plb $row]
            array set ary $data
            opp_compoundfiltertype "%tmp%" setSubfilterAttr 0 $ary(name) $ary(value)
        }

        # rename "%tmp%" to name (and delete old one if exists)
        catch {opp_compoundfiltertype $name delete}
        opp_compoundfiltertype "%tmp%" rename $name
        destroy $w
        return 1
    }
    opp_compoundfiltertype "%tmp%" delete
    destroy $w

    if {!$tmp(advanced)} {
        return 0
    } else {
        return [editFilterForVectorAdvancedDialog $name $parentw]
    }
}

#
# Dialog for editing a filter for a vector. Strategy: we clone the filter
# under the name "%tmp%"; dialog modifies "%tmp%" filter and "OK" button
# overwrites original filter with "%tmp%".
#
proc editFilterForVectorAdvancedDialog {name parentw} {
    global g

    # make temporary copy of the filter, fill listboxes with data
    if {[opp_nodetype $name exists]} {
        opp_compoundfiltertype $name clone "%tmp%"
    } else {
        opp_compoundfiltertype_create "%tmp%"
    }
    opp_compoundfiltertype "%tmp%" setHidden 1

    set w $parentw.editfilter
    if {$parentw=="."} {set w .editfilter}
    createOkCancelDialog $w "Vector Filter - Advanced"
    wm geometry $w 400x400

    set f $w.f

    # add entry fields and focus on first one
    label $f.help -justify left -text "You can add several filters, and they will be applied in top-down order."
    labelframe $f.f1 -text "Filters"
    set flb $f.f1.filters
    multicolumnlistbox $flb {
        {index    #          20}
        {name     Filter     60}
        {comment  Comment    60}
        {params   Parameters}
    }

    grid $f.f1.filters - - - - -sticky news
    button $f.f1.app  -text " Append... "  -width 8 -command "editFilter_appendSubfilter $flb"
    button $f.f1.ins  -text " Insert... "  -width 8 -command "editFilter_insertSubfilter $flb"
    button $f.f1.edit -text " Edit... " -width 8 -command "editFilter_editSubfilter $flb"
    button $f.f1.del  -text " Delete "  -width 8 -command "editFilter_deleteSubfilter $flb"

    grid x $f.f1.app $f.f1.ins $f.f1.edit $f.f1.del -sticky news -padx 5 -pady 5
    grid columnconfig $f.f1 0 -weight 1
    grid rowconfig $f.f1 0 -weight 1

    pack $f.help -anchor w -expand 0 -fill none -side top -padx 5 -pady 5
    pack $f.f1 -anchor center -expand 1 -fill both -side top -padx 5 -pady 5

    bind $flb <Double-Button-1> "$f.f1.edit invoke"
    bind $flb <Key-Return> "$f.f1.edit invoke"

    focus $flb

    # fill dialog with data
    foreach par [opp_nodetype "%tmp%" attrs] {
        set parname [lindex $par 0]
        set descr [lindex $par 1]
        multicolumnlistbox_insert $plb $parname [list name $parname descr $descr]
    }

    editFilter_refreshSubfilterListbox $flb

    # exec the dialog, extract its contents if OK was pressed, then delete it
    if {[execOkCancelDialog $w] == 1} {
        # rename "%tmp%" to name (and delete old one if exists)
        catch {opp_compoundfiltertype $name delete}
        opp_compoundfiltertype "%tmp%" rename $name
        destroy $w
        return 1
    }

    opp_compoundfiltertype "%tmp%" delete
    destroy $w
    return 0
}

