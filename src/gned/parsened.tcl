#=================================================================
#  PARSENED.TCL - part of
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


#------------------------------------------------
# Data structure is defined in datadict.tcl
#------------------------------------------------


# NedParser_createNedElement --
#
# This procedure is called from parsened.cc, NEDParser::create()
#
proc NedParser_createNedElement {nedarrayname type parentkey} {
   global ned_attr ned_attlist
   upvar #0 $nedarrayname nedarray

   # choose key
   set key $nedarray(nextkey)
   incr nedarray(nextkey)

   # add ned() fields
   foreach field $ned_attlist(common) {
      set nedarray($key,$field) $ned_attr(common,$field)
   }
   foreach field $ned_attlist($type) {
      set nedarray($key,$field) $ned_attr($type,$field)
   }
   set nedarray($key,type) $type

   # set parent
   set nedarray($key,parentkey) $parentkey
   lappend nedarray($parentkey,childrenkeys) $key

   return $key
}

# NedParser_findChild --
#
# Find a child element within the given parent and with a given attribute value.
# (attr is usually "name".)
# This procedure is called from parsened.cc, NEDParser::create()
#
proc NedParser_findChild {nedarrayname parentkey attr value} {
   upvar #0 $nedarrayname nedarray

   set key ""
   foreach key1 $nedarray($parentkey,childrenkeys) {
       if {[info exist nedarray($key1,$attr)] && $nedarray($key1,$attr)==$value} {
          if {$key==""} {
             set key $key1
          } else {
             return "not unique"
          }
       }
   }
   return $key
}


