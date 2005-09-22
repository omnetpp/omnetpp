#=================================================================
#  DATA.TCL - part of
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


#------------------------------------------------
# Data structure is defined in datadict.tcl
# Initialization of global vars (ned(0,*) and ned(nextkey)) is also
# at the end of datadict.tcl
#------------------------------------------------


# addItem --
#
# Create an item with the given type and add it to ned().
# Doesn't care about the 'name' attribute!
#
proc addItem {type parentkey} {
    global ned ned_attr ned_attlist

    # choose key
    set key $ned(nextkey)
    incr ned(nextkey)

    # add ned() attributes: (for 'common' and for $type)
    foreach attr $ned_attlist(common) {
       set ned($key,$attr) $ned_attr(common,$attr)
    }
    foreach attr $ned_attlist($type) {
       set ned($key,$attr) $ned_attr($type,$attr)
    }
    set ned($key,type) $type

    # set parent  (everyone has one except root)
    set ned($key,parentkey) $parentkey
    if {$type!="root"} {
        lappend ned($parentkey,childrenkeys) $key
    }

    # mark it "not selected" on canvas
    set ned($key,aux-isselected) 0

    #debug "addItem: $type $key added to $parentkey (its children now: $ned($parentkey,childrenkeys))"

    return $key
}


# addItemWithUniqueName --
#
# create item with unique name among its siblings
#
proc addItemWithUniqueName {type parentkey} {
    global ned
    set key [addItem $type $parentkey]

    # make name unique (if it has one)
    if [info exist ned($key,name)] {
        set ned($key,name) [makeUniqueName $key $ned($key,name)]
    }
    return $key
}


# insertItem --
#
# make item the child of another item
#
proc insertItem {key parentkey} {
    global ned

    set ned($key,parentkey) $parentkey
    lappend ned($parentkey,childrenkeys) $key
}


# deleteItem --
#
# delete an item; also delete it from canvas, with linked items
#
proc deleteItem {key} {
    global ned canvas ned_attlist

    #debug "deleteItem $key entered"

    # prevent race conditions...
    if {[info exist ned($key,being-deleted)]} {
       # we were here already
       return
    }
    set ned($key,being-deleted) 1

    # delete children recursively
    foreach childkey $ned($key,childrenkeys) {
       deleteItem $childkey
    }

    set type $ned($key,type)

    # delete non-child linked objects
    #   (e.g. connections when a submod is deleted)
    debug "deleteItem: inefficient code ('foreach array names')"
    foreach i [array names ned "*,*ownerkey"] {
       if {[info exist ned($i)] && $ned($i)==$key} {
          regsub -- ",.*ownerkey" $i "" childkey
          deleteItem $childkey
       }
    }

    # delete item from canvas (if it's there)
    # if a canvas displayed exactly this item, close that canvas
    set canv_id [canvasIdFromItemKey $key]
    if {$canv_id!=""} {
       if {$canvas($canv_id,module-key)==$key} {
           destroyCanvas $canv_id
       } else {
           set c $canvas($canv_id,canvas)
           # note: we assume here that 'common' attributes don't contain any 'cid'
           foreach attr $ned_attlist($type) {
               if [string match "*-cid" $attr] {
                  $c delete $ned($key,$attr)
               }
           }
       }
    }

    # unlink from parent
    set parentkey $ned($key,parentkey)
    set pos [lsearch -exact $ned($parentkey,childrenkeys) $key]
    set ned($parentkey,childrenkeys) [lreplace $ned($parentkey,childrenkeys) $pos $pos]

    # delete from array
    foreach field $ned_attlist($type) {
        unset ned($key,$field)
    }
    foreach field $ned_attlist(common) {
        catch {unset ned($key,$field)}
    }

    # delete empty for-loop, conns, etc
    if {![info exist ned($parentkey,being-deleted)]} {
        if {$ned($parentkey,childrenkeys)==""} {
            if {[lsearch {gates params submods substmachines substparams gatesizes conns} $ned($parentkey,type)]!=-1} {
                deleteItem $parentkey
            }
        } elseif {$ned($parentkey,type)=="forloop"} {
            set hasconn 0
            foreach tmpkey $ned($parentkey,childrenkeys) {
                if {$ned($tmpkey,type)=="conn"} {set hasconn 1}
            }
            if {$hasconn==0} {
                deleteItem $parentkey
            }
        }
    }
}


