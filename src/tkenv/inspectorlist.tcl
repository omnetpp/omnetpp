#=================================================================
#  INSPECTORLIST.TCL - part of
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
# PIL stands for Pending Inspector List.
# Variables:
# - pil_name()
# - pil_class()
# - pil_id()
# - pil_type()
# - pil_geom()
#

#
# THIS PROC IS CALLED FROM C++ CODE, at each inspector display update.
#
proc inspectorUpdateCallback {} {
    global priv

    set priv(animspeed) [opp_getsimoption animation_speed]

    inspectorList:openInspectors
    notifyPlugins inspectorUpdate
    filteredObjectList:inspectorUpdate
}


#
# Try to open inspectors in 'pending inspectors' list
#
proc inspectorList:openInspectors {} {
    global pil_name pil_class pil_id pil_type pil_geom

    foreach key [array names pil_name] {
        if [info exists pil_name($key)] {
            debug "trying to open inspector $key"
            set ptrs [opp_findobjectbyfullpath $pil_name($key) $pil_class($key) $pil_id($key)]
            if {$ptrs!=""} {
                debug "object(s) for inspector list element $key found, removing it from inspector list"
                foreach ptr $ptrs {
                    if [catch {
                        opp_inspect $ptr $pil_type($key) $pil_geom($key)
                    } err] {
                        debug "error opening inspector for $key: $err"
                    }
                }
                unset pil_name($key)
                unset pil_class($key)
                unset pil_id($key)
                unset pil_type($key)
                unset pil_geom($key)
            }
        }
    }
}

#
# Add an inspector to the list. The underlying object must still exist.
#
proc inspectorList:add {insp allowdestructive} {
    global pil_name pil_class pil_id pil_type pil_geom pil_nextindex

    set object [opp_inspector_getobject $insp]
    set type [opp_inspector_gettype $insp]
    if [opp_isnull $object] {
        return
    }

    set objname [opp_getobjectfullpath $object]
    set classname [opp_getobjectshorttypename $object]
    set id [opp_getobjectid $object]

    set key "$objname:$classname:$id:$type"

    set pil_name($key)   $objname
    set pil_class($key)  $classname
    set pil_id($key)     $id
    set pil_type($key)   $type
    set pil_geom($key)   [inspectorList:getGeometry $insp $allowdestructive]

    debug "entry $key added to inspector list"
}

#
# Add all open inspectors to the inspector list.
#
proc inspectorList:addAll {allowdestructive} {
    foreach insp [opp_getinspectors 1] {
        inspectorList:add $insp $allowdestructive
    }
}

#
# remove an inspector from the 'pending inspectors' list (if it was in the list)
#
# called when an inspector window is opened.
#
proc inspectorList:remove {insp} {
    global pil_name pil_class pil_id pil_type pil_geom

    set object [opp_inspector_getobject $insp]
    set type [opp_inspector_gettype $insp]
    set objname [opp_getobjectfullpath $object]
    set classname [opp_getobjectshorttypename $object]
    set id [opp_getobjectid $object]

    set key "$objname:$classname:$id:$type"

    catch {
        unset pil_name($key)
        unset pil_class($key)
        unset pil_id($key)
        unset pil_type($key)
        unset pil_geom($key)
        debug "$key removed from inspector list"
    }
}

proc inspectorList:tkenvrcGetContents {allowdestructive} {
    global pil_name pil_class pil_id pil_type pil_geom

    set res ""
    foreach insp [opp_getinspectors 1] {
       set object [opp_inspector_getobject $insp]
       set type [opp_inspector_gettype $insp]
       if [opp_isnull $object] continue

       set objname [opp_getobjectfullpath $object]
       set class [opp_getobjectshorttypename $object]
       set id [opp_getobjectid $object]
       set geom [inspectorList:getGeometry $insp $allowdestructive]

       append res [list inspector $objname $class $id $type $geom]
       append res "\n"
    }

    foreach key [array names pil_name] {
       append res [list inspector $pil_name($key) $pil_class($key) $pil_id($key) $pil_type($key) $pil_geom($key)]
       append res "\n"
    }

    debug "generated contents: >>>$res<<<"
    return $res
}


proc inspectorList:tkenvrcReset {} {
    global pil_name pil_class pil_id pil_type pil_geom

    # delete old array
    catch {
       unset pil_name
       unset pil_class
       unset pil_id
       unset pil_type
       unset pil_geom
    }
    debug "inspector list cleared"
}


proc inspectorList:tkenvrcProcessLine {line} {
    global pil_name pil_class pil_id pil_type pil_geom

    if {[llength $line]==5} {return} ;# silently ignore inspector lines in older tkenvrc files
    if {[llength $line]!=6} {error "wrong number of columns"}
    setvars {dummy objname class id type geom} $line

    if [catch {opp_inspectortype $type}] {return}  ;# ignore obsolete inspector types

    set key "$objname:$class:$id:$type"

    set pil_name($key)   $objname
    set pil_class($key)  $class
    set pil_id($key)     $id
    set pil_type($key)   $type
    set pil_geom($key)   $geom

    debug "inspector list entry $key read from tkenvrc"
}

#
# Utility function: returns a "geometry:state" string for the given window.
# The geometry corresponds to the "normal" (non-zoomed) state of the
# window, so that can be restored from .tkenvrc as well.
#
proc inspectorList:getGeometry {w allowdestructive} {
    set state [wm state $w]
    if {$state == "normal"} {
        return "[wm geometry $w]:$state"
    } else {
        # Return the "restored" pos and size, not the zoomed or iconized one.
        # For this, we have to temporarily restore the window from the current
        # state (zoomed, iconic, etc.) If Tkenv is exiting or $w is being closed,
        # we don't bother to restore the original state afterwards, as it would
        # only cause more screen flicker.
        wm state $w normal
        set result "[wm geometry $w]:$state"
        if {!$allowdestructive} {
            wm state $w $state ;# restore original state
        }
        return $result
    }
}

