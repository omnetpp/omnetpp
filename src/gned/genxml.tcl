#=================================================================
#  GENXML.TCL - part of
#
#                     OMNeT++/OMNEST
#            Discrete System Simulation in C++
#
#=================================================================

#----------------------------------------------------------------#
#  Copyright (C) 1992-2005 Andras Varga
#
#  This file is distributed WITHOUT ANY WARRANTY. See the file
#  `license' for details on this and other legal matters.
#----------------------------------------------------------------#


foreach i {label-cid icon-cid rect-cid rect2-cid arrow-cid background-cid
           aux-isdirty aux-isunnamed aux-isselected
           disp-icon disp-iconcolor disp-iconcolorpc
           disp-fillcolor disp-outlinecolor disp-linethickness
           disp-drawmode disp-src-anchor-x disp-src-anchor-y
           disp-dest-anchor-x disp-dest-anchor-y
           disp-xpos disp-ypos disp-xsize disp-ysize disp-layout
           disp-layoutpar1 disp-layoutpar2 disp-layoutpar3 disp-shape} {
   set ned_internal($i) 1
}

proc saveXML {nedfilekey fname} {
   global ned

   if [catch {
       busy "Saving $fname..."
       set fout [open $fname w]
       puts $fout [generateXML $nedfilekey]
       close $fout
       busy
   } errmsg] {
       tk_messageBox -icon warning -type ok -message "Error: $errmsg"
       busy
       return
   }
}


proc generateXML {key} {
    update_displaystrings $key

    set xml ""
    append xml "<?xml version=\"1.0\" ?>\n"
    append xml "<!-- XML file format for NED is currently ***EXPERIMENTAL*** -->\n"

    append xml [generateXMLElement $key ""]
    return $xml
}


proc generateXMLElement {key indent} {
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
            regsub -all "&" $val "%26" val
            regsub -all "<" $val "%3c" val
            regsub -all ">" $val "%3e" val
            append out "$indent  $field=\"$val\"\n"
        }
    }

    # generate children if there are any
    set childkeys $ned($key,childrenkeys)
    if {!$needs_separate_endtag && [llength $childkeys]==0} {
        append out "$indent  />\n"
    } else {
        append out "$indent  >\n"

        set indent2 "$indent    "
        foreach i $childkeys {
            append out [generateXMLElement $i $indent2]
        }
        append out "$indent</$type>\n"
    }
    return $out
}



