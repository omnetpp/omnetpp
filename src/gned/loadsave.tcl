#==========================================================================
#  LOADSAVE.TCL -
#            part of the GNED, the Tcl/Tk graphical topology editor of
#                            OMNeT++
#
#   By Andras Varga
#   Parts: Istvan Pataki, Gabor Vincze ({deity|vinczeg}@sch.bme.hu)
#
#==========================================================================

#----------------------------------------------------------------#
#  Copyright (C) 1992,99 Andras Varga
#  Technical University of Budapest, Dept. of Telecommunications,
#  Stoczek u.2, H-1111 Budapest, Hungary.
#
#  This file is distributed WITHOUT ANY WARRANTY. See the file
#  `license' for details on this and other legal matters.
#----------------------------------------------------------------#


# saveNED --
#
#
proc saveNED {nedfilekey} {
   global ned

puts "DBG: saving nedfilekey $nedfilekey"

   if [catch {
       set nedfile $ned($nedfilekey,filename)
       set fout [open $nedfile w]
       puts $fout [generateNed $nedfilekey]
       close $fout
   } errmsg] {
       tk_messageBox -icon warning -type ok -message "Error: $errmsg"
       return
   }
}


# loadNED --
#
#
proc loadNED {nedfile} {
    global ned
    global tmp_ned tmp_errors

    # parse NED file and fill the tmp_ned() array
    catch {unset tmp_ned}
    catch {unset tmp_errors}

    set tmp_ned(nextkey) $ned(nextkey)

    set filekey $tmp_ned(nextkey)
    incr tmp_ned(nextkey)

    set tmp_ned($filekey,type) "nedfile"
    set tmp_ned($filekey,name) $nedfile
    set tmp_ned($filekey,filename) $nedfile

    set num_errs [opp_parsened -file $nedfile \
                               -nedarray tmp_ned -errorsarray tmp_errors \
                               -nedfilekey $filekey]

    # handle parse errors
    if {$num_errs!="0"} {
        # simplified handling: display only the first error
        set errmsg "$tmp_errors(0,type): $tmp_errors(0,text) in line $tmp_errors(0,line)"

        tk_messageBox -icon error -title "Error(s)" -type ok \
            -message "Error(s) loading $nedfile:\n$errmsg"

        catch {unset tmp_ned}
        catch {unset tmp_errors}
        return
    }

    # debug code:
    #set showkeys [lsort [array names tmp_ned "*"]]
    #foreach i $showkeys {
    #    puts "DBG: tmp_ned($i)=\"$tmp_ned($i)\""
    #}

    # collect modules from code for further display
    set modulekeys ""
    foreach key $tmp_ned($filekey,childrenkeys) {
        if {$tmp_ned($key,type)=="module"} {
            lappend modulekeys $key
        }
    }

    # add tmp_ned() contents to ned()
    foreach i [array names tmp_ned] {
        set ned($i) $tmp_ned($i)
    }
    set ned(nextkey) $tmp_ned(nextkey)
    unset tmp_ned

    # insert under "root" (item 0)
    insertItem $filekey 0

    # debug code
    #puts "dbg: checkArray says:"
    #checkArray

    # update manager
    updateTreeManager

    # open modules on canvases
    foreach key $modulekeys {
        openModuleOnNewCanvas $key
    }
}


# _compareDefinitions --  private procedure for generateNed
proc _compareDefinitions {key1 key2} {
   global ned
   if {$ned($key1,type)!="import"} {
      return -1
   } elseif {$ned($key2,type)!="import"} {
      return 1
   } elseif {$ned($key1,order)!="" && $ned($key2,order)!=""} {
     if {$ned($key1,order) < $ned($key2,order)} {
        return -1
     } elseif {$ned($key1,order) > $ned($key2,order)} {
        return 1
     }
   }

   if {$ned($key1,type)!="channel"} {
      return -1
   } elseif {$ned($key2,type)!="channel"} {
      return 1;
   } elseif {$ned($key1,type)!="network"} {
      return 1;
   } elseif {$ned($key2,type)!="network"} {
      return -1;
   } else {
     # both are module
     return -1;
   }

   return [string compare $ned($key1,name) $ned($key2,name)]
}