# isNameLegal --
#
# Check that name string doesn't contain invalid chars and is not
# reserved word. Returns 0 or 1.
#
proc isNameLegal {name} {
    # check for invalid chars
    if ![regexp -- {^[a-zA-Z_][a-zA-Z0-9_]*$} $name] {
        # contains invalid chars
        return 0
    }

    # check if it's a reserved word
    set keywords {include import network module simple channel delay error datarate
                  for do true false ref ancestor input const sizeof endsimple endmodule
                  endchannel endnetwork endfor parameters gatesizes gates
                  submodules connections display on like machines to if index nocheck
                  numeric string bool xml xmldoc anytype cplusplus class message struct enum
                  fields properties extends int long char unsigned}
    if {[lsearch $keywords $name]!=-1} {
        # reserved word
        return 0
    }
    return 1
}


# isNameLegalAndUnique --
#
# Check that name string doesn't contain invalid chars and it is locally unique
# Returns 0 or 1.
#
proc isNameLegalAndUnique {key name} {
    global ned

    if ![info exist ned($key,name)] {
        error "item $key has no name attribute"
    }

    # check for invalid chars and reserved words
    if ![isNameLegal $name] {
        return 0
    }

    # check if name is unique among all siblings
    foreach siblingkey [getChildren $ned($key,parentkey)] {
        if {$siblingkey!=$key && [info exist ned($siblingkey,name)]} {
            if {[string compare $ned($siblingkey,name) $name]==0} {
                # name not unique
                return 0
            }
        }
    }

    # ok
    return 1
}


# makeUniqueName --
#
# Adjusts the passed name string by killing invalid chars and making it
# locally unique, and returns the new name.
#
# Returns the new name.
#
proc makeUniqueName {key name} {
    global ned

    # check if item has a name at all
    if ![info exist ned($key,name)] {
        error "item $key has no name attribute"
    }

    # adjust name string: kill invalid chars and make it begin with letter
    regsub -all -- {[^a-zA-Z0-9_]} $name {} name
    regsub -- {^[0-9]} $name {_\0} name
    if {$name==""} {
        # leave original name
        return $ned($key,name)
    }

    # if it's a reserved word, append "1"
    set keywords {include import network module simple channel delay error datarate
                  for do true false ref ancestor input const sizeof endsimple endmodule
                  endchannel endnetwork endfor parameters gatesizes gates
                  submodules connections display on like machines to if index nocheck
                  numeric string bool xml xmldoc anytype cplusplus class message struct enum
                  fields properties extends int long char unsigned}
    set suffixneeded ""
    if {[lsearch $keywords $name]!=-1} {
        set suffixneeded "1"
    }

    # make name unique among all siblings by appending numbers
    foreach siblingkey [getChildren $ned($key,parentkey)] {
        if {$siblingkey!=$key && [info exist ned($siblingkey,name)]} {
            if [regexp -- "^${name}(\[0-9\]*)$" $ned($siblingkey,name) match suffix] {
                set suffix1 [expr $suffix + 1]
                if {$suffixneeded == ""} {
                    set suffixneeded $suffix1
                } else {
                    if {$suffix1 > $suffixneeded} {
                        set suffixneeded $suffix1
                    }
                }
            }
        }
    }

    return "$name$suffixneeded"
}


# getChildren --
#
# Find a child element within the given parent
#
proc getChildren {parentkey} {
    global ned
    return $ned($parentkey,childrenkeys)
}

# getChildrenWithType --
#
# Find a child element within the given parent
#
proc getChildrenWithType {parentkey type} {
    global ned

    set keys {}
    foreach key $ned($parentkey,childrenkeys) {
       if {$ned($key,type)==$type} {
           lappend keys $key
       }
    }
    return $keys
}

# getChildrenWithName --
#
# Find children within the given parent and with a given name
#
proc getChildrenWithName {parentkey value} {
   global ned

   set keys {}
   foreach key $ned($parentkey,childrenkeys) {
       if {[info exist ned($key,name)] && $ned($key,name)==$value} {
          lappend keys $key
       }
   }
   return $keys
}


# canvasIdFromItemKey --
#
# returns the id of canvas the item is on
#
proc canvasIdFromItemKey {key} {
    global ned canvas

    while {$key!=""} {
        foreach i [array names canvas "*,module-key"] {
            if {$canvas($i)==$key} {
                regsub -- ",module-key" $i "" canv_id
                return $canv_id
            }
        }
        set key $ned($key,parentkey)
    }
    return ""
}

