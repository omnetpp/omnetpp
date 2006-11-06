#=================================================================
#  CONFIG.TCL - part of
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



#
# Prepare a string for writing it out into .scalarsrc: insert backslash
# before every "\n" in it
#
proc escape {line} {
    regsub -all -- "\n" $line "\\\\\n  " line
    return $line
}


#
# Save .scalarsrc
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

       # work around Tcl/Tk bug: on Windows, .rc is appended to .scalarsrc
       if [string match "*.scalarsrc.rc" $fname] {
           set fname [string range $fname 0 [expr [string length $fname]-4]]
       }

       if [catch {
           busyCursor "Saving config file $fname..."
           set fout [open $fname w]

           # save config
           puts $fout "version 3.4"

           puts $fout "\n# general config"
           foreach i [lsort [array names config]] {
               if {$i!="configfile" && $i!="tmp"} {
                   puts $fout [escape [list "config" $i $config($i)]]
               }
           }

           # close
           close $fout
           idleCursor
           set config(configfile) $fname

       } errmsg] {
           # handle errors
           idleCursor
           tk_messageBox -icon warning -type ok -message "Error: $errmsg"
       }
    }
}


#
# Read .scalarsrc
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

       if [catch {
           busyCursor "Loading config file $fname..."
           set fin [open $fname r]

           set lineno 1
           set fversion ""
           while {[gets $fin line]>=0} {
               if {$line == ""} {incr lineno; continue}
               if [string match {#*} $line] {incr lineno; continue}

               # restore lines that use backslash continuation
               while {[string index $line end]=="\\" && [gets $fin linecont]>=0} {
                   set line "[string range $line 0 end-1]\n[string range $linecont 2 end]"
                   incr lineno
               }
               set cat [lindex $line 0]
               if {$cat == "version"} {
                   set fversion [lindex $line 1]
                   if {$fversion!="3.2" && $fversion!="3.3" && $fversion!="3.4"} {
                       error "unrecognized file version $fversion"
                   }
               }
               if {$cat == "config"} {
                   set key [lindex $line 1]
                   set value [lindex $line 2]
                   set config($key) $value
               } elseif {$cat != "version"} {
                   error "unrecognized keyword at beginning of line"
               }
               incr lineno
           }
           close $fin

           idleCursor

       } errmsg] {
           # handle errors
           idleCursor

           if [info exist lineno] {
               tk_messageBox -icon warning -type ok -title {Error reading .scalarsrc} -message "Error at $fname line $lineno: $errmsg"
           } else {
               tk_messageBox -icon warning -type ok -title {Error reading .scalarsrc} -message "Error reading $fname: $errmsg"
           }
       }

       set config(configfile) $fname
    }

}

