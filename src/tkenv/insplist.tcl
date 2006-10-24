#=================================================================
#  INSPLIST.TCL - part of
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
# Inspector list handling
#

# PIL stands for Pending Inspector List
#set pil_name() {}
#set pil_class() {}
#set pil_type() {}
#set pil_geom() {}

# must store a copy if object names and class names because this info
# is no longer available when we get called from cObject dtor:
#set insp_w2name() {}
#set insp_w2class() {}


#
# THIS PROC IS CALLED FROM C++ CODE, at each inspector display update.
#
proc inspectorupdate_callback {} {
    global priv

    set priv(animspeed) [opp_getsimoption animation_speed]

    inspectorlist_openinspectors
    notifyPlugins inspectorUpdate
    filteredobjectlist_inspectorupdate
}


#
# try to open inspectors in 'pending inspectors' list
#
proc inspectorlist_openinspectors {} {
    global pil_name pil_class pil_type pil_geom

    foreach key [array names pil_name] {
        # check if element is still in the array: if an inspector was several times
        # on the list (ie. both w/ type=0 and type!=0), opening it removes both elements...
        if [info exists pil_name($key)] {
            #DBG: puts [list opp_inspectbyname $pil_name($key) $pil_class($key) $pil_type($key) $pil_geom($key)]
            if [catch {opp_inspectbyname $pil_name($key) $pil_class($key) $pil_type($key) $pil_geom($key)}] {
                tk_messageBox -title Error -message "Error opening inspector for ($pil_class($key))$pil_name($key), ignoring."
                unset pil_name($key)
                unset pil_class($key)
                unset pil_type($key)
                unset pil_geom($key)
            }
        }
    }
}


proc inspectorlist_storename {w} {
    global insp_w2name insp_w2class

    if {![regexp {\.(ptr.*)-([0-9]+)} $w match object type]} {
        error "window name $w doesn't look like an inspector window"
    }

    set insp_w2name($w) [opp_getobjectfullpath $object]
    set insp_w2class($w) [opp_getobjectclassname $object]
    #debug "object and class name for $w stored"
}


#
# add an inspector to the list
#
# called when an inspector window gets closed because the underlying object
# was destroyed -- in this case remember it on the 'pending inspectors' list
# so that we can reopen the inspector when (if) the object reappears.
#
proc inspectorlist_add {w} {
    global pil_name pil_class pil_type pil_geom pil_nextindex
    global insp_w2name insp_w2class

    if {![regexp {\.(ptr.*)-([0-9]+)} $w match object type]} {
        error "window name $w doesn't look like an inspector window"
    }

    # we cannot use here the opp_getobjectfullpath, opp_getclass methods because
    # we're called from the cObject destructor, name and class are long gone!
    set objname $insp_w2name($w)
    set classname $insp_w2class($w)
    set key "$objname:$classname:$type"

    set pil_name($key)   $objname
    set pil_class($key)  $classname
    set pil_type($key)   $type
    set pil_geom($key)   [winfo geometry $w]  ;# NOTE: "wm geometry" is bogus when window is maximized!
    #debug "$key added to insp list"
}

#
# remove an inspector from the 'pending inspectors' list (if it was in the list)
#
# called when an inspector window is opened.
#
proc inspectorlist_remove {w} {
    global pil_name pil_class pil_type pil_geom

    if {![regexp {\.(ptr.*)-([0-9]+)} $w match object type]} {
        error "window name $w doesn't look like an inspector window"
    }

    set key "[opp_getobjectfullpath $object]:[opp_getobjectclassname $object]:$type"

    catch {
        unset pil_name($key)
        unset pil_class($key)
        unset pil_type($key)
        unset pil_geom($key)
        #debug "$key removed from insp list"
    }
}

proc inspectorlist_tkenvrc_get_contents {} {
    global pil_name pil_class pil_type pil_geom

    set res ""
    foreach win [winfo children .] {
       if [regexp {\.(ptr.*)-([0-9]+)} $win match object type] {
           set objname [opp_getobjectfullpath $object]
           set class [opp_getobjectclassname $object]
           set geom [winfo geometry $win]

           append res "inspector \"$objname\" \"$class\" \"$type\" \"$geom\"\n"
       }
    }

    foreach key [array names pil_name] {
       append res "inspector \"$pil_name($key)\" \"$pil_class($key)\" \"$pil_type($key)\" \"$pil_geom($key)\"\n"
    }

    return $res
}


proc inspectorlist_tkenvrc_reset {} {
    global pil_name pil_class pil_type pil_geom

    # delete old array
    catch {
       unset pil_name
       unset pil_class
       unset pil_type
       unset pil_geom
    }
}


proc inspectorlist_tkenvrc_process_line {line} {
    global pil_name pil_class pil_type pil_geom

    if {[llength $line]!=5} {error "wrong number of columns"}

    set objname [lindex $line 1]
    set class [lindex $line 2]
    set type [lindex $line 3]
    set geom [lindex $line 4]

    if [catch {opp_inspectortype $type}] {return}  ;# ignore obsolete inspector types

    set key "$objname:$class:$type"

    set pil_name($key)   $objname
    set pil_class($key)  $class
    set pil_type($key)   $type
    set pil_geom($key)   $geom
}

