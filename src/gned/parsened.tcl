#==========================================================================
#  PARSENED.TCL -
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


#------------------------------------------------
# Data structure is defined in datadict.tcl
#------------------------------------------------


# NedParser_createNedElement --
#
# This procedure is called from parsened.cc, NEDParser::create()
#
proc NedParser_createNedElement {nedarrayname type parentkey} {
   global ddict ddfields
   upvar #0 $nedarrayname nedarray

   # choose key
   set key $nedarray(nextkey)
   incr nedarray(nextkey)

   # add ned() fields
   foreach field $ddfields(common) {
      set nedarray($key,$field) $ddict(common,$field)
   }
   foreach field $ddfields($type) {
      set nedarray($key,$field) $ddict($type,$field)
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

