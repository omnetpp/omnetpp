#=================================================================
#  LOADSAVE.TCL - part of
#
#                     OMNeT++/OMNEST
#            Discrete System Simulation in C++
#
#  Parts by: Istvan Pataki, Gabor Vincze ({deity|vinczeg}@sch.bme.hu)
#
#=================================================================

#----------------------------------------------------------------#
#  Copyright (C) 1992-2005 Andras Varga
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

   debug "saving nedfilekey $nedfilekey"

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
    #    debug "tmp_ned($i)=\"$tmp_ned($i)\""
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
    #debug "checkArray says:"
    #checkArray

    # update manager
    updateTreeManager

    # open modules on canvases
    foreach key $modulekeys {
        openModuleOnCanvas $key
    }

    busy
}


# loadNEDrec --
#
# Load NED file and its imports recursiveley
#
proc loadNEDrec {fname} {
    global ned gned config

    set imports {}
    set dir [file dirname $fname]

    set canv_id {}

    while {$fname != ""} {
        set config(default-dir) [file dirname $fname]
        # regsub "^$env(HOME)/" $fname "~/" fname
        loadNED $fname

        #save canvas id of first opened file
        if {$canv_id == ""} {
            set canv_id $gned(canvas_id)
        }

        if {!$config(autoimport)} {
            break
        }

        # find key of last opened file
        foreach fkey [getChildrenWithType 0 nedfile] {
            if {[info exist ned($fkey,filename)] && ($fname == $ned($fkey,filename))} {

                # key found: collect imports of last opened file
                set impskeys [getChildrenWithType $fkey imports]
                foreach impskey $impskeys {
                    foreach impkey [getChildrenWithType $impskey import] {

                        # key of imported file found: add name of import-file to "imports" if not already open
                        set impfilename "$dir/$ned($impkey,name)"
                        if ![string match "*.ned" $impfilename] {
                            append impfilename ".ned"
                        }
                        set isOpen 0
                        foreach fkey [getChildrenWithType 0 nedfile] {
                            if {$impfilename == $ned($fkey,filename)} {
                                set isOpen 1
                            }
                        }
                        if {!$isOpen} {
                            lappend imports $impfilename
                        }
                    }
                }
                break
            }
        }

        # open imported files
        set fname [lindex $imports 0]
        set imports [lreplace $imports 0 0]
    }

    # switch to canvas of first opened file
    if {$canv_id != ""} {
        switchToCanvas $canv_id
    }
}