# isItemPartOfItem --
#
# check if 1st item is under 2nd one in the data structure tree
#
proc isItemPartOfItem {key anc_key} {
    global ned
    while {$key!=""} {
        if {$key==$anc_key} {
            return 1
        }
        set key $ned($key,parentkey)
    }
    return 0
}

# itemKeyFromName --
#
# get a key from name and type
#
proc itemKeyFromName {name type} {
    global ned
    foreach i [array names ned "*,type"] {
        if {$ned($i)==$type} {
            regsub -- ",type" $i "" key
            if {$ned($key,name)==$name} {
               return $key
            }
        }
    }
    return ""
}

#
# get a key from canvas item id
#
proc itemKeyFromCid {cid {canv_id ""}} {
    global ned gned canvas

    if {$cid==""} {return ""}
    if {$canv_id==""} {set canv_id $gned(canvas_id)}

    foreach i [array names ned "*-cid"] {
       if {$ned($i)==$cid} {
          regsub -- ",.*-cid" $i "" key
          # make sure item is on this canvas
          if [isItemPartOfItem $key $canvas($canv_id,module-key)] {
              return $key
          }
       }
    }
    return ""
}

#
# return connections to/from module or submodule
#
proc connKeysOfItem {key} {
   global ned

   set keys ""
   foreach i [array names ned "*,src-ownerkey"] {
      if {$ned($i)==$key} {
         regsub -- ",src-ownerkey" $i "" connkey
         lappend keys $connkey
      }
   }
   foreach i [array names ned "*,dest-ownerkey"] {
      if {$ned($i)==$key} {
         regsub -- ",dest-ownerkey" $i "" connkey
         lappend keys $connkey
      }
   }
   return $keys
}

#
# change a key in the tmp_ned() array passed by name
#
proc changeKey {tmp_ned_name from to} {
   upvar $tmp_ned_name tmp_ned

   foreach i [array names tmp_ned "$from,*"] {
      regsub -- $from $i $to newi
      set tmp_ned($newi) $tmp_ned($i)
      unset tmp_ned($i)
   }

   foreach i [array names tmp_ned "*,*-ownerkey"] {
      if {$tmp_ned($i)==$from} {set tmp_ned($i) $to}
   }

}


# copyArrayFromNed
#
# Copy items from ned() into another array (the "clipboard")
#
proc copyArrayFromNed {tmp_ned_name keys} {
   global ned
   upvar $tmp_ned_name tmp_ned

   # clear target array
   foreach i [array names tmp_ned] {
      unset tmp_ned($i)
   }

   # copy out all needed elems from ned()
   foreach i [array names ned] {
      regsub -- ",.*" $i "" key
      if {[lsearch $keys $key]!=-1} {
         set tmp_ned($i) $ned($i)
      }
   }

   # null out canvas ids
   foreach i [array names tmp_ned "*-cid"] {
      set tmp_ned($i) ""
   }
}


# pasteArrayIntoNed
#
#  merge an array ("clipboard") similar to ned() into ned()
#  return the list of item keys
#
proc pasteArrayIntoNed {tmp_ned_name} {
   global ned
   upvar $tmp_ned_name tmp_ned

   # collect all keys from tmp_ned()
   set keys {}
   foreach i [array names tmp_ned "*,type"] {
      regsub -- ",type" $i "" key
      if {[lsearch $keys $key]==-1} {
         lappend keys $key
      }
   }

   # alter keys in tmp_ned to avoid conflicts
   foreach i $keys {
      changeKey tmp_ned $i "tmp-$i"
   }

   # pick a new key for each
   set new_keys {}
   foreach key $keys {
      set key "tmp-$key"

      set new_key $ned(nextkey)
      incr ned(nextkey)

      changeKey tmp_ned $key $new_key
      lappend new_keys $new_key
   }

   # merge array into ned()
   foreach i [array names tmp_ned ] {
      set ned($i) $tmp_ned($i)
   }
   return $new_keys
}


