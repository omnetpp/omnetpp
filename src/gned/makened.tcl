#==========================================================================
#  MAKENED.TCL -
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


# generateNed --
#
# Entry point of NED code generation: return the code as a single big string
#
proc generateNed {key} {
    return [generateNedItem $key {} 0]
}

proc generateNedItem {key indent islast} {
    global ned ddesc

    set type $ned($key,type)

    # if component type not recognized: "undefined proc"
    return [generate_$type $key "$indent$ddesc($type,plusindent)" $islast]
}

proc generateNedItemWithArg {key indent islast arg} {
    global ned ddesc

    set type $ned($key,type)

    # if component type not recognized: "undefined proc"
    return [generate_$type $key "$indent$ddesc($type,plusindent)" $islast $arg]
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
          set out "$out[generateNedItem $i $indent 1]"
        } else {
          set out "$out[generateNedItem $i $indent 0]"
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
          set out "$out[generateNedItem $i $indent 1]"
        } else {
          set out "$out[generateNedItem $i $indent 0]"
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
          set out "$out[generateNedItemWithArg $i $indent 1 $arg]"
        } else {
          set out "$out[generateNedItemWithArg $i $indent 0 $arg]"
        }
    }
    return $out
}

#--------------------------------------

proc generate_nedfile {key indent islast} {
    global ned
    set out ""
    appendBannerComment out $ned($key,banner-comment) $indent
    appendTo out [generateChildren $key $indent] nonewline
    return $out
}

proc generate_imports {key indent islast} {
    global ned
    set out ""
    appendBannerComment out $ned($key,banner-comment) $indent
    appendTo out "${indent}import " nonewline
    appendRightComment out $ned($key,right-comment)
    appendTo out [generateChildren $key $indent] nonewline
    appendTrailingComment out $ned($key,trailing-comment) ""
    return $out
}

proc generate_import {key indent islast} {
    global ned
    set out ""
    appendBannerComment out $ned($key,banner-comment) $indent
    appendTo out "${indent}$ned($key,name)" nonewline
    if {$islast} {appendTo out ";" nonewline} else {appendTo out "," nonewline}
    appendRightComment out $ned($key,right-comment)
    return $out
}

proc generate_channel {key indent islast} {
    global ned
    set out ""
    appendBannerComment out $ned($key,banner-comment) $indent
    appendTo out "${indent}channel $ned($key,name)" nonewline
    appendRightComment out $ned($key,right-comment)
    appendTo out [generateChildren $key $indent] nonewline
    appendTo out "${indent}endchannel" nonewline
    appendTrailingComment out $ned($key,trailing-comment) ""
    return $out
}

proc generate_chanattr {key indent islast} {
    global ned
    set out ""
    appendBannerComment out $ned($key,banner-comment) $indent
    appendTo out "${indent}$ned($key,name) $ned($key,value);" nonewline
    appendRightComment out $ned($key,right-comment)
    return $out
}

proc generate_network {key indent islast} {
    global ned
    set out ""
    appendBannerComment out $ned($key,banner-comment) $indent
    appendTo out "${indent}network $ned($key,name) : $ned($key,type-name)" nonewline
    appendRightComment out $ned($key,right-comment)

    appendTo out [generateChildrenWithType $key substmachines $indent] nonewline
    appendTo out [generateChildrenWithType $key substparams $indent] nonewline

    appendTo out "${indent}endnetwork" nonewline
    appendTrailingComment out $ned($key,trailing-comment) ""
    return $out
}

proc generate_simple {key indent islast} {
    global ned
    set out ""
    appendBannerComment out $ned($key,banner-comment) $indent
    appendTo out "${indent}simple $ned($key,name)" nonewline
    appendRightComment out $ned($key,right-comment)

    appendTo out [generateChildrenWithType $key machines $indent] nonewline
    appendTo out [generateChildrenWithType $key params $indent] nonewline
    appendTo out [generateChildrenWithType $key gates $indent] nonewline

    appendTo out "${indent}endsimple" nonewline
    appendTrailingComment out $ned($key,trailing-comment) ""
    return $out
}

proc generate_module {key indent islast} {
    global ned
    set out ""
    appendBannerComment out $ned($key,banner-comment) $indent
    appendTo out "${indent}module $ned($key,name)" nonewline
    appendRightComment out $ned($key,right-comment)

    appendTo out [generateChildrenWithType $key machines $indent] nonewline
    appendTo out [generateChildrenWithType $key params $indent] nonewline
    appendTo out [generateChildrenWithType $key gates $indent] nonewline
    appendTo out [generateChildrenWithType $key submods $indent] nonewline
    appendTo out [generateChildrenWithType $key conns $indent] nonewline

    set dispstr [makeModuleDispStr $key]
    if {$dispstr!=""} {
        # HACK: the "display:" line uses indent of "parameters:" line
        global ddesc
        appendTo out "${indent}$ddesc(params,plusindent)display: \"$dispstr\";"
    }
    appendTo out "${indent}endmodule" nonewline
    appendTrailingComment out $ned($key,trailing-comment) ""
    return $out
}

