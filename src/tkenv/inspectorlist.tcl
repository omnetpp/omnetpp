#=================================================================
#  INSPECTORLIST.TCL - part of
#
#                     OMNeT++/OMNEST
#            Discrete System Simulation in C++
#
#=================================================================

#----------------------------------------------------------------#
#  Copyright (C) 1992-2008 Andras Varga
#
#  This file is distributed WITHOUT ANY WARRANTY. See the file
#  `license' for details on this and other legal matters.
#----------------------------------------------------------------#

#
# PIL stands for Pending Inspector List.
# Variables:
# - pil_name()
# - pil_class()
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
    global pil_name pil_class pil_type pil_geom

    foreach key [array names pil_name] {
        # check if element is still in the array: if an inspector was several times
        # on the list (ie. both w/ type=0 and type!=0), opening it removes both elements...
        if [info exists pil_name($key)] {
            debug "trying to open inspector $key"
            if [catch {set opened [opp_inspectbyname $pil_name($key) $pil_class($key) $pil_type($key) $pil_geom($key)]}] {
                tk_messageBox -title "Error" -message "Error opening inspector for ($pil_class($key))$pil_name($key), ignoring."
                unset pil_name($key)
                unset pil_class($key)
                unset pil_type($key)
                unset pil_geom($key)
                debug "error opening inspector, discarding entry $key"
            } elseif {$opened} {
                unset pil_name($key)
                unset pil_class($key)
                unset pil_type($key)
                unset pil_geom($key)
                debug "success, removing $key from inspector list"
            } else {
                debug "no such object (yet)"
            }
        }
    }
}

#
# Add an inspector to the list. The underlying object must still exist.
#
proc inspectorList:add {w allowdestructive} {
    global pil_name pil_class pil_type pil_geom pil_nextindex

    set object [opp_inspector_getobject $w]
    set type [opp_inspector_gettype $w]
    if [opp_isnull $object] {
        return
    }

    set objname [opp_getobjectfullpath $object]
    set classname [opp_getobjectshorttypename $object]
    set key "$objname:$classname:$type"

    set pil_name($key)   $objname
    set pil_class($key)  $classname
    set pil_type($key)   $type
    set pil_geom($key)   [inspectorList:getGeometry $w $allowdestructive]

    debug "entry $key added to inspector list"
}

#
# Add all open inspectors to the inspector list.
#
proc inspectorList:addAll {allowdestructive} {
    foreach w [opp_getinspectors 1] {
        inspectorList:add $w $allowdestructive
    }
}

#
# remove an inspector from the 'pending inspectors' list (if it was in the list)
#
# called when an inspector window is opened.
#
proc inspectorList:remove {w} {
    global pil_name pil_class pil_type pil_geom

    set object [opp_inspector_getobject $w]
    set type [opp_inspector_gettype $w]

    set key "[opp_getobjectfullpath $object]:[opp_getobjectshorttypename $object]:$type"

    catch {
        unset pil_name($key)
        unset pil_class($key)
        unset pil_type($key)
        unset pil_geom($key)
        debug "$key removed from inspector list"
    }
}

proc inspectorList:tkenvrcGetContents {allowdestructive} {
    global pil_name pil_class pil_type pil_geom

    set res ""
    foreach w [opp_getinspectors 1] {
       set object [opp_inspector_getobject $w]
       set type [opp_inspector_gettype $w]

       set objname [opp_getobjectfullpath $object]
       set class [opp_getobjectshorttypename $object]
       set geom [inspectorList:getGeometry $w $allowdestructive]

       append res "inspector \"$objname\" \"$class\" \"$type\" \"$geom\"\n"
    }

    foreach key [array names pil_name] {
       append res "inspector \"$pil_name($key)\" \"$pil_class($key)\" \"$pil_type($key)\" \"$pil_geom($key)\"\n"
    }

    debug "generated contents: >>>$res<<<"
    return $res
}


proc inspectorList:tkenvrcReset {} {
    global pil_name pil_class pil_type pil_geom

    # delete old array
    catch {
       unset pil_name
       unset pil_class
       unset pil_type
       unset pil_geom
    }
    debug "inspector list cleared"
}


proc inspectorList:tkenvrcProcessLine {line} {
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

