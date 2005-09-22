#=================================================================
#  MAKENED.TCL - part of
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


# generateNed --
#
# Entry point of NED code generation: return the code as a single big string
#
proc generateNed {key} {
    update_displaystrings $key

    debug "generating NED code..."
    set out [generateNedItem $key {} 0]

    # strip end-line spaces
    regsub -all " +\n" $out "\n" out
    return $out
}

proc generateNedItem {key indent islast} {
    global ned ned_desc

    set type $ned($key,type)

    # if component type not recognized: "undefined proc"
    return [generate_$type $key "$indent$ned_desc($type,plusindent)" $islast]
}

proc generateNedItemWithArg {key indent islast arg} {
    global ned ned_desc

    set type $ned($key,type)

    # if component type not recognized: "undefined proc"
    return [generate_$type $key "$indent$ned_desc($type,plusindent)" $islast $arg]
}

proc generateChildren {key indent} {
    global ned

    # pick children and sort them
    set childkeys $ned($key,childrenkeys)

    # generate them
    set out ""
    set lastkey [lindex $childkeys end]

    foreach i $childkeys {
        if {$i==$lastkey} {
          append out [generateNedItem $i $indent 1]
        } else {
          append out [generateNedItem $i $indent 0]
        }
    }
    return $out
}

proc generateChildrenWithType {key type indent} {
    global ned

    # pick children with the given type and sort them
    set childkeys {}
    foreach i $ned($key,childrenkeys) {
        if {$ned($i,type)==$type} {
            lappend childkeys $i
        }
    }

    # generate them
    set out ""
    set lastkey [lindex $childkeys end]

    foreach i $childkeys {
        if {$i==$lastkey} {
          append out [generateNedItem $i $indent 1]
        } else {
          append out [generateNedItem $i $indent 0]
        }
    }
    return $out
}

proc generateChildrenWithArg {key indent arg} {
    global ned

    # pick children and sort them
    set childkeys $ned($key,childrenkeys)

    # generate them
    set out ""
    set lastkey [lindex $childkeys end]

    foreach i $childkeys {
        if {$i==$lastkey} {
          append out [generateNedItemWithArg $i $indent 1 $arg]
        } else {
          append out [generateNedItemWithArg $i $indent 0 $arg]
        }
    }
    return $out
}

#--------------------------------------

proc generate_nedfile {key indent islast} {
    global ned
    set out ""
    appendBannerComment out $ned($key,banner-comment) $indent
    append out [generateChildren $key $indent]
    return $out
}

proc generate_imports {key indent islast} {
    global ned
    set out ""
    appendBannerComment out $ned($key,banner-comment) $indent
    append out "${indent}import "
    appendRightComment out $ned($key,right-comment)
    append out [generateChildren $key $indent]
    return $out
}

proc generate_import {key indent islast} {
    global ned
    set out ""
    appendBannerComment out $ned($key,banner-comment) $indent
    append out "${indent}\"$ned($key,name)\""
    if {$islast} {append out ";"} else {append out ","}
    appendRightComment out $ned($key,right-comment)
    return $out
}

proc generate_channel {key indent islast} {
    global ned
    set out ""
    appendBannerComment out $ned($key,banner-comment) $indent
    append out "${indent}channel $ned($key,name)"
    appendRightComment out $ned($key,right-comment)
    append out [generateChildren $key $indent]
    append out "${indent}endchannel"
    appendTrailingComment out $ned($key,trailing-comment) ""
    return $out
}

proc generate_chanattr {key indent islast} {
    global ned
    set out ""
    appendBannerComment out $ned($key,banner-comment) $indent
    append out "${indent}$ned($key,name) $ned($key,value);"
    appendRightComment out $ned($key,right-comment)
    return $out
}

proc generate_network {key indent islast} {
    global ned
    set out ""
    appendBannerComment out $ned($key,banner-comment) $indent
    append out "${indent}network $ned($key,name) : $ned($key,type-name)"
    if {$ned($key,like-name)!=""} {
        append out " like $ned($key,like-name)"
    }
    appendRightComment out $ned($key,right-comment)

    append out [generateChildrenWithType $key substmachines $indent]
    append out [generateChildrenWithType $key substparams $indent]

    append out "${indent}endnetwork"
    appendTrailingComment out $ned($key,trailing-comment) ""
    return $out
}

proc generate_simple {key indent islast} {
    global ned
    set out ""
    appendBannerComment out $ned($key,banner-comment) $indent
    append out "${indent}simple $ned($key,name)"
    appendRightComment out $ned($key,right-comment)

    append out [generateChildrenWithType $key machines $indent]
    append out [generateChildrenWithType $key params $indent]
    append out [generateChildrenWithType $key gates $indent]

    append out "${indent}endsimple"
    appendTrailingComment out $ned($key,trailing-comment) ""
    return $out
}

