#=================================================================
#  SCALARTABLE.TCL - part of
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


proc loadScalarFile {fname} {
    if {![file readable $fname] || [file type $fname]=="directory"} {
        tk_messageBox -icon warning -type ok -title Error \
             -message "File $fname doesn't exist or there's no read permission."
        return
    }

    busyCursor "Loading $fname..."

    debug "loading file"
    if [catch {opp_loadScalar $fname} err] {
        idleCursor
        tk_messageBox -icon warning -type ok -title Error -message "Error reading $fname: $err"
        return
    }

    debug "refreshing filter combos"
    refreshFilters

    idleCursor

    debug "applying filter"
    applyFilter
}

proc refreshFilters {} {
    debug "  getting lists"
    set filelist [lsort [opp_getFileAndRunList]]
    set modulelist [lsort [opp_getModuleList]]
    set namelist [lsort [opp_getScalarNameList]]

    # generate a few hints for wildcard usage
    # first for file names
    foreach fname $filelist {
        regsub {#.*} $fname {#*} fname
        set fnames($fname) 1
    }
    set filewildcardwords [lsort [array names fnames]]

    # then for module names
    foreach mod $modulelist {
        foreach w [split "!$mod!" "."] {
            if {[string match {*\[*} $w]} {
                regsub {\[.*\]} $w {[*]} w
            }
            set w "*.$w.*"
            if [catch {incr mwords($w)}] {set mwords($w) 1}
        }
    }
    set modulewildcardwords {}
    foreach w [array names mwords] {
        if {$mwords($w)>=2} {
            lappend modulewildcardwords $w
        }
    }
    set modulewildcardwords [lsort $modulewildcardwords]
    regsub -all -- {\*\.!} $modulewildcardwords {} modulewildcardwords
    regsub -all -- {!\.\*} $modulewildcardwords {} modulewildcardwords

    # then the same for scalar names
    foreach name $namelist {
        foreach w [split "!$name!" " "] {
            set w "* $w *"
            if [catch {incr nwords($w)}] {set nwords($w) 1}
        }
    }
    set namewildcardwords {}
    foreach w [array names nwords] {
        if {$nwords($w)>=2} {
            lappend namewildcardwords $w
        }
    }
    set namewildcardwords [lsort $namewildcardwords]
    regsub -all -- {\* !} $namewildcardwords {} namewildcardwords
    regsub -all -- {! \*} $namewildcardwords {} namewildcardwords

    # fill combo boxes
    set filelist [concat "*" $filewildcardwords $filelist]
    set modulelist [concat "*" $modulewildcardwords $modulelist]
    set namelist [concat "*" $namewildcardwords $namelist]

    debug "  updating combo boxes"
    comboconfig .main.f.filter.pars.fileentry $filelist
    comboconfig .main.f.filter.pars.moduleentry $modulelist
    comboconfig .main.f.filter.pars.nameentry $namelist
}


proc getFilteredList {} {
    global tmp

    # filter parameters
    set file $tmp(file)
    set module $tmp(module)
    set name $tmp(name)

    if {$file=="*"} {set file ""}
    if {$module=="*"} {set module ""}
    if {$name=="*"} {set name ""}

    # get list
    return [opp_getFilteredScalarList $file $module $name]

}

proc applyFilter {} {
    global widget

    busyCursor "Working..."

    # filter parameters
    debug "  getting filtered list"
    set idlist [getFilteredList]

    # clear listbox
    set lb .main.f.main.list
    multicolumnlistbox_deleteall $lb

    # get list
    set num [llength $idlist]

    # ask user if too many...
    set maxcount 100000
    if {$num>$maxcount} {
        tk_messageBox -title "Selection too broad" -icon warning -type ok \
                      -message "Selection too broad: $num matching values, displaying first $maxcount only. Choose a (stricter) filter get rid of this warning."
        set idlist [lreplace $idlist $maxcount end]
    }

    # insert into listbox
    debug "  inserting into table"
    foreach id $idlist {
        multicolumnlistbox_insert $lb $id [opp_getListboxLine $id]
    }
    debug "  done"

    idleCursor

    if {$num<=$maxcount} {
        $widget(status) config -text "$num lines"
    } else {
        $widget(status) config -text "$maxcount lines displayed (out of $num)"
    }

}
