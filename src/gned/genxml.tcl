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


foreach i {label-cid icon-cid rect-cid rect2-cid arrow-cid background-cid
           dirty unnamed selected
           fill-color outline-color icon linethickness drawmode
           an_src_x an_src_y an_dest_x an_dest_y x-pos y-pos x-size y-size} {
   set ned_internal($i) 1
}

proc saveXML {nedfilekey fname} {
   global ned

   if [catch {
       busy "Saving $fname..."
       set fout [open $fname w]
       puts $fout "<!-- XML file format for NED is currently ***EXPERIMENTAL*** -->"
       puts $fout "<?xml version=\"1.0\" ?>"
       puts $fout "<!doctype system=\"ned1.dtd\">"
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
    global ned ned_attlist ned_internal

    # TclXML parser doesn't understand <tag .. /> syntax
    set needs_separate_endtag 1

    # generate attributes
    set type $ned($key,type)
    set out ""
    append out "$indent<$type id=\"$key\"\n"
    foreach field [lsort $ned_attlist($type)] {
        if {![info exist ned_internal($field)]} {
            set val $ned($key,$field)
            regsub -all "\n" $val "%0d%0a" val
            regsub -all "\"" $val "%22" val
            append out "$indent  $field=\"$val\"\n"
        }
    }
    set dispstr ""
    if {$type=="module"} {
        set dispstr [makeModuleDispStr $key]
    } elseif {$type=="submod"} {
        set dispstr [makeSubmoduleDispStr $key]
    } elseif {$type=="conn"} {
        set dispstr [makeConnectionDispStr $key]
    }
    if {$dispstr!=""} {
        append out "$indent  display=\"$dispstr\"\n"
    }

    # generate children if there are any
    set childkeys $ned($key,childrenkeys)
    if {!$needs_separate_endtag && [llength $childkeys]==0} {
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