# checkArray --
#
# Data structure sanity check, for debugging.
# Check ned() array if it conforms to the data dictionary
#
proc checkArray {} {
   global ned ned_attr

   # check that ned() contains only those attrs in ned_attr()
   foreach i [array names ned] {
      if {$i=="nextkey"} continue
      regsub -- ",.*" $i "" key
      regsub -- ".*," $i "" attr
      if {![info exist ned($key,type)]} {
         puts "ned($i)=$ned($i): invalid item, has no 'type' attribute"
         continue
      }
      set type $ned($key,type)
      if {![info exist ned_attr($type,$attr)]} {
          puts "ned($key/$type): unwanted attribute '$attr'"
      }
   }

   # check that ned() contains all elements in ned_attr()
   foreach i [array names ned "*,type"] {
      regsub -- ",.*" $i "" key
      set type $ned($key,type)
      foreach j [array names ned_attr "$type,*"] {
         regsub -- "$type," $j "" attr
         if {![info exist ned($key,$attr)]} {
            puts "ned($key/$type): missing attribute '$attr'"
         }
      }
   }

   # check parent-child relationships
   foreach i [array names ned "*,parentkey"] {
      regsub -- ",.*" $i "" key
      if {![info exist ned($key,type)]} {
         puts "ned($i)=$ned($i): invalid item, has no 'type' attribute"
         continue
      }
      set type $ned($key,type)
      set ownerkey $ned($i)
      if {![info exist ned($ownerkey,type)]} {
         puts "ned($key/$type): parent '$ownerkey' not in array"
      } else {
         set parenttype $ned($ownerkey,type)
         set possibleparents $ned_attr($ned($key,type)-parents)
         if {[lsearch $possibleparents $parenttype]==-1} {
            puts "ned($key/$type): parent type '$parenttype' not allowed"
            puts "   (allowed ones: $possibleparents)"
         }
      }
   }
}


# markNedFileOfItemDirty --
#
# Returns 1 if the dirty bit was just set to true.
# updateTreeManager must always be called after this!
#
proc markNedFileOfItemDirty {key} {
    global ned

    # seach up until nedfile item
    while {$ned($key,type)!="nedfile"} {
        set key $ned($key,parentkey)
    }

    # mark nedfile dirty
    if {$ned($key,aux-isdirty)==0} {
        set ned($key,aux-isdirty) 1
        return 1
    }
    return 0
}


proc nedfileIsDirty {key} {
    global ned

    if {$ned($key,type)!="nedfile"} {error "internal error in nedfileIsDirty"}

    return $ned($key,aux-isdirty)
}


proc nedfileClearDirty {key} {
    global ned

    if {$ned($key,type)!="nedfile"} {error "internal error in nedfileIsDirty"}

    set ned($key,aux-isdirty) 0

    updateTreeManager
}


proc deleteNedfile {nedfilekey} {
    global ned

    # guard against openUnnamedCanvas (called from destroyCanvas)
    set ned($nedfilekey,being-deleted) 1

    # close all related canvases
    foreach key $ned($nedfilekey,childrenkeys) {
        set ned($key,being-deleted) 1
        set canv_id [canvasIdFromItemKey $key]
        if {$canv_id!=""} {
            destroyCanvas $canv_id
        }
    }
    set canv_id [canvasIdFromItemKey $nedfilekey]
    if {$canv_id!=""} {
        destroyCanvas $canv_id
    }

    # delete from parent (root)
    set root $ned($nedfilekey,parentkey)
    set pos [lsearch -exact $ned($root,childrenkeys) $nedfilekey]
    set ned($root,childrenkeys) [lreplace $ned($root,childrenkeys) $pos $pos]

    # delete from array
    deleteModuleData $nedfilekey
}


# deleteModuleData --
#
# used by switchToGraphics and deleteNedfile.
#
# $key must be a top-level item (module,simple,channel etc.).
# This proc is a simplified version of deleteItem:
#  o  deleting graphics stuff from the canvas is omitted
#     (callers are supposed to clear the whole canvas manually)
#  o  recursive deletion of data is also simplified
#     (we don't need to care about deleting 'related items' (like
#     ned(*,*-ownerkey)) because such relations do not occur between
#     top-level items)
#
proc deleteModuleData {key} {
    global ned ned_attlist

    # delete children recursively
    foreach childkey $ned($key,childrenkeys) {
        deleteModuleData $childkey
    }

    # - deleting non-child linked objects omitted
    # - deleting item from canvas is omitted
    # - potentially closing the canvas is omitted
    # - unlink from parent omitted

    # delete from array
    set type $ned($key,type)
    foreach field $ned_attlist($type) {
        unset ned($key,$field)
    }
    foreach field $ned_attlist(common) {
        catch {unset ned($key,$field)}
    }
}

