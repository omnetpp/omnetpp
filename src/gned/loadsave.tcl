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
#  Copyright (C) 1992-2001 Andras Varga
#  Technical University of Budapest, Dept. of Telecommunications,
#  Stoczek u.2, H-1111 Budapest, Hungary.
#
#  This file is distributed WITHOUT ANY WARRANTY. See the file
#  `license' for details on this and other legal matters.
#----------------------------------------------------------------#


# makeFancyName --
#
# Put together a short filename for display
#
proc makeFancyName {nedfile} {
    if [string compare [pwd] [file dirname $nedfile]] {
       set fancyname "...[file tail [file dirname $nedfile]]/[file tail $nedfile]"
    } else {
       set fancyname [file tail $nedfile]
    }
    return $fancyname
}

# saveNED --
#
#
proc saveNED {nedfilekey} {
   global ned

   puts "DBG: saving nedfilekey $nedfilekey"

   if [catch {
       set nedfile $ned($nedfilekey,filename)
       busy "Saving $nedfile..."
       set fout [open $nedfile w]
       puts $fout [generateNed $nedfilekey]
       close $fout
       busy
   } errmsg] {
       tk_messageBox -icon warning -type ok -message "Error: $errmsg"
       busy
       return
   }
}


# loadNED --
#
#
proc loadNED {nedfile} {
    global ned ned_attr
    global tmp_ned tmp_errors

    busy "Loading $nedfile..."

    # parse NED file and fill the tmp_ned() array
    catch {unset tmp_ned}
    catch {unset tmp_errors}

    set tmp_ned(nextkey) $ned(nextkey)

    set filekey $tmp_ned(nextkey)
    incr tmp_ned(nextkey)

    # add fields
    foreach i [array names ned_attr "common,*"] {
       regsub -- "common," $i "" field
       set tmp_ned($filekey,$field) $ned_attr($i)
    }
    foreach i [array names ned_attr "nedfile,*"] {
       regsub -- "nedfile," $i "" field
       set tmp_ned($filekey,$field) $ned_attr($i)
    }

    set tmp_ned($filekey,type) "nedfile"
    set tmp_ned($filekey,name) [makeFancyName $nedfile]
    set tmp_ned($filekey,filename) $nedfile
    set tmp_ned($filekey,aux-isunnamed) 0
    set tmp_ned($filekey,aux-isdirty) 0

    # parsing...
    if [catch {set num_errs [opp_parsened -file $nedfile \
                               -nedarray tmp_ned \
                               -errorsarray tmp_errors \
                               -nedfilekey $filekey]} errmsg] {
        tk_messageBox -icon error -title "Error" -type ok -message "Error loading $nedfile:\n$errmsg"
        catch {unset tmp_ned}
        catch {unset tmp_errors}
        busy
        return
    }

    # handle parse errors
    if {$num_errs!="0"} {
        # simplified handling: display only the first error
        set errmsg "$tmp_errors(0,type): $tmp_errors(0,text) in line $tmp_errors(0,line)"

        tk_messageBox -icon error -title "Error(s)" -type ok \
            -message "Error(s) loading $nedfile:\n$errmsg"

        catch {unset tmp_ned}
        catch {unset tmp_errors}
        busy
        return
    }

    ## debug code:
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

    # parse display strings
    parse_displaystrings $filekey

    # debug code
    #puts "dbg: checkArray says:"
    #checkArray

    # update manager
    updateTreeManager

    # open modules on canvases
    foreach key $modulekeys {
        openModuleOnCanvas $key
    }

    busy
}