proc generate_params {key indent islast} {
    global ned
    set out ""
    appendBannerComment out $ned($key,banner-comment) $indent
    appendTo out "${indent}parameters:" nonewline
    appendRightComment out $ned($key,right-comment)
    appendTo out [generateChildren $key $indent] nonewline
    return $out
}

proc generate_param {key indent islast} {
    global ned
    set out ""
    appendBannerComment out $ned($key,banner-comment) $indent
    appendTo out "${indent}$ned($key,name) : $ned($key,datatype)" nonewline
    if {$islast} {appendTo out ";" nonewline} else {appendTo out "," nonewline}
    appendRightComment out $ned($key,right-comment)
    return $out
}

proc generate_gates {key indent islast} {
    global ned
    set out ""
    appendBannerComment out $ned($key,banner-comment) $indent
    appendTo out "${indent}gates:" nonewline
    appendRightComment out $ned($key,right-comment)
    appendTo out [generateChildren $key $indent] nonewline
    return $out
}

proc generate_gate {key indent islast} {
    global ned
    set out ""
    appendBannerComment out $ned($key,banner-comment) $indent
    appendTo out "${indent}$ned($key,gatetype): $ned($key,name)" nonewline
    if {$ned($key,isvector)} {appendTo out "\[\]" nonewline}
    appendTo out ";" nonewline
    appendRightComment out $ned($key,right-comment)
    return $out
}

proc generate_machines {key indent islast} {
    global ned
    set out ""
    appendBannerComment out $ned($key,banner-comment) $indent
    appendTo out "${indent}machines:" nonewline
    appendRightComment out $ned($key,right-comment)
    appendTo out [generateChildren $key $indent] nonewline
    return $out
}

proc generate_machine {key indent islast} {
    global ned
    set out ""
    appendBannerComment out $ned($key,banner-comment) $indent
    appendTo out "${indent}$ned($key,name)" nonewline
    if {$islast} {appendTo out ";" nonewline} else {appendTo out "," nonewline}
    appendRightComment out $ned($key,right-comment)
    return $out
}

proc generate_submods {key indent islast} {
    global ned
    set out ""
    appendBannerComment out $ned($key,banner-comment) $indent
    appendTo out "${indent}submodules:" nonewline
    appendRightComment out $ned($key,right-comment)
    appendTo out [generateChildren $key $indent] nonewline
    return $out
}

proc generate_submod {key indent islast} {
    global ned
    set out ""
    appendBannerComment out $ned($key,banner-comment) $indent
    appendTo out "${indent}$ned($key,name): $ned($key,type-name)" nonewline
    if {$ned($key,vectorsize)!=""} {
        appendTo out "\[$ned($key,vectorsize)\]" nonewline
    }
    if {$ned($key,like-name)!=""} {
        appendTo out " like $ned($key,like-name)" nonewline
    }
    appendTo out ";" nonewline
    appendRightComment out $ned($key,right-comment)

    appendTo out [generateChildrenWithType $key substmachines $indent] nonewline
    appendTo out [generateChildrenWithType $key substparams $indent] nonewline
    appendTo out [generateChildrenWithType $key gatesizes $indent] nonewline

    set dispstr [makeSubmoduleDispStr $key]
    if {$dispstr!=""} {
        # HACK: the "display:" line uses indent of "parameters:" line
        global ddesc
        appendTo out "${indent}$ddesc(substparams,plusindent)display: \"$dispstr\";"
    }
    return $out
}

proc generate_substparams {key indent islast} {
    global ned
    set out ""
    appendBannerComment out $ned($key,banner-comment) $indent
    if {$ned($key,condition)!=""} {
        appendTo out "${indent}parameters if $ned($key,condition):" nonewline
    } else {
        appendTo out "${indent}parameters:" nonewline
    }
    appendRightComment out $ned($key,right-comment)
    appendTo out [generateChildren $key $indent] nonewline
    return $out
}

proc generate_substparam {key indent islast} {
    global ned
    set out ""
    appendBannerComment out $ned($key,banner-comment) $indent
    appendTo out "${indent}$ned($key,name) = $ned($key,value)" nonewline
    if {$islast} {appendTo out ";" nonewline} else {appendTo out "," nonewline}
    appendRightComment out $ned($key,right-comment)
    return $out
}

