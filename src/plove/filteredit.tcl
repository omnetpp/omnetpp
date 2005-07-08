#=================================================================
#  FILTEREDIT.TCL - part of
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
# Readonly dialog. Shows properties of a builtin filter.
#
proc filterDetails {name parentw} {
    global g

    set w $parentw.editfilter
    createOkCancelDialog $w "Filter Properties"
    wm geometry $w 400x400

    set nb $w.f.nb
    notebook $nb
    notebook_addpage $nb gen "General"
    pack $nb -expand 1 -fill both

    #
    # "General" page
    #

    # add entry fields and focus on first one
    labelframe $nb.gen.f0 -text "General"
    label-sunkenlabel $nb.gen.f0.name "Name:" $name
    label-sunkenlabel $nb.gen.f0.descr "Description:" [opp_nodetype $name description]
    grid $nb.gen.f0.name -sticky new
    grid $nb.gen.f0.descr -sticky new
    grid columnconfig $nb.gen.f0 0 -weight 1

    labelframe $nb.gen.f1 -text "Filter Parameters"
    set plb $nb.gen.f1.params
    multicolumnlistbox $plb {
        {name   Parameter  80}
        {default Default}
        {descr  Description}
    }
    grid $nb.gen.f1.params - - - -sticky news
    grid columnconfig $nb.gen.f1 0 -weight 1
    grid rowconfig $nb.gen.f1 0 -weight 1

    pack $nb.gen.f0 -anchor center -expand 1 -fill both -side top -padx 5 -pady 5
    pack $nb.gen.f1 -anchor center -expand 1 -fill both -side top -padx 5 -pady 5

    # fill listbox with data
    foreach par [opp_nodetype $name attrs] {
        set parname [lindex $par 0]
        set descr [lindex $par 1]
        set default [lindex $par 2]
        multicolumnlistbox_insert $plb $parname [list name $parname descr $descr default $default]
    }

    focus $nb.gen.f0.name.e

    #pack forget $w.buttons.cancelbutton

    # exec the dialog, then delete it
    execOkCancelDialog $w
    destroy $w
    return ""
}