# getContainingModule --
#
# get module that contains item.
#
proc getContainingModule {key} {
    global ned
    while {$key!="" && $ned($key,type)!="module" && $ned($key,type)!="simple"}  {
        set key $ned($key,parentkey)
    }
    return $key
}

# findNEDFileByFilename --
#
# If given NED file has already been loaded, returns its key, otherrwise ""
#
proc findNEDFileByFilename {fname} {
    global ned

    foreach key [getChildrenWithType 0 nedfile] {
        if {$fname == $ned($key,filename)} {
            return $key
        }
    }
    return ""
}

# getModuleNameList --
#
# get a list of simple and compound module type names
#
proc getModuleNameList {} {
    global ned
    set list {}
    foreach i [array names ned "*,type"] {
        if {$ned($i)=="module" || $ned($i)=="simple"} {
            regsub -- ",type" $i "" key
            lappend list $ned($key,name)
        }
    }
    return $list
}

# getChannelNameList --
#
# get a list of channel type names.
#
# Could be merged with getModuleNameList, it's almost the same.
#
proc getChannelNameList {} {
    global ned
    set list {}
    foreach i [array names ned "*,type"] {
        if {$ned($i)=="channel"} {
            regsub -- ",type" $i "" key
            lappend list $ned($key,name)
        }
    }
    return $list
}


# getNameList --
#
# get a list names within given section (params/substparams/...) of module/submodule/network/...
#
proc getNameList {componentkey sectiontype} {
    global ned
    set list {}
    set sectionkeylist [getChildrenWithType $componentkey $sectiontype]
    foreach sectionkey $sectionkeylist {
        foreach key [getChildren $sectionkey] {
            if {[info exist ned($key,name)]} {
                if {[lsearch -exact $list $ned($key,name)]==-1} {
                    lappend list $ned($key,name)
                }
            }
        }
    }
    return $list
}

#
# Finds all occurrences of the given gate in the compound module, and
# return the list of indices it occurs with. E.g. {0 1 i j+1}
# $submodkey=$modkey indicates parent module.
#
proc getAllGateUsages {modkey submodkey gatename} {
    global ned

    set indices {}

    set connskey [getChildrenWithType $modkey conns]

    # collect connections, then loop through them
    set connkeys [getChildrenWithType $connskey conn]
    foreach forloopkey [getChildrenWithType $connskey forloop] {
        set connkeys [concat $connkeys [getChildrenWithType $forloopkey conn]]
    }

    # collect indices for that connection
    foreach connkey $connkeys {
       if {$ned($connkey,src-ownerkey)==$submodkey && $ned($connkey,srcgate)==$gatename} {
           if {$ned($connkey,src-gate-plusplus)} {
               lappend indices "++"
           } else {
               lappend indices $ned($connkey,src-gate-index)
           }
       }
       if {$ned($connkey,dest-ownerkey)==$submodkey && $ned($connkey,destgate)==$gatename} {
           if {$ned($connkey,dest-gate-plusplus)} {
               lappend indices "++"
           } else {
               lappend indices $ned($connkey,dest-gate-index)
           }
       }
    }
    return $indices
}

proc getMaxGateIndex {connkey srcdest} {
    global ned

    set modkey    [getContainingModule $connkey]
    set submodkey $ned($connkey,${srcdest}-ownerkey)
    set gatename  $ned($connkey,${srcdest}gate)

    return [getMaxGateIndex2 $modkey $submodkey $gatename]
}

proc getMaxGateIndex2 {modkey submodkey gatename} {
    global ned

    # Look up all connections of current module, and find the
    # maximum index of their source/destination.
    # If we find any index which is not numeric, we return -1.
    # Note: we could be a little smarter, e.g. if we find gate[i] inside a for i=0..n-1
    # loop we might conclude that gatesize is (n-1)+1, but that's just another
    # special case. The proper, full solution looks sort of hard...

    set indices [getAllGateUsages $modkey $submodkey $gatename]

    set max -1
    foreach index $indices {
        if {![string is integer $index]} {return -1}
        if {$max < $index} {set max $index}
    }
    return $max
}