proc generate_module {key indent islast} {
    global ned
    set out ""
    appendBannerComment out $ned($key,banner-comment) $indent
    append out "${indent}module $ned($key,name)"
    appendRightComment out $ned($key,right-comment)

    append out [generateChildrenWithType $key machines $indent]
    append out [generateChildrenWithType $key params $indent]
    append out [generateChildrenWithType $key gates $indent]
    append out [generateChildrenWithType $key submods $indent]
    append out [generateChildrenWithType $key conns $indent]

    set dispstr $ned($key,displaystring)
    if {$dispstr!=""} {
        # HACK: the "display:" line uses indent of "parameters:" line
        global ned_desc
        append out "${indent}$ned_desc(params,plusindent)display: \"$dispstr\";\n"
    }
    append out "${indent}endmodule"
    appendTrailingComment out $ned($key,trailing-comment) ""
    return $out
}

proc generate_params {key indent islast} {
    global ned
    set out ""
    appendBannerComment out $ned($key,banner-comment) $indent
    append out "${indent}parameters:"
    appendRightComment out $ned($key,right-comment)
    append out [generateChildren $key $indent]
    return $out
}

proc generate_param {key indent islast} {
    global ned
    set out ""
    appendBannerComment out $ned($key,banner-comment) $indent
    append out "${indent}$ned($key,name): $ned($key,datatype)"
    if {$islast} {append out ";"} else {append out ","}
    appendRightComment out $ned($key,right-comment)
    return $out
}

proc generate_gates {key indent islast} {
    global ned
    set out ""
    appendBannerComment out $ned($key,banner-comment) $indent
    append out "${indent}gates:"
    appendRightComment out $ned($key,right-comment)
    append out [generateChildren $key $indent]
    return $out
}

proc generate_gate {key indent islast} {
    global ned
    set out ""
    appendBannerComment out $ned($key,banner-comment) $indent
    append out "${indent}$ned($key,gatetype): $ned($key,name)"
    if {$ned($key,isvector)} {append out {[]}}
    append out ";"
    appendRightComment out $ned($key,right-comment)
    return $out
}

proc generate_machines {key indent islast} {
    global ned
    set out ""
    appendBannerComment out $ned($key,banner-comment) $indent
    append out "${indent}machines:"
    appendRightComment out $ned($key,right-comment)
    append out [generateChildren $key $indent]
    return $out
}

proc generate_machine {key indent islast} {
    global ned
    set out ""
    appendBannerComment out $ned($key,banner-comment) $indent
    append out "${indent}$ned($key,name)"
    if {$islast} {append out ";"} else {append out ","}
    appendRightComment out $ned($key,right-comment)
    return $out
}

proc generate_submods {key indent islast} {
    global ned
    set out ""
    appendBannerComment out $ned($key,banner-comment) $indent
    append out "${indent}submodules:"
    appendRightComment out $ned($key,right-comment)
    append out [generateChildren $key $indent]
    return $out
}

proc generate_submod {key indent islast} {
    global ned
    set out ""
    appendBannerComment out $ned($key,banner-comment) $indent
    append out "${indent}$ned($key,name): $ned($key,type-name)"
    if {$ned($key,vectorsize)!=""} {
        append out "\[$ned($key,vectorsize)\]"
    }
    if {$ned($key,like-name)!=""} {
        append out " like $ned($key,like-name)"
    }
    append out ";"
    appendRightComment out $ned($key,right-comment)

    append out [generateChildrenWithType $key substmachines $indent]
    append out [generateChildrenWithType $key substparams $indent]
    append out [generateChildrenWithType $key gatesizes $indent]

    set dispstr $ned($key,displaystring)
    if {$dispstr!=""} {
        # HACK: the "display:" line uses indent of "parameters:" line
        global ned_desc
        append out "${indent}$ned_desc(substparams,plusindent)display: \"$dispstr\";\n"
    }
    return $out
}

proc generate_substparams {key indent islast} {
    global ned
    set out ""
    appendBannerComment out $ned($key,banner-comment) $indent
    if {$ned($key,condition)!=""} {
        append out "${indent}parameters if $ned($key,condition):"
    } else {
        append out "${indent}parameters:"
    }
    appendRightComment out $ned($key,right-comment)
    append out [generateChildren $key $indent]
    return $out
}

