#==========================================================================
#  SWITCHVI.TCL -
#            part of the GNED, the Tcl/Tk graphical topology editor of
#                            OMNeT++
#   By Andras Varga
#==========================================================================

#----------------------------------------------------------------#
#  Copyright (C) 1992,98 Andras Varga
#  Technical University of Budapest, Dept. of Telecommunications,
#  Stoczek u.2, H-1111 Budapest, Hungary.
#
#  This file is distributed WITHOUT ANY WARRANTY. See the file
#  `license' for details on this and other legal matters.
#----------------------------------------------------------------#

# switchToNED --
#
# called when the user wants to switch from Graphics view to NED view
#
proc switchToNED {} {
    global gned canvas

    set canv_id $gned(canvas_id)
    if {$canvas($canv_id,mode)=="textedit"} return

    setCanvasMode "textedit"

    # generate ned code
    set key $canvas($canv_id,module-key)
    set nedcode [generateNed $key]

    # insert into textedit
    set t $canvas($canv_id,textedit)
    set curpos [$t index insert]
    $t delete 1.0 end
    $t insert end $nedcode
    catch {$t mark set insert $curpos}
    $t see insert
}


# switchToGraphics --
#
# called when the user wants to switch from NED view to Graphics view
#
proc switchToGraphics {} {
    global gned canvas ned
    global tmp_ned tmp_errors

    set canv_id $gned(canvas_id)
    if {$canvas($canv_id,mode)=="graphics"} return

    # get source from textedit
    set nedcode [$canvas($canv_id,textedit) get 1.0 end]

    #
    # parse it into tmp_ned() array, then merge into ned()
    #

puts "dbg: parsing and checking..."

    # parse NED file and fill the tmp_ned() array
    catch {unset tmp_ned}
    catch {unset tmp_errors}

    set tmp_ned(nextkey) $ned(nextkey)
    set filekey $ned($canvas($canv_id,module-key),parentkey)

    set num_errs [opp_parsened -text $nedcode \
                               -nedarray tmp_ned -errorsarray tmp_errors \
                               -nedfilekey $filekey]

    # handle parse errors
    if {$num_errs!="0"} {
        # simplified handling: display only the first error
        set errmsg "$tmp_errors(0,type): $tmp_errors(0,text) in line $tmp_errors(0,line)"

        if {[okToLoseChanges "Could not backparse NED to graphics:\n\n$errmsg\n"]=="yes"} {
            # switch back
            setCanvasMode "graphics"
        } else {
            # position to error location
            set t $canvas($canv_id,textedit)
            $t mark set insert "$tmp_errors(0,line).$tmp_errors(0,column)"
            $t see insert
        }
        catch {unset tmp_ned}
        catch {unset tmp_errors}
        return
    }

    # debug code:
    #set showkeys [lsort [array names tmp_ned "*"]]
    #foreach i $showkeys {
    #  puts "DBG: tmp_ned($i)=\"$tmp_ned($i)\""
    #}

    # check what was in ned code actually
    set count 0
    set modkey ""
    set extras ""
    foreach i [array names tmp_ned "*,type"] {
        regsub -- ",type" $i "" key
        set type $tmp_ned($i)

        if {$type=="module"} {
            set modkey $key
            set count [expr $count+1]
            set extras "$extras   $type $tmp_ned($key,name)\n"
        } elseif {$type=="import"} {
            set count [expr $count+1]
            set extras "$extras   $type $tmp_ned($key,name)\n"
        } elseif {$type=="network"} {
            set count [expr $count+1]
            set extras "$extras   $type $tmp_ned($key,name)\n"
        } elseif {$type=="channel"} {
            set count [expr $count+1]
            set extras "$extras   $type $tmp_ned($key,name)\n"
        }
    }
    if {$count>=2 || ($modkey=="" && $count>=1)} {
        tk_messageBox -icon info -title "Too much stuff" -type ok \
            -message "NED contains more elements than expected:\n$extras\
                     Remove extra stuff and leave only one module definition there."
        catch {unset tmp_ned}
        return
    }
    if {$modkey==""} {
        if {[okToLoseChanges "NED contains no module definition!\n"]=="yes"} {
           # switch back
           setCanvasMode "graphics"
        }
        catch {unset tmp_ned}
        return
    }

    #
    # OK! In the next lines we'll transfer the new stuff into ned()
    #

    # preserve the position of the orig. module in the ned file:
    set origmodkey $canvas($canv_id,module-key)
    set tmp_ned($modkey,order) $ned($origmodkey,order)

    # we're not supposed to overwrite the nedfile's properties in ned():
    foreach i [array names tmp_ned "$filekey,*"] {
        unset tmp_ned($i)
    }

puts "dbg: deleting old stuff..."

    # delete old stuff from ned() and add new stuff
    deleteModuleData $origmodkey

puts "dbg: pasting into ned..."

    foreach i [array names tmp_ned] {
        set ned($i) $tmp_ned($i)
    }
    insertItem $modkey $filekey

    set ned(nextkey) $tmp_ned(nextkey)
    unset tmp_ned

puts "dbg: drawing..."

    set canvas($canv_id,module-key) $modkey
    $canvas($canv_id,canvas) delete all  # should not be necessary

    # show module on canvas
    drawItemRecursive $modkey

    # graphics view
    setCanvasMode "graphics"

    # maybe the module name has changed
    updateTreeManager

}

# okToLoseChanges --
#
#
proc okToLoseChanges {msg} {
    set ans [tk_messageBox -icon error -title "Error" -type yesno \
              -message "$msg\nDo you want to continue editing?"]
    if {$ans=="yes"} {return "no"}

    set ans [tk_messageBox -icon question -title "Confirm" -type okcancel \
              -message "Lose NED changes and revert to existing graphics?"]
    if {$ans=="cancel"} {return "no"}

    return "yes"
}


# deleteModuleData --
#
# Private proc for switchToGraphics.
#
# $key must be a top-level item. This proc is a simplified version
# of deleteItem. Here we don't care about deleting graphics stuff
# from the canvas (because the whole canvas will be cleared anyway),
# and recursive deletion of data is also simplified (we don't need to
# care about deleting 'related items' (ned(*,*-ownerkey)) because
# such relations only occur within a module.
#
proc deleteModuleData {key} {
   global ned

   # unlink from parent
   set parentkey $ned($key,parentkey)
   set pos [lsearch -exact $ned($parentkey,childrenkeys) $key]
   set ned($parentkey,childrenkeys) [lreplace $ned($parentkey,childrenkeys) $pos $pos]

   deleteModuleDataProc $key
}

# deleteModuleDataProc --
#
# private proc for deleteModuleData
#
proc deleteModuleDataProc {key} {
   global ned

   # delete children recursively
   foreach childkey $ned($key,childrenkeys) {
      deleteModuleDataProc $childkey
   }

   # - deleting non-child linked objects omitted
   # - deleting item from canvas is omitted
   # - potentially closing the canvas is omitted
   # - unlink from parent omitted

   # delete from array
   foreach i [array names ned "$key,*"] {
      unset ned($i)
   }
}