proc generate_gatesizes {key indent islast} {
    global ned
    set out ""
    appendBannerComment out $ned($key,banner-comment) $indent
    if {$ned($key,condition)!=""} {
        appendTo out "${indent}gatesizes if $ned($key,condition):" nonewline
    } else {
        appendTo out "${indent}gatesizes:" nonewline
    }
    appendRightComment out $ned($key,right-comment)
    appendTo out [generateChildren $key $indent] nonewline
    return $out
}

proc generate_gatesize {key indent islast} {
    global ned
    set out ""
    appendBannerComment out $ned($key,banner-comment) $indent
    appendTo out "${indent}$ned($key,name)\[$ned($key,size)\]" nonewline
    if {$islast} {appendTo out ";" nonewline} else {appendTo out "," nonewline}
    appendRightComment out $ned($key,right-comment)
    return $out
}

proc generate_substmachines {key indent islast} {
    global ned
    set out ""
    appendBannerComment out $ned($key,banner-comment) $indent
    if {$ned($key,condition)!=""} {
        appendTo out "${indent}on if $ned($key,condition):" nonewline
    } else {
        appendTo out "${indent}on:" nonewline
    }
    appendRightComment out $ned($key,right-comment)
    appendTo out [generateChildren $key $indent] nonewline
    return $out
}

proc generate_substmachine {key indent islast} {
    global ned
    set out ""
    appendBannerComment out $ned($key,banner-comment) $indent
    appendTo out "${indent}$ned($key,value)" nonewline
    if {$islast} {appendTo out ";" nonewline} else {appendTo out "," nonewline}
    appendRightComment out $ned($key,right-comment)
    return $out
}

proc generate_conns {key indent islast} {
    global ned
    set out ""
    appendBannerComment out $ned($key,banner-comment) $indent
    if {$ned($key,nocheck)} {
        appendTo out "${indent}connections nocheck:" nonewline
    } else {
        appendTo out "${indent}connections:" nonewline
    }
    appendRightComment out $ned($key,right-comment)
    appendTo out [generateChildren $key $indent] nonewline
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
    if {$ned($key,src_index)!=""}       {set src_index "\[$ned($key,src_index)\]"}
    if {$ned($key,dest_index)!=""}      {set dest_index "\[$ned($key,dest_index)\]"}
    if {$ned($key,src_gate_index)!=""}  {set src_gate_index "\[$ned($key,src_gate_index)\]"}
    if {$ned($key,dest_gate_index)!=""} {set dest_gate_index "\[$ned($key,dest_gate_index)\]"}

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
    appendTo out "${indent}$src $arrow" nonewline
    appendTo out [generateChildrenWithArg $key $indent $arrow] nonewline
    appendTo out " $dest" nonewline
    if {$ned($key,condition)!=""} {
       appendTo out " if $ned($key,condition)" nonewline
    }
    set dispstr [makeConnectionDispStr $key]
    if {$dispstr!=""} {
        appendTo out " display \"$dispstr\"" nonewline
    }
    appendTo out ";" nonewline

    appendRightComment out $ned($key,right-comment)
    return $out
}

proc generate_connattr {key indent islast {arrow {}}} {
    global ned
    set out ""
    appendBannerComment out $ned($key,banner-comment) $indent
    if {$ned($key,name)=="channel"} {
        # sorry for the special case :-(
        appendTo out " $ned($key,value)" nonewline
    } else {
        appendTo out " $ned($key,name) $ned($key,value)" nonewline
    }
    if {$islast} {appendTo out " $arrow" nonewline}
    appendInlineRightComment out $ned($key,right-comment) ""
    return $out
}

proc generate_forloop {key indent islast} {
    global ned
    set out ""
    appendBannerComment out $ned($key,banner-comment) $indent
    appendTo out "${indent}for " nonewline
    appendTo out [generateChildrenWithType $key loopvar $indent] nonewline
    appendTo out " do" nonewline
    appendRightComment out $ned($key,right-comment)
    appendTo out [generateChildrenWithType $key conn $indent] nonewline
    appendTo out "${indent}endfor;" nonewline
    appendTrailingComment out $ned($key,trailing-comment) ""
    return $out
}

proc generate_loopvar {key indent islast} {
    global ned
    set out ""
    appendBannerComment out $ned($key,banner-comment) $indent
    appendTo out "$ned($key,name)=$ned($key,fromvalue)..$ned($key,tovalue)" nonewline
    if {!$islast} {appendTo out ", " nonewline}
    appendInlineRightComment out $ned($key,right-comment) ""
    return $out
}

