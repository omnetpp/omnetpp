#==========================================================================
#  SWITCHVI.TCL -
#            part of the GNED, the Tcl/Tk graphical topology editor of
#                            OMNeT++
#   By Andras Varga
#==========================================================================

#----------------------------------------------------------------#
#  Copyright (C) 1992,99 Andras Varga
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

    # remember ned code
    set canvas($canv_id,nedsource) $nedcode

    # initial syntax hightlight
    syntaxHighlight $t 1.0 end
    updateTextStatusbar $t
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

    # check if there were edits in the source
    # (the text widget seems to add a newline, so we'll take this into account)
    set originalnedcode $canvas($canv_id,nedsource)
    if {[string compare $nedcode "$originalnedcode\n"]==0} {
        # no change, so simply switch back to original graphics
        puts "dbg: source unchanged, switching back without parsing..."
        setCanvasMode "graphics"
        return
    }

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

    puts "dbg: deleting old stuff..."

    # delete old stuff from ned() and add new stuff
    set origmodkey $canvas($canv_id,module-key)
    deleteModuleData $origmodkey

    puts "dbg: pasting into ned..."

    # we're not supposed to overwrite the nedfile's properties in ned():
    foreach i [array names tmp_ned "$filekey,*"] {
        unset tmp_ned($i)
    }
    # pasting
    foreach i [array names tmp_ned] {
        set ned($i) $tmp_ned($i)
    }
    # replace $origmodkey with $modkey in nedfile
    set pos [lsearch -exact $ned($filekey,childrenkeys) $origmodkey]
    set ned($filekey,childrenkeys) [lreplace $ned($filekey,childrenkeys) $pos $pos $modkey]

    set ned(nextkey) $tmp_ned(nextkey)
    unset tmp_ned

    # parse display strings
    parse_displaystrings $modkey

    puts "dbg: drawing..."

    # update canvas
    set canvas($canv_id,module-key) $modkey
    $canvas($canv_id,canvas) delete all
    drawItemRecursive $modkey

    # graphics view
    setCanvasMode "graphics"

    # markNedfileOfItemDirty $modkey wouldn't be good: it doesn't
    # always call updateTreeManager
    set ned($filekey,aux-isdirty) 1
    updateTreeManager

    #dbg: this not so good...
    $gned(statusbar).mode config -text "Ready"
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



