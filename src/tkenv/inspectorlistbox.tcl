#=================================================================
#  INSPECTORLISTBOX.TCL - part of
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


proc createInspectorListbox {f insp} {
    global B2 B3

    ttk::frame $f.main
    pack $f.main -expand 1 -fill both -side top

    set lb $f.main.list
    ttk::treeview $lb -columns {name info ptr} -show {tree headings}
    $lb heading "#0" -anchor c -text "Class"   ;#TODO: -command [list inspectorListbox:sortBy $lb "#0" 0]
    $lb heading name -anchor w -text "Name"    -command [list inspectorListbox:sortBy $lb name 0]
    $lb heading info -anchor w -text "Info"    -command [list inspectorListbox:sortBy $lb info 0]
    $lb heading ptr  -anchor w -text "Pointer" -command [list inspectorListbox:sortBy $lb ptr 0]
    $lb column "#0"  -stretch 0 -width 140
    $lb column name  -stretch 0 -width 120
    $lb column info  -stretch 0 -width 300
    addScrollbars $lb

    bind $f.main.list <<TreeviewSelect>> [list inspectorListbox:selectionChanged $insp %W]
    bind $f.main.list <Double-Button-1> [list inspectorListbox:dblClick $insp %W]
    bind $f.main.list <Button-$B3> [list +inspectorListbox:rightClick $insp %W %X %Y]  ;# Note "+"! it appends this code to binding in widgets.tcl
    bind $f.main.list <Key-Return> [list inspectorListbox:dblClick $insp %W]

    focus $f.main.list

    return $f.main.list
}

# source: Tk "widget" demo
proc inspectorListbox:sortBy {tree col direction} {
    #TODO: cannot sort by the #0 column
    # Determine currently sorted column and its sort direction
    foreach c {"#0" name info ptr} {
        set s [$tree heading $c state]
        if {("selected" in $s || "alternate" in $s) && $col ne $c} {
            # Sorted column has changed
            $tree heading $c state {!selected !alternate !user1}
            set direction [expr {"alternate" in $s}]
        }
    }

    # Build something we can sort
    set data {}
    foreach row [$tree children {}] {
        lappend data [list [$tree set $row $col] $row]
    }

    set dir [expr {$direction ? "-decreasing" : "-increasing"}]
    set r -1

    # Now reshuffle the rows into the sorted order
    foreach info [lsort -dictionary -index 0 $dir $data] {
        $tree move [lindex $info 1] {} [incr r]
    }

    # Switch the heading so that it will sort in the opposite direction
    $tree heading $col -command [list inspectorListbox:sortBy $tree $col [expr {!$direction}]] \
        state [expr {$direction?"!selected alternate":"selected !alternate"}]
    if {[tk windowingsystem] eq "aqua"} {
        # Aqua theme displays native sort arrows when user1 state is set
        $tree heading $col state "user1"
    }
}

proc inspectorListbox:getSelection {lb} {
    set ptrs {}
    foreach item [$lb selection] {
        set values [$lb item $item -values]
        set ptr [lindex $values 2]
        lappend ptrs $ptr
    }
    return $ptrs
}

proc inspectorListbox:getCurrent {lb} {
    set ptrs [inspectorListbox:getSelection $lb]
    return [lindex $ptrs 0]
}

proc inspectorListbox:selectionChanged {insp lb} {
    set ptr [inspectorListbox:getCurrent $lb]
    if [opp_isnotnull $ptr] {
        mainWindow:selectionChanged $insp $ptr
    }
}

proc inspectorListbox:rightClick {insp lb X Y} {
    set ptr [inspectorListbox:getCurrent $lb]
    if [opp_isnotnull $ptr] {
        set popup [createInspectorContextMenu $insp $ptr]
        tk_popup $popup $X $Y
    }
}

proc inspectorListbox:dblClick {insp lb} {
    set ptr [inspectorListbox:getCurrent $lb]
    if [opp_isnotnull $ptr] {
        inspector:dblClick $insp $ptr
    }
}

proc inspectorListbox:storeColumnWidths {lb configkey} {
    global config
    set columnwidths [list "#0" [$lb column "#0" -width]]
    foreach col [$lb cget -columns] {
        lappend columnwidths $col [$lb column $col -width]
    }
    set config($configkey) $columnwidths
}

proc inspectorListbox:restoreColumnWidths {lb configkey} {
    global config
    if [info exists config($configkey)] {
        foreach {col width} $config($configkey) {
            $lb column $col -width $width
        }
    }
}