proc generate_substparam {key indent islast} {
    global ned
    set out ""
    appendBannerComment out $ned($key,banner-comment) $indent
    append out "${indent}$ned($key,name) = $ned($key,value)"
    if {$islast} {append out ";"} else {append out ","}
    appendRightComment out $ned($key,right-comment)
    return $out
}

proc generate_gatesizes {key indent islast} {
    global ned
    set out ""
    appendBannerComment out $ned($key,banner-comment) $indent
    if {$ned($key,condition)!=""} {
        append out "${indent}gatesizes if $ned($key,condition):"
    } else {
        append out "${indent}gatesizes:"
    }
    appendRightComment out $ned($key,right-comment)
    append out [generateChildren $key $indent]
    return $out
}

proc generate_gatesize {key indent islast} {
    global ned
    set out ""
    appendBannerComment out $ned($key,banner-comment) $indent
    append out "${indent}$ned($key,name)\[$ned($key,size)\]"
    if {$islast} {append out ";"} else {append out ","}
    appendRightComment out $ned($key,right-comment)
    return $out
}

proc generate_substmachines {key indent islast} {
    global ned
    set out ""
    appendBannerComment out $ned($key,banner-comment) $indent
    if {$ned($key,condition)!=""} {
        append out "${indent}on if $ned($key,condition):"
    } else {
        append out "${indent}on:"
    }
    appendRightComment out $ned($key,right-comment)
    append out [generateChildren $key $indent]
    return $out
}

proc generate_substmachine {key indent islast} {
    global ned
    set out ""
    appendBannerComment out $ned($key,banner-comment) $indent
    append out "${indent}$ned($key,value)"
    if {$islast} {append out ";"} else {append out ","}
    appendRightComment out $ned($key,right-comment)
    return $out
}

proc generate_conns {key indent islast} {
    global ned
    set out ""
    appendBannerComment out $ned($key,banner-comment) $indent
    if {$ned($key,nocheck)} {
        append out "${indent}connections nocheck:"
    } else {
        append out "${indent}connections:"
    }
    appendRightComment out $ned($key,right-comment)
    append out [generateChildren $key $indent]
    return $out
}

proc generate_conn {key indent islast} {
    global ned
    set out ""
    appendBannerComment out $ned($key,banner-comment) $indent

    # src and dest module and gate indices
    set src_index ""
    set dest_index ""
    set src_gate_index ""
    set dest_gate_index ""
    if {$ned($key,src-mod-index)!=""}   {set src_index "\[$ned($key,src-mod-index)\]"}
    if {$ned($key,dest-mod-index)!=""}  {set dest_index "\[$ned($key,dest-mod-index)\]"}

    if {$ned($key,src-gate-plusplus)}   {set src_gate_index "++"}   ;# HACK!
    if {$ned($key,dest-gate-plusplus)}  {set dest_gate_index "++"}  ;# HACK!

    if {$ned($key,src-gate-index)!=""}  {set src_gate_index "\[$ned($key,src-gate-index)\]"}
    if {$ned($key,dest-gate-index)!=""} {set dest_gate_index "\[$ned($key,dest-gate-index)\]"}

    # src and dest gates
    if {$ned($ned($key,src-ownerkey),type)=="module"} {
        set src "$ned($key,srcgate)$src_gate_index"
    } else {
        set src "$ned($ned($key,src-ownerkey),name)$src_index.$ned($key,srcgate)$src_gate_index"
    }
    if {$ned($ned($key,dest-ownerkey),type)=="module"} {
        set dest "$ned($key,destgate)$dest_gate_index"
    } else {
        set dest "$ned($ned($key,dest-ownerkey),name)$dest_index.$ned($key,destgate)$dest_gate_index"
    }

    # direction
    if {$ned($key,arrowdir-l2r)} {
        set arrow "-->"
    } else {
        set arrow "<--"

        set tmp $src
        set src $dest
        set dest $tmp
    }

    # now print it
    append out "${indent}$src $arrow"
    append out [generateChildrenWithArg $key $indent $arrow]
    append out " $dest"
    if {$ned($key,condition)!=""} {
       append out " if $ned($key,condition)"
    }
    set dispstr $ned($key,displaystring)
    if {$dispstr!=""} {
        append out " display \"$dispstr\""
    }
    append out ";"

    appendRightComment out $ned($key,right-comment)
    return $out
}

proc generate_connattr {key indent islast {arrow {}}} {
    global ned
    set out ""
    appendBannerComment out $ned($key,banner-comment) $indent
    if {$ned($key,name)=="channel"} {
        # sorry for the special case :-(
        append out " $ned($key,value)"
    } else {
        append out " $ned($key,name) $ned($key,value)"
    }
    if {$islast} {append out " $arrow"}
    appendInlineRightComment out $ned($key,right-comment) ""
    return $out
}

