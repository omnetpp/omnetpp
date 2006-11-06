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
# Implements Options|General...
#
proc editGeneralOptions {} {

    global config

    # create dialog with OK and Cancel buttons
    set w .dlg
    createOkCancelDialog $w "General configuration"

    # add entry fields and focus on first one
    frame $w.f.f1 -relief groove -border 2
    label-entry $w.f.f1.titlefmt "Default title format:" $config(titlefmt)
    $w.f.f1.titlefmt.e config -width 30
    commentlabel $w.f.f1.c "Setting doesn't affect already loaded vectors."
    commentlabel $w.f.f1.c2 "Following macros can be used:\n\$NAME,\$MODULE,\$ID,\$MULT,\$FILENAME,\$FILEPATH"

    pack $w.f.f1 -expand 0 -fill x -side top
    pack $w.f.f1.titlefmt -anchor center -expand 0 -fill x -side top
    pack $w.f.f1.c -anchor center -expand 0 -fill x -side top
    pack $w.f.f1.c2 -anchor center -expand 0 -fill x -side top

    global tmp
    set tmp(type) $config(rightarrow)

    frame $w.f.f2 -relief groove -border 2
    label $w.f.f2.l -text "Double-click in left pane"
    radiobutton $w.f.f2.r1 -text "copies vectors to right pane" -value "copy" -variable tmp(type)
    radiobutton $w.f.f2.r2 -text "moves vectors to right pane" -value "move" -variable tmp(type)

    pack $w.f.f2.l -anchor w -expand 0 -fill none -side top
    pack $w.f.f2.r1 -anchor w -expand 0 -fill none -side top
    pack $w.f.f2.r2 -anchor w -expand 0 -fill none -side top
    pack $w.f.f2 -expand 0 -fill x -side top

    # exec the dialog, extract its contents if OK was pressed, then delete dialog
    if {[execOkCancelDialog $w] == 1} {
        set config(titlefmt) [$w.f.f1.titlefmt.e get]
        set config(rightarrow) $tmp(type)
    }
    destroy $w
}


#
# Prepare a string for writing it out into .ploverc: insert backslash
# before every "\n" in it
#
proc escape {line} {
    regsub -all -- "\n" $line "\\\\\n  " line
    return $line
}


#
# Assemble all filter data into a list, to be written into .ploverc as a string
#
proc serializeFilter {name} {
   set data "customfilter"
   lappend data $name
   lappend data [opp_nodetype $name category]
   lappend data [opp_nodetype $name description]
   lappend data [opp_nodetype $name attrs]
   set n [opp_compoundfiltertype $name numSubfilters]
   set subfilterlist {}
   for {set k 0} {$k<$n} {incr k} {
       set subfilter {}
       lappend subfilter [opp_compoundfiltertype $name subfilterType $k]
       lappend subfilter [opp_compoundfiltertype $name subfilterComment $k]
       lappend subfilter [opp_compoundfiltertype $name subfilterAttrs $k]
       lappend subfilterlist $subfilter
   }
   lappend data $subfilterlist
   return $data
}


#
# Restore filter from a line in .ploverc
#
proc deserializeFilter {data} {
   if {[llength $data]!=6} {
       error "wrong number of items parsing the filter"
   }

   set name [lindex $data 1]
   set category [lindex $data 2]
   set description [lindex $data 3]
   set attrs [lindex $data 4]
   set subfilterlist [lindex $data 5]

   opp_compoundfiltertype_create $name
   opp_compoundfiltertype $name setDescription $description

   foreach attr $attrs {
       set aname [lindex $attr 0]
       set adescr [lindex $attr 1]
       set adefault [lindex $attr 2]
       opp_compoundfiltertype $name setAttr $aname $adescr  $adefault
   }

   set k 0
   foreach subfilter $subfilterlist {
       set sftype [lindex $subfilter 0]
       set sfcomment [lindex $subfilter 1]
       set sfattrs [lindex $subfilter 2]
       opp_compoundfiltertype $name insertSubfilter $k
       opp_compoundfiltertype $name setSubfilter $k $sftype $sfcomment

       foreach sfattr $sfattrs {
           set aname [lindex $sfattr 0]
           set avalue [lindex $sfattr 1]
           opp_compoundfiltertype $name setSubfilterAttr $k $aname $avalue
       }
       incr k
   }
}


#
# Save .ploverc
#
proc saveConfig {{fname {}}} {
   global g filter config

   if {$fname == ""} {
       set fname $config(configfile)

       catch {cd [file dirname $fname]}
       set fname [file tail $fname]
       set fname [tk_getSaveFile -defaultextension {} \
                  -initialdir [pwd] -initialfile $fname \
                  -filetypes {{{config files} {*.rc}} {{All files} {*}}}]
   }

   if {$fname!=""} {

       # work around Tcl/Tk bug: on Windows, .rc is appended to .ploverc
       if [string match "*.ploverc.rc" $fname] {
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

           # save custom filters
           puts $fout "\n# custom filters"
           foreach name [opp_getnodetypes] {
               set cat [opp_nodetype $name category]
               if {$cat=="custom filter"} {
                   puts $fout [escape [serializeFilter $name]]
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
# Read .ploverc
#
proc loadConfig {{fname {}}} {
   global g filter config

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
                   if {$fversion!="3.0" && $fversion!="3.1" && $fversion!="3.2" && $fversion!="3.3" && $fversion!="3.4"} {
                       error "unrecognized file version $fversion"
                   }
               }
               if {$fversion==""} {
                   break ;# missing version info: old .ploverc -- fail silently
               }
               if {$cat == "config"} {
                   set key [lindex $line 1]
                   set value [lindex $line 2]
                   set config($key) $value
               } elseif {$cat == "customfilter"} {
                   deserializeFilter $line
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
               tk_messageBox -icon warning -type ok -title {Error reading .ploverc} -message "Error at $fname line $lineno: $errmsg"
           } else {
               tk_messageBox -icon warning -type ok -title {Error reading .ploverc} -message "Error reading $fname: $errmsg"
           }
       }

       set config(configfile) $fname
    }

}