# getGateNameList --
#
# get a list of gates of a module type, to be offered for connecting (ConnProps dialog)
# vector gates are suffixed with either "++" or an appropriate index.
#
# if {$config(autoextend)} {set vecsuffix "++"} else {set vecsuffix "\[...\]"}
# but: "++" not allowed on parent module's gates
#
proc getGateNameList {modkey submodkey modtypekey gatetype} {
    global ned
    set list {}
    set sectionkeylist [getChildrenWithType $modtypekey gates]
    foreach sectionkey $sectionkeylist {
        foreach key [getChildren $sectionkey] {
            if {$ned($key,type)=="gate" && $ned($key,gatetype)==$gatetype} {
                # gate found
                set gatename $ned($key,name)
                if {!$ned($key,isvector)} {
                    lappend list "$gatename"
                } else {
                    # vector
                    set usedindices [getAllGateUsages $modkey $submodkey $gatename]
                    set max [getMaxGateIndex2 $modkey $submodkey $gatename]
                    if {$usedindices==""} {
                        lappend list "$gatename++"
                        lappend list "$gatename\[0\]"
                        lappend list "$gatename\[...\]"
                    } elseif {$max!=-1} {
                        set maxplus1 [expr $max + 1]
                        lappend list "$gatename++"
                        lappend list "$gatename\[$maxplus1\]"
                        lappend list "$gatename\[...\]"
                    } else {
                        lappend list "$gatename++"
                        lappend list "$gatename\[...\]"
                    }
                }
            }
        }
    }
    return $list
}

# getSubmodNameList --
#
# get a list of submodules in a compound module type -- submod vectors are suffixed with
# $vecsuffix (which is can be conveniently '[]' or '[...]')
#
proc getSubmodNameList {modtypekey vecsuffix} {
    global ned
    set list {}
    set sectionkeylist [getChildrenWithType $modtypekey submods]
    foreach sectionkey $sectionkeylist {
        foreach key [getChildren $sectionkey] {
            if {$ned($key,type)=="submod"} {
                if {[lsearch -exact $list $ned($key,name)]==-1} {
                    if {$ned($key,vectorsize)!=""} {
                        lappend list $ned($key,name)$vecsuffix
                    } else {
                        lappend list $ned($key,name)
                    }
                }
            }
        }
    }
    return $list
}

# findSubmodule --
#
# Find submodule from its name within a specified compound module
#
proc findSubmodule {modkey submodname} {
    global ned

    set submodskey [getChildrenWithType $modkey submods]
    if {$submodskey==""} {
        return ""
    }
    set submodkey [getChildrenWithName $submodskey $submodname]
    return $submodkey
}



# nedClipboard:cut --
#
# doesn't work yet
#
proc nedClipboard:cut {} {
       tk_messageBox -title "GNED" -icon info -type ok -message "Copy/paste is only available in text mode."
       return
       editCopy
       deleteSelected
}


# nedClipboard:copy --
#
# doesn't work yet
#
proc nedClipboard:copy {} {
       tk_messageBox -title "GNED" -icon info -type ok -message "Copy/paste is only available in text mode."
       return

       global clipboard_ned ned

       set selection [selectedItems]

       # accept only submodules and connections whose both ends will be copied
       set keys {}
       foreach key $selection {
          if {$ned($key,type)=="submod"} {
             lappend keys $key
          } elseif {$ned($key,type)=="conn"} {
             if {[lsearch $selection $ned($key,src-ownerkey)]!=-1 &&
                 [lsearch $selection $ned($key,dest-ownerkey)]!=-1} \
             {
                 lappend keys $key
             }
          }
       }

       # copy out items to clipboard
       copyArrayFromNed clipboard_ned $keys
       return $keys
}


# nedClipboard:paste --
#
# doesn't work yet
#
proc nedClipboard:paste {} {
       tk_messageBox -title "GNED" -icon info -type ok -message "Copy/paste is only available in text mode."
       return

       global clipboard_ned ned gned canvas

       deselectAllItems

       # offset disp-xpos and disp-ypos
       foreach i [array names clipboard_ned "*,?-pos"] {
          set clipboard_ned($i) [expr $clipboard_ned($i)+8]
       }

       set new_keys [pasteArrayIntoNed clipboard_ned]

       # make items owned by the module on this canvas
       set modkey $canvas($gned(canvas_id),module-key)
       foreach key $new_keys {
          set ned($key,module-ownerkey) $modkey
          selectItem $key
       }

       # draw pasted items on canvas
       drawItems $new_keys
}

