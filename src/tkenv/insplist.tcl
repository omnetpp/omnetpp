#==========================================================================
#  INSPLIST.TCL -
#            part of the Tcl/Tk windowing environment of
#                            OMNeT++
#==========================================================================

#----------------------------------------------------------------#
#  Copyright (C) 1992-2001 Andras Varga
#  Technical University of Budapest, Dept. of Telecommunications,
#  Stoczek u.2, H-1111 Budapest, Hungary.
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
    # experimental code (for Token sample prog):
    # opp_inspectbyname {token.comp[0].sink.local-objects.queuing-time} {cOutVector} {As Graphics}
    inspectorlist_openinspectors
}


#
# try to open inspectors in 'pending inspectors' list
#
proc inspectorlist_openinspectors {} {
    global pil_name pil_class pil_type pil_geom

    foreach key [array names pil_name] {
        #puts [list opp_inspectbyname $pil_name($key) $pil_class($key) $pil_type($key) $pil_geom($key)]
        opp_inspectbyname $pil_name($key) $pil_class($key) $pil_type($key) $pil_geom($key)
    }
}


proc inspectorlist_storename {w} {
    global insp_w2name insp_w2class

    if {![regexp {\.(ptr.*)-([0-9]+)} $w match object type]} {
        error "window name $w doesn't look like an inspector window"
    }

    set insp_w2name($w) [opp_getobjectfullpath $object]
    set insp_w2class($w) [opp_getobjectclassname $object]
    #puts "DBG: object and class name for $w stored"
}


#
# add an inspector to the list
#
# called when an inspector window closed because the undelying object
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
    set pil_geom($key)   [wm geometry $w]
    #puts "DBG: $key added to insp list"
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
        #puts "DBG: $key removed from insp list"
    }
}

#
# save inspector list to given stream
#
proc inspectorlist_save {f} {
    global pil_name pil_class pil_type pil_geom

    foreach win [winfo children .] {
       if [regexp {\.(ptr.*)-([0-9]+)} $win match object type] {
           set objname [opp_getobjectfullpath $object]
           set class [opp_getobjectclassname $object]
           set geom [wm geometry $win]

           puts $f "\"$objname\" \"$class\" \"$type\" \"$geom\""
       }
    }

    foreach key [array names pil_name] {
       puts $f "\"$pil_name($key)\" \"$pil_class($key)\" \"$pil_type($key)\" \"$pil_geom($key)\""
    }
}

#
# load inspector list from a given stream; contents add to current inspector list
#
proc inspectorlist_load {f} {
    global pil_name pil_class pil_type pil_geom

    # read file line by line
    set lineno 1
    while {[gets $f line] >= 0} {
      if {$line == ""} {incr lineno; continue}
      if [string match {#*} $line] {incr lineno; continue}
      if [catch {
          set objname [lindex $line 0];
          set class [lindex $line 1];
          set type [lindex $line 2]
          set geom [lindex $line 3]
      }] {
          messagebox {Open Inspectors in File} "`$filename' line $lineno is invalid." info ok
          incr lineno; continue
      }

      set key "$objname:$class:$type"

      set pil_name($key)   $objname
      set pil_class($key)  $class
      set pil_type($key)   $type
      set pil_geom($key)   $geom


      incr lineno
    }

    inspectorlist_openinspectors
}

