#=================================================================
#  FILTERLIST.TCL - part of
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
# Dialog for viewing/editing list of available filters
#
proc filterListDialog {} {
    global g

    set w .filtcfg
    createCloseDialog $w "Filter List"
    wm geometry $w 500x300

    #
    # listbox
    #
    set f $w.f.f
    labelframe $f -text "Filters"
    set lb $f.list
    multicolumnlistbox $f.list {
        {name Name 80}
        {type Type 60}
        {description Description}
    } -yscrollcommand "$f.sby set" -width 500
    scrollbar $f.sby -borderwidth 1 -command "$f.list yview"

    button $f.new  -text New...  -width 8 -command "filterList_newFilter $lb"
    button $f.dup  -text Clone   -width 8  -command "filterList_dupFilter $lb"
    button $f.edit -text Details... -width 8 -command "filterList_editFilter $lb"
    button $f.del  -text Delete  -width 8 -command "filterList_deleteFilter $lb"

    grid $f.list - - - - $f.sby -sticky news
    grid x $f.new $f.dup $f.edit $f.del - -sticky n -padx 5 -pady 5
    grid columnconfig $f 0 -weight 1
    grid rowconfig $f 0 -weight 1

    pack $f -expand 1 -fill both -side top -padx 5 -pady 5

    #
    # fill dialog (use opp_getnodetypes -all to view hidden filters too)
    #
    foreach i [opp_getnodetypes] {
        set cat [opp_nodetype $i category]
        set desc [opp_nodetype $i description]
        multicolumnlistbox_insert $lb $i [list name $i type $cat description $desc]
    }

    focus $lb

    #
    # bindings
    #
    bind $lb <Double-1> "filterList_editFilter $lb"
    bind $lb <3> "filterList_editFilter $lb"
    bind $w <Return> "filterList_editFilter $lb"
    bind $w <Delete> "filterList_deleteFilter $lb"

    execCloseDialog $w
    destroy $w
}

proc filterList_deleteFilter {lb} {
    global g

    set name [multicolumnlistbox_curselection $lb]
    if {$name == ""} return

    if {![opp_nodetype $name exists]} return

    if {[opp_nodetype $name category]!="custom filter"} {
        tk_messageBox -parent $lb -type ok -icon warning -title "Filter" -message "Builtin filters cannot be changed, cloned or deleted."
        return
    }

    opp_compoundfiltertype $name delete
    multicolumnlistbox_delete $lb $name
}

proc filterList_dupFilter {lb} {
    global g

    set name [multicolumnlistbox_curselection $lb]
    if {$name == ""} return

    if {![opp_nodetype $name exists]} return

    if {[opp_nodetype $name category]!="custom filter"} {
        tk_messageBox -parent $lb -type ok -icon warning -title "Filter" -message "Builtin filters cannot be changed, cloned or deleted."
        return
    }

    # find unique name
    set oldname $name
    if {[opp_nodetype $name exists]} {
        set i 0
        while 1 {
            incr i
            if {![opp_nodetype "$oldname$i" exists]} break
        }
        set name "$oldname$i"
    }

    opp_compoundfiltertype $oldname clone $name
    set cat [opp_nodetype $name category]
    set desc [opp_nodetype $name description]
    multicolumnlistbox_insert $lb $name [list name $name type $cat description $desc]
}

proc filterList_newFilter {lb} {
    global g

    # find unique name
    set name "new"
    if {[opp_nodetype "new" exists]} {
        set i 0
        while 1 {
            incr i
            if {![opp_nodetype "new$i" exists]} break
        }
        set name "new$i"
    }
    set w .filtcfg
    set newname [editFilter $name $w]
    if {$newname!=""} {
        set cat [opp_nodetype $newname category]
        set desc [opp_nodetype $newname description]
        multicolumnlistbox_insert $lb $newname [list name $newname type $cat description $desc]
    }
}

proc filterList_editFilter {lb} {
    global g

    set name [multicolumnlistbox_curselection $lb]
    if {$name == ""} return

    if {![opp_nodetype $name exists]} return

    set w .filtcfg
    if {[opp_nodetype $name category]!="custom filter"} {
        filterDetails $name $w
        return
    }

    set newname [editFilter $name $w]
    if {$newname==""} {
        return
    }
    if {$newname==$name} {
        set cat [opp_nodetype $newname category]
        set desc [opp_nodetype $newname description]
        multicolumnlistbox_modify $lb $newname [list name $newname type $cat description $desc]
    } else {
        set cat [opp_nodetype $newname category]
        set desc [opp_nodetype $newname description]
        multicolumnlistbox_delete $lb $name
        catch {multicolumnlistbox_delete $lb $newname}
        multicolumnlistbox_insert $lb $newname [list name $newname type $cat description $desc]
    }
}

