#=================================================================
#  GNEDRC.TCL - part of
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


# saveConfig --
#
#
proc saveConfig {{fname {}}} {
   global config

   if {$fname == ""} {
       set fname $config(configfile)

       catch {cd [file dirname $fname]}
       set fname [file tail $fname]
       set fname [tk_getSaveFile -defaultextension {} \
                  -initialdir [pwd] -initialfile $fname \
                  -filetypes {{{config files} {*.rc}} {{All files} {*}}}]
   }

   if {$fname!=""} {

       # work around Tcl/Tk bug: on Windows, .rc is appended to .gnedrc
       if [string match "*.gnedrc.rc" $fname] {
          set fname [string range $fname 0 [expr [string length $fname]-4]]
       }

       if [catch {open $fname w} fout] {
          tk_messageBox -icon warning -type ok -message "Error: $fout"
          return
       }

       busy "Saving config file $fname..."

       puts $fout "\n# gned configuration"
       foreach i [lsort [array names config]] {
          puts $fout "set config($i) \{$config($i)\}"
       }

       close $fout
       busy

       set config(configfile) $fname
    }
}


# loadConfig --
#
#
proc loadConfig {{fname {}}} {
   global config

   if {$fname == ""} {
       set fname $config(configfile)

       catch {cd [file dirname $fname]}
       set fname [file tail $fname]
       set fname [tk_getOpenFile -defaultextension {} \
                   -initialdir [pwd] -initialfile $fname \
                   -filetypes {{{config files} {*.rc}} {{All files} {*}}}]
   }

   if {$fname!=""} {

       if [catch {open $fname r} fin] {
           tk_messageBox -icon warning -type ok -message "Error: $fin"
           return
       }

       busy "Loading config file $fname..."

       # just evaluate saved file as TCL script
       if [catch {source $fname} errmsg] {
           tk_messageBox -icon warning -type ok -message "Error: $errmsg"
           busy
           return
       }

       busy

       set config(configfile) $fname

       reflectConfigInGUI
   }
}


# reflextConfigInGUI --
#
# make config settings effective on the GUI
#
proc reflectConfigInGUI {} {
   global config gned

   if {$config(snaptogrid)} {
       $gned(graphics-toolbar).grid config -relief sunken
   } else {
       $gned(graphics-toolbar).grid config -relief raised
   }
}