proc generate_forloop {key indent islast} {
    global ned
    set out ""
    appendBannerComment out $ned($key,banner-comment) $indent
    append out "${indent}for "
    append out [generateChildrenWithType $key loopvar $indent]
    append out " do"
    appendRightComment out $ned($key,right-comment)
    append out [generateChildrenWithType $key conn $indent]
    append out "${indent}endfor;"
    appendTrailingComment out $ned($key,trailing-comment) ""
    return $out
}

proc generate_loopvar {key indent islast} {
    global ned
    set out ""
    appendBannerComment out $ned($key,banner-comment) $indent
    append out "$ned($key,name)=$ned($key,fromvalue)..$ned($key,tovalue)"
    if {!$islast} {append out ", "}
    appendInlineRightComment out $ned($key,right-comment) ""
    return $out
}

#-----------------------------------------------------------------

proc appendBannerComment {outvar comment indent {default {}}} {
    upvar $outvar out

    if {$comment==""} return

    regsub -all "\n" $comment "\n$indent//" comment
    set comment "$indent//$comment"
    regsub -all "//-\n" $comment "\n" comment
    regsub "$indent//$" $comment "" comment

    append out $comment
}

proc appendRightComment {outvar comment} {
    upvar $outvar out

    debug "------------"
    debug "orig rightcomment: (($comment))"
    if {$comment==""} {set comment "-\n"}

    regsub ".*\n" $out "" lastline
    regsub -all "." $lastline " " indent
    #debug "lastline: ($lastline)"
    #debug "indent:   ($indent)"

    debug "rightcomment: (($comment))"
    set comment " //$comment"
    regsub -all "\n" $comment "\n$indent //" comment
    regsub -all "//-\n" $comment "\n" comment
    regsub -all "$indent //$" $comment "" comment
    debug "turned into:  (($comment))"

    append out $comment
}

proc appendInlineRightComment {outvar comment indent} {
    upvar $outvar out

    if {$comment=="-\n"} return

    regsub -all "\n" $comment "\n$indent//" comment
    set comment "//$comment"
    regsub -all "//-\n" $comment "\n" comment
    regsub "$indent//$" $comment "$indent" comment

    append out $comment
}

proc appendTrailingComment {outvar comment indent} {
    upvar $outvar out

    if {$comment==""} {set comment "-\n"}

    regsub -all "\n" $comment "\n$indent//" comment
    set comment " //$comment"
    regsub -all "//-\n" $comment "\n" comment
    regsub "$indent//$" $comment "" comment

    append out $comment

}

###################################################################
# raw stuff:
###################################################################

proc _indentLines {prefix str} {
   regsub "\n*$" $str "" str
   if {$str!=""} {
      regsub -all "\n" $str "\n$prefix" str
      set str "$prefix$str\n"
   }
   return $str
}

proc _indentLines2 {prefix str} {
   regsub "\n*$" $str "" str
   if {$str!=""} {
      regsub -all "\n" $str ",\n$prefix" str
      set str "$prefix$str;\n"
   }
   regsub -all ",," $str "," str
   regsub -all ";;" $str ";" str
   return $str
}

proc _indentLines3 {prefix str} {
   regsub "\n*$" $str "" str
   if {$str!=""} {
      regsub -all "\n" $str ";\n$prefix" str
      set str "$prefix$str;\n"
   }
   regsub -all ";;" $str ";" str
   return $str
}

#-----------------------------------------------------------------

# _compareSubmods --  private procedure for generateNedItem
proc _compareSubmods {key1 key2} {
   global ned
   return [string compare $ned($key1,name) $ned($key2,name)]
}

# _compareConnections --  private procedure for generateNedItem
proc _compareConnections {key1 key2} {
   global ned
   if {[info exist ned($key1,for_expression)]} {
      if {[info exist ned($key2,for_expression)]} {
         return [string compare $ned($key1,for_expression) $ned($key2,for_expression)]
      } else {return 1}
   } else {
      if {[info exist ned($key2,for_expression)]} {
         return -1
      } else {
         if [info exist ned($ned($key1,src-ownerkey),is-module)] {
            return -1
         } elseif [info exist ned($ned($key2,src-ownerkey),is-module)] {
            return 1
         } elseif [info exist ned($ned($key1,dest-ownerkey),is-module)] {
            return -1
         } elseif [info exist ned($ned($key2,dest-ownerkey),is-module)] {
            return 1
         } else {
            # Something better should come here
            return [_compareSubmods $ned($key1,src-ownerkey) $ned($key2,src-ownerkey)];
         }

      }
   }
}


