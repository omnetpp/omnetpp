#==========================================================================
#  DATA.TCL -
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


#------------------------------------------------
# Data structure is defined in datadict.tcl
#------------------------------------------------

#
# init global vars: add a root item with key 0
#
foreach i [array names ddict "common,*"] {
   regsub -- "common," $i "" field
   set ned(0,$field) $ddict($i)
}
foreach i [array names ddict "root,*"] {
   regsub -- "root," $i "" field
   set ned(0,$field) $ddict($i)
}
set ned(nextkey) 1


# addItem --
#
# add an item to ned()
#
proc addItem {type parentkey} {
   global ned ddict

   # choose key
   set key $ned(nextkey)
   incr ned(nextkey)

   # add ned() fields
   foreach i [array names ddict "common,*"] {
      regsub -- "common," $i "" field
      set ned($key,$field) $ddict($i)
   }
   foreach i [array names ddict "$type,*"] {
      regsub -- "$type," $i "" field
      set ned($key,$field) $ddict($i)
   }

   # set parent  (everyone has one except root)
   set ned($key,parentkey) $parentkey
   if {$type!="root"} {lappend ned($parentkey,childrenkeys) $key}

   # order is creation order
   set ned($key,order) $key

   # generate a unique name if necessary
   if [info exist ned($key,name)] {
      set name $ned($key,name)
      set max 0
      set name $ned($key,name)
      foreach i [array names ned "*,name"] {
         if [regsub -- "$name" $ned($i) "" n] {
             catch {if {int($n)>$max} {set max $n}}
         }
      }
      set max [expr int($max+1)]
      set ned($key,name) "$name$max"
   }

   # mark it "not selected" on canvas
   set ned($key,selected) 0

puts "dbg: addItem: $type $key added to $parentkey (its chilren now: $ned($parentkey,childrenkeys))"

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
   global ned canvas

   # delete item from canvas
   # We need to check if ned($key,canvasnum) exists because the item
   # may have been already deleted (eg. as a connection whose src/dest
   # mod was deleted)
   if {[info exist ned($key,canvasnum)] && $ned($key,canvasnum)!=""} {
      set c $canvas($ned($key,canvasnum),canvas)
      foreach i [array names ned "$key,*-cid"] {
         $c delete $ned($i)
      }
   }

   # unlink from parent
   set parentkey $ned($key,parentkey)
   set pos [lsearch -exact $ned($parentkey,childrenkeys) $key]
   set ned($parentkey,childrenkeys) [lreplace $ned($parentkey,childrenkeys) $pos $pos]

   # delete from array
   foreach i [array names ned "$key,*"] {
      unset ned($i)
   }

   # delete linked objects
puts "dbg: deleteItem to be checked here!!!"
   foreach i [array names ned "*,*ownerkey"] {
      if {[info exist ned($i)] && $ned($i)==$key} {
         regsub -- ",.*ownerkey" $i "" childkey
         deleteItem $childkey
      }
   }
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
proc itemKeyFromCid {cid {canvasnum ""}} {

   global ned gned

   if {$cid==""} {return ""}
   if {$canvasnum==""} {set canvasnum $gned(canvas_id)}

   foreach i [array names ned "*-cid"] {
      if {$ned($i)==$cid} {
         regsub -- ",.*-cid" $i "" key
         if [info exist ned($key,canvasnum)] {
            if {$ned($key,canvasnum)==$canvasnum} {
               return $key
            }
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

   # null out canvasnum fields
   foreach i [array names tmp_ned "*,canvasnum"] {
      set tmp_ned($i) ""
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
   global ned ddict

   # check that ned() contains only those attrs in ddict()
   foreach i [array names ned] {
      if {$i=="nextkey"} continue
      regsub -- ",.*" $i "" key
      regsub -- ".*," $i "" attr
      if {![info exist ned($key,type)]} {
         puts "ned($i)=$ned($i): invalid item, has no 'type' attribute"
         continue
      }
      set type $ned($key,type)
      if {![info exist ddict($type,$attr)]} {
          puts "ned($key/$type): unwanted attribute '$attr'"
      }
   }

   # check that ned() contains all elements in ddict()
   foreach i [array names ned "*,type"] {
      regsub -- ",.*" $i "" key
      set type $ned($key,type)
      foreach j [array names ddict "$type,*"] {
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
         set possibleparents $ddict($ned($key,type)-parents)
         if {[lsearch $possibleparents $parenttype]==-1} {
            puts "ned($key/$type): parent type '$parenttype' not allowed"
            puts "   (allowed ones: $possibleparents)"
         }
      }
   }
}

