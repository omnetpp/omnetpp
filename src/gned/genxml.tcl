#==========================================================================
#  GENXML.TCL -
#            part of the GNED, the Tcl/Tk graphical topology editor of
#                            OMNeT++
#
#   By Andras Varga
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


proc saveXML {nedfilekey fname} {
   global ned

   if [catch {
       busy "Saving $fname..."
       set fout [open $fname w]
       puts $fout "<!--\n     XML file format for NED is currently ***EXPERIMENTAL*** \n-->\n"
       puts $fout "<?xml public=\"ned1.dtd\" ?>"
       puts $fout [generateXML $nedfilekey ""]
       close $fout
       busy
   } errmsg] {
       tk_messageBox -icon warning -type ok -message "Error: $errmsg"
       busy
       return
   }
}


proc generateXML {key indent} {
    global ned ned_attlist

    # generate attributes
    set type $ned($key,type)
    set out ""
    append out "$indent<$type\n"
    foreach field $ned_attlist($type) {
        set val $ned($key,$field)
        regsub -all "\n" $val "\\n" val
        regsub -all "\"" $val "\\&quot;" val
        append out "$indent  $field=\"$val\"\n"
    }

    # generate children if there are any
    set childkeys $ned($key,childrenkeys)
    if {[llength $childkeys]==0} {
        append out "$indent  />\n"
    } else {
        append out "$indent  >\n"

        set indent2 "$indent    "
        foreach i $childkeys {
            append out [generateXML $i $indent2]
        }
        append out "$indent</$type>\n"
    }
    return $out
}

