#=================================================================
#  VECTORCFG.TCL - part of
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
# to be rewritten...
#
proc saveVectorConfig {} {

   global g vec config

   set fname $config(vectorconfigfile)

   catch {cd [file dirname $fname]}
   set fname [file tail $fname]
   set fname [tk_getSaveFile -defaultextension ".cfg" \
              -initialdir [pwd] -initialfile $fname \
              -filetypes {{{vector config files} {*.cfg}} {{All files} {*}}}]

   if {$fname!=""} {

       if [catch {open $fname w} fout] {
          tk_messageBox -icon warning -type ok -message "Error: $fout"
          return
       }

       busyCursor "Saving vector config file $fname..."

       puts $fout "\n# vectors"
       foreach i [lsort [array names vec]] {
          puts $fout "set vec($i) \{$vec($i)\}"
       }

       puts $fout "\n# listbox 1"
       set num [$g(listbox1) index end]
       for {set i 0} {$i < $num} {incr i} {
          puts $fout "\$g(listbox1) insert end \{[$g(listbox1) get $i]\}"
       }

       puts $fout "\n# listbox 2"
       set num [$g(listbox2) index end]
       for {set i 0} {$i < $num} {incr i} {
          puts $fout "\$g(listbox2) insert end \{[$g(listbox2) get $i]\}"
       }

       close $fout

       idleCursor

       set config(vectorconfigfile) $fname
    }
}


#
# To be rewritten...
# (vector ids should be matched by module and vector name)
#
proc loadVectorConfig {} {

   global g vec config

   set fname $config(vectorconfigfile)

   catch {cd [file dirname $fname]}
   set fname [file tail $fname]
   set fname [tk_getOpenFile -defaultextension ".cfg" \
              -initialdir [pwd] -initialfile $fname \
              -filetypes {{{vector config files} {*.cfg}} {{All files} {*}}}]

   if {$fname!=""} {

       if [catch {open $fname r} fin] {
          tk_messageBox -icon warning -type ok -message "Error: $fin"
          return
       }

       busyCursor "Loading config file $fname..."

       $g(listbox1) delete 0 end
       $g(listbox2) delete 0 end

       # just evaluate saved file as TCL script
       if [catch {source $fname} errmsg] {
          tk_messageBox -icon warning -type ok -message "Error: $errmsg"
          return
       }

       idleCursor
       status 1
       status 2

       set config(vectorconfigfile) $fname
    }

}