#---------------------------------------------------

proc makeModuleDispStr {key} {
    global ned

    set pos [format "p=%s,%s;" $ned($key,x-pos) $ned($key,y-pos)]
    set siz [format "b=%s,%s,rect;" $ned($key,x-size) $ned($key,y-size)]
    set opts [format "o=%s,%s,%s;" $ned($key,fill-color) $ned($key,outline-color) $ned($key,linethickness)]
    if {$pos=="p=,;"}      {set pos ""}
    if {$siz=="b=,,rect;"} {set siz ""}
    if {$opts=="o=,,;"}    {set opts ""}

    set dispstr "$pos$siz$opts"
    regsub -- ";$" $dispstr "" dispstr

    return $dispstr
}

proc makeSubmoduleDispStr {key} {
    global ned

    if {$ned($key,icon)!=""} {
       set pos [format "p=%s,%s;" $ned($key,x-pos) $ned($key,y-pos)]
       set icon [format "i=%s;" $ned($key,icon)]
       if {$pos=="p=,;"} {set pos ""}
       set dispstr "$pos$icon"
    } else {
       set pos [format "p=%s,%s;" $ned($key,x-pos) $ned($key,y-pos)]
       set siz [format "b=%s,%s,rect;" $ned($key,x-size) $ned($key,y-size)]
       set opts [format "o=%s,%s,%s;" $ned($key,fill-color) $ned($key,outline-color) $ned($key,linethickness)]
       if {$pos=="p=,;"}      {set pos ""}
       if {$siz=="b=,,rect;"} {set siz ""}
       if {$opts=="o=,,;"}    {set opts ""}
       set dispstr "$pos$siz$opts"
    }
    regsub -- ";$" $dispstr "" dispstr
    return $dispstr
}

proc makeConnectionDispStr {key} {
    global ned

    if {$ned($key,drawmode)=="a" || $ned($key,drawmode)==""} {
        set mode ""
    } elseif {$ned($key,drawmode)=="m"} {
        set mode [format "m=m,%s,%s,%s,%s;" \
                           $ned($key,an_src_x)  $ned($key,an_src_y) \
                           $ned($key,an_dest_x) $ned($key,an_dest_y)]
       if {$mode=="m=m,,,,;"}  {set mode ""}
    } else {
        set mode "m=$ned($key,drawmode);"
    }

    set opts [format "o=%s,%s;" $ned($key,fill-color) $ned($key,linethickness)]
    if {$opts=="o=,;"}  {set opts ""}

    set dispstr "$mode$opts"
    regsub -- ";$" $dispstr "" dispstr
    return $dispstr
}

#-----------------------------------------------------------------

proc appendTo {varname str {nl {}}} {
    upvar $varname var
    if {$nl=="nonewline"} {
        set var "$var$str"
    } else {
        set var "$var$str\n"
    }
}

proc appendBannerComment {outvar comment indent {default {}}} {
    upvar $outvar out

    if {$comment==""} return

    regsub -all "\n" $comment "\n$indent//" comment
    set comment "$indent//$comment"
    regsub -all "//-\n" $comment "\n" comment
    regsub "$indent//$" $comment "" comment

    appendTo out $comment nonewline
}

proc appendRightComment {outvar comment} {
    upvar $outvar out

    if {$comment==""} {set comment "-\n"}

    regsub ".*\n" $out "" lastline
    regsub -all "." $lastline " " indent
    #puts "dbg: lastline: ($lastline)"
    #puts "dbg: indent:   ($indent)"

    #puts "dbg: rightcomment: (($comment))"
    set comment " //$comment"
    regsub -all "\n" $comment "\n$indent //" comment
    regsub -all "//-\n" $comment "\n" comment
    regsub -all "$indent //$" $comment "" comment
    #puts "dbg: turned into:  (($comment))"

    appendTo out $comment nonewline
}

proc appendInlineRightComment {outvar comment indent} {
    upvar $outvar out

    if {$comment=="-\n"} return

    regsub -all "\n" $comment "\n$indent//" comment
    set comment "//$comment"
    regsub -all "//-\n" $comment "\n" comment
    regsub "$indent//$" $comment "$indent" comment

    appendTo out $comment nonewline
}

proc appendTrailingComment {outvar comment indent} {
    upvar $outvar out

    if {$comment==""} {set comment "-\n"}

    regsub -all "\n" $comment "\n$indent//" comment
    set comment " //$comment"
    regsub -all "//-\n" $comment "\n" comment
    regsub "$indent//$" $comment "" comment

    appendTo out $comment nonewline

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