#
# Dialog for editing a filter. Returns new name of filter (name might have been
# edited by user). Strategy: we clone the filter under the name "%tmp%"
# dialog modifies "%tmp%" filter and "OK" button overwrites original filter with "%tmp%".
#
proc editFilter {name parentw} {
    global g

    # make temporary copy of the filter, fill listboxes with data
    if {[opp_nodetype $name exists]} {
        opp_compoundfiltertype $name clone "%tmp%"
    } else {
        opp_compoundfiltertype_create "%tmp%"
    }
    opp_compoundfiltertype "%tmp%" setHidden 1

    set w $parentw.editfilter
    createOkCancelDialog $w "Edit Filter"
    wm geometry $w 400x400

    set nb $w.f.nb
    notebook $nb
    notebook_addpage $nb gen "General"
    notebook_addpage $nb int "Components"
    pack $nb -expand 1 -fill both

    #
    # "General" page
    #

    # add entry fields and focus on first one
    label $nb.gen.help -justify left -text "Here you can assemble a compound filter by cascading existing ones."
    labelframe $nb.gen.f0 -text "General"
    label-entry $nb.gen.f0.name "Name:" $name
    label-text $nb.gen.f0.descr "Description:" 3 [opp_nodetype "%tmp%" description]
    grid $nb.gen.f0.name -sticky new
    grid $nb.gen.f0.descr -sticky new
    grid columnconfig $nb.gen.f0 0 -weight 1

    labelframe $nb.gen.f1 -text "Filter parameters (can be passed to components)"
    set plb $nb.gen.f1.params
    multicolumnlistbox $plb {
        {name   Parameter  80}
        {default Default}
        {descr  Description}
    }
    grid $nb.gen.f1.params - - - -sticky news
    button $nb.gen.f1.new  -text " Add... "  -width 8 -command "editFilter_addPar $plb"
    button $nb.gen.f1.edit -text " Edit... " -width 8 -command "editFilter_editPar $plb"
    button $nb.gen.f1.del  -text " Delete "  -width 8 -command "editFilter_deletePar $plb"

    grid x $nb.gen.f1.new $nb.gen.f1.edit $nb.gen.f1.del -sticky news -padx 5 -pady 5
    grid columnconfig $nb.gen.f1 0 -weight 1
    grid rowconfig $nb.gen.f1 0 -weight 1

    pack $nb.gen.help -anchor w -expand 0 -fill none -side top -padx 5 -pady 5
    pack $nb.gen.f0 -anchor center -expand 1 -fill both -side top -padx 5 -pady 5
    pack $nb.gen.f1 -anchor center -expand 1 -fill both -side top -padx 5 -pady 5

    #
    # "Internals" page
    #

    # add entry fields and focus on first one
    label $nb.int.help -justify left -text "Subfilters will be applied in top-down order. They can refer to parameters defined on the previous page."
    labelframe $nb.int.f1 -text "Composed of filters"
    set flb $nb.int.f1.filters
    multicolumnlistbox $flb {
        {index    #          20}
        {name     Filter     60}
        {params   Parameters}
    }

    button $nb.int.f1.app  -text " Append... "  -width 8 -command "editFilter_appendSubfilter $flb"
    button $nb.int.f1.ins  -text " Insert... "  -width 8 -command "editFilter_insertSubfilter $flb"
    button $nb.int.f1.edit -text " Edit... " -width 8 -command "editFilter_editSubfilter $flb"
    button $nb.int.f1.del  -text " Delete "  -width 8 -command "editFilter_deleteSubfilter $flb"

    grid $nb.int.f1.filters - - - - -sticky news
    grid x $nb.int.f1.app $nb.int.f1.ins $nb.int.f1.edit $nb.int.f1.del -sticky news -padx 5 -pady 5
    grid columnconfig $nb.int.f1 0 -weight 1
    grid rowconfig $nb.int.f1 0 -weight 1

    pack $nb.int.help -anchor w -expand 0 -fill none -side top -padx 5 -pady 5
    pack $nb.int.f1 -anchor center -expand 1 -fill both -side top -padx 5 -pady 5

    bind $plb <Double-Button-1> "$nb.gen.f1.edit invoke"
    bind $plb <Key-Return> "$nb.gen.f1.edit invoke"

    bind $flb <Double-Button-1> "$nb.int.f1.edit invoke"
    bind $flb <Key-Return> "$nb.int.f1.edit invoke"

    focus $nb.gen.f0.name.e
    notebook_showpage $nb gen

    # fill dialog with data
    foreach par [opp_nodetype "%tmp%" attrs] {
        set parname [lindex $par 0]
        set descr [lindex $par 1]
        set default [lindex $par 2]
        multicolumnlistbox_insert $plb $parname [list name $parname descr $descr default $default]
    }

    editFilter_refreshSubfilterListbox $flb

    # exec the dialog, extract its contents if OK was pressed, then delete it
    if {[execOkCancelDialog $w] == 1} {
        set descr [string trim [$nb.gen.f0.descr.t get 1.0 end]]
        opp_compoundfiltertype "%tmp%" setDescription $descr

        set newname [string trim [$nb.gen.f0.name.e get]]
        # newname cannot contain space etc (BLT doesn't like it)
        regsub -all -- {[^-_a-zA-Z0-9]} $newname {_} newname
        if {$name!=$newname && [opp_nodetype $newname exists]} {
            tk_messageBox -parent $w -type ok -icon info \
                  -title "Filter" -message "A filter named \"$newname\" already exists -- keeping name \"$name.\""
            set newname $name
        }

        # rename "%tmp%" to new name (and delete old one if exists)
        catch {opp_compoundfiltertype $name delete}
        opp_compoundfiltertype "%tmp%" rename $newname
        opp_compoundfiltertype $newname setHidden 0
        destroy $w
        return $newname
    }

    opp_compoundfiltertype "%tmp%" delete
    destroy $w
    return ""
}

proc editFilter_addPar {plb} {
    set data [editFilterParDialog [winfo toplevel $plb] {name "" descr "" default ""}]
    if {$data!=""} {
        array set ary $data
        if [multicolumnlistbox_hasrow $plb $ary(name)] {
            tk_messageBox -parent [winfo toplevel $plb] -type ok -icon warning \
              -title "Duplicate Parameter" -message "Parameter \"$ary(name)\" already exists!"
        } else {
            multicolumnlistbox_insert $plb $ary(name) $data
            opp_compoundfiltertype "%tmp%" setAttr $ary(name) $ary(descr) $ary(default)
        }
    }
}

proc editFilter_editPar {plb} {
    set parname [multicolumnlistbox_curselection $plb]
    if {$parname == ""} return

    set data [multicolumnlistbox_getrow $plb $parname]
    set data [editFilterParDialog [winfo toplevel $plb] $data]
    if {$data!=""} {
        array set ary $data
        if {$ary(name)==$parname} {
            multicolumnlistbox_modify $plb $parname $data
            opp_compoundfiltertype "%tmp%" setAttr $ary(name) $ary(descr) $ary(default)
        } elseif [multicolumnlistbox_hasrow $plb $ary(name)] {
            tk_messageBox -parent [winfo toplevel $plb] -type ok -icon warning \
              -title "Duplicate Parameter" -message "Parameter \"$ary(name)\" already exists!"
        } else {
            multicolumnlistbox_delete $plb $parname
            multicolumnlistbox_insert $plb $ary(name) $data
            opp_compoundfiltertype "%tmp%" removeAttr $parname
            opp_compoundfiltertype "%tmp%" setAttr $ary(name) $ary(descr) $ary(default)
        }
    }
}

proc editFilter_deletePar {plb} {
    set parname [multicolumnlistbox_curselection $plb]
    if {$parname == ""} return
    multicolumnlistbox_delete $plb $parname
    opp_compoundfiltertype "%tmp%" removeAttr $parname
}

proc editFilterParDialog {parentw data} {
    set w $parentw.editpar
    createOkCancelDialog $w "Filter Parameter"

    array set ary $data

    # add entry fields and focus on first one
    label-entry $w.f.name "Name:" $ary(name)
    label-entry $w.f.def "Default value:" $ary(default)
    label-text $w.f.descr "Description:" 3 $ary(descr)
    grid $w.f.name -sticky new
    grid $w.f.def -sticky new
    grid $w.f.descr -sticky new
    grid columnconfig $w.f 0 -weight 1

    focus $w.f.name.e

    # exec the dialog, extract its contents if OK was pressed, then delete it
    if {[execOkCancelDialog $w] == 1} {
        set parname [string trim [$w.f.name.e get]]
        regsub -all -- {[^-_a-zA-Z0-9]} $parname {_} parname
        set def [string trim [$w.f.def.e get]]
        set descr [string trim [$w.f.descr.t get 1.0 end]]
        destroy $w
        return [list name $parname descr $descr default $def]
    }
    destroy $w
    return {}
}

proc editFilter_appendSubfilter {flb} {
    set k [opp_compoundfiltertype "%tmp%" numSubfilters]
    opp_compoundfiltertype "%tmp%" insertSubfilter $k
    set ok [editSubfilterDialog [winfo toplevel $flb] $k]
    if {!$ok} {
        opp_compoundfiltertype "%tmp%" removeSubfilter $k
    }
    editFilter_refreshSubfilterListbox $flb
}

proc editFilter_insertSubfilter {flb} {
    set k [multicolumnlistbox_curselection $flb]
    if {$k==""} {set k 0}
    opp_compoundfiltertype "%tmp%" insertSubfilter $k
    set ok [editSubfilterDialog [winfo toplevel $flb] $k]
    if {!$ok} {
        opp_compoundfiltertype "%tmp%" removeSubfilter $k
    }
    editFilter_refreshSubfilterListbox $flb
}

proc editFilter_editSubfilter {flb} {
    set k [multicolumnlistbox_curselection $flb]
    if {$k==""} return
    editSubfilterDialog [winfo toplevel $flb] $k
    editFilter_refreshSubfilterListbox $flb
}

proc editFilter_deleteSubfilter {flb} {
    set k [multicolumnlistbox_curselection $flb]
    if {$k==""} return
    opp_compoundfiltertype "%tmp%" removeSubfilter $k
    editFilter_refreshSubfilterListbox $flb
}

proc editFilter_refreshSubfilterListbox {flb} {
    multicolumnlistbox_deleteall $flb
    set n [opp_compoundfiltertype "%tmp%" numSubfilters]
    for {set k 0} {$k<$n} {incr k} {
        set name [opp_compoundfiltertype "%tmp%" subfilterType $k]
        set params {}
        foreach attr [opp_compoundfiltertype "%tmp%" subfilterAttrs $k] {
            lappend params "[lindex $attr 0]=[lindex $attr 1]"
        }
        set params [join $params ", "]
        multicolumnlistbox_insert $flb "$k" [list index $k name $name params $params]
    }
}

proc editSubfilterDialog {parentw k} {
    global tmp

    set w $parentw.editsubfilt
    createOkCancelDialog $w "Subfilter Properties"
    wm geometry $w 400x300

    # filter selector
    set name [opp_compoundfiltertype "%tmp%" subfilterType $k]

    # filter description
    labelframe $w.f.f0 -text "Filter description"
    label $w.f.f0.desc -justify left
    pack $w.f.f0.desc -expand 0 -fill none -anchor w

    # add entry fields and focus on first one
    labelframe $w.f.f1 -text "Assign filter parameters"
    set plb $w.f.f1.params
    multicolumnlistbox $plb {
        {name   Parameter  60}
        {value  Value     100}
        {descr  Description}
    }
    set tmp(lastfiltercombovalue) ""
    set visibletypes [opp_getnodetypes]
    label-combo $w.f.name "Type:" $visibletypes $name "editSubfilter_refreshParamsListbox $w.f.name.e $plb $k $w.f.f0.desc"
    #FIXME show description here instead

    grid $w.f.f1.params - -sticky news
    button $w.f.f1.edit -text " Change... " -width 8 -command "editSubfilter_changePar $plb"

    grid x $w.f.f1.edit -sticky news -padx 5 -pady 5
    grid columnconfig $w.f.f1 0 -weight 1
    grid rowconfig $w.f.f1 0 -weight 1

    pack $w.f.name -anchor center -expand 0 -fill x -side top -padx 5 -pady 5
    pack $w.f.f0 -anchor w -expand 0 -fill x -side top -padx 5 -pady 5
    pack $w.f.f1 -anchor center -expand 1 -fill both -side top -padx 5 -pady 5

    bind $plb <Double-Button-1> "$w.f.f1.edit invoke"
    bind $plb <Key-Return> "$w.f.f1.edit invoke"

    focus $w.f.name.e

    # exec the dialog, extract its contents if OK was pressed, then delete it
    if {[execOkCancelDialog $w] == 1} {
        set name [string trim [$w.f.name.e get]]
        opp_compoundfiltertype "%tmp%" setSubfilter $k $name ""

        foreach attr [opp_compoundfiltertype "%tmp%" subfilterAttrs $k] {
            set name [lindex $attr 0]
            opp_compoundfiltertype "%tmp%" removeSubfilterAttr $k $name
        }
        foreach row [multicolumnlistbox_getrownames $plb] {
            set data [multicolumnlistbox_getrow $plb $row]
            array set ary $data
            opp_compoundfiltertype "%tmp%" setSubfilterAttr $k $ary(name) $ary(value)
        }
        #FIXME try subfilter validation here:
        #opp_compoundfiltertype "%tmp%" validateSubfilter $k

        destroy $w
        return 1
    }
    destroy $w
    return 0
}

proc editSubfilter_changePar {plb} {
    set parname [multicolumnlistbox_curselection $plb]
    if {$parname == ""} return

    set data [multicolumnlistbox_getrow $plb $parname]
    array set ary $data

    set ary(value) [editSubfilterParDialog [winfo toplevel $plb] $ary(name) $ary(value) $ary(descr)]
    if {$ary(value)!=""} {
        set data [array get ary]
        multicolumnlistbox_modify $plb $parname $data
    }
}

proc editSubfilter_refreshParamsListbox {combo plb k filtdesclabel} {
    global tmp

    # eliminate a subtle UI bug: when closing the dialog by pressing Enter, filter
    # parameters got lost because this proc reloaded the $plb listbox content.
    if {[$combo get]==$tmp(lastfiltercombovalue)} return
    set tmp(lastfiltercombovalue) [$combo get]

    # get all values currently stored into values[]
    foreach attr [opp_compoundfiltertype "%tmp%" subfilterAttrs $k] {
        set name [lindex $attr 0]
        set value [lindex $attr 1]
        set values($name) $value
    }

    # pick which attrs current filter type has, and fill into listbox
    set filttype [string trim [$combo get]]
    multicolumnlistbox_deleteall $plb
    if {![opp_nodetype $filttype exists]} {
        set filtTypeAttrs {}
        $filtdesclabel config -text "(no such filter)"
    } else {
        set filtTypeAttrs [opp_nodetype $filttype attrs]
        $filtdesclabel config -text [opp_nodetype $filttype description]
    }
    foreach attr $filtTypeAttrs {
        set name [lindex $attr 0]
        set descr [lindex $attr 1]
        set value [lindex $attr 2]
        catch {set value $values($name)}
        multicolumnlistbox_insert $plb $name [list name $name value $value descr $descr]
    }
}

proc editSubfilterParDialog {parentw name value descr} {
    set w $parentw.editpar
    createOkCancelDialog $w "Assign Filter Parameter"

    set choices {0 1}
    foreach par [opp_nodetype "%tmp%" attrs] {
        lappend choices [lindex $par 0]
    }
    if {[lsearch $choices $value]==-1} {
        set choices [concat [list $value] $choices]
    }
    label-combo $w.f.value "Parameter $name" $choices $value
    label-sunkenlabel $w.f.descr "Description" $descr
    grid $w.f.value -sticky new
    grid $w.f.descr -sticky new
    grid columnconfig $w.f 0 -weight 1

    focus $w.f.value.e

    # exec the dialog, extract its contents if OK was pressed, then delete it
    if {[execOkCancelDialog $w] == 1} {
        set value [$w.f.value.e get]
        destroy $w
        return $value
    }
    destroy $w
    return {}
}

