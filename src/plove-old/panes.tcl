#=================================================================
#  PANES.TCL - part of
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

proc cut_prefix {str p} {

    # if $str begins with $p, cut it off

    if {$p != "" && [string match "$p*" $str]} {
        set l [string length $p]
        set str [string range $str $l end]
    }
    return $str
}

proc regexp_to_stringmatch {expr} {

    # make 'string match' style pattern...

    regsub -all -- {\\} $expr {\\\\} expr
    regsub -all -- {\.} $expr {\\.} expr
    regsub -all -- {\^} $expr {\\^} expr
    regsub -all -- {\$} $expr {\\$} expr
    regsub -all -- {\[} $expr {\\[} expr
    regsub -all -- {\]} $expr {\\]} expr
    regsub -all -- {\(} $expr {\\(} expr
    regsub -all -- {\)} $expr {\\)} expr
    regsub -all -- {\+} $expr {\\+} expr
    regsub -all -- {\*} $expr {.*} expr
    regsub -all -- {\?} $expr {.?} expr
    regsub -all -- {\{} $expr {[} expr
    regsub -all -- {\}} $expr {]} expr

    return $expr
}

proc focusToPanel {n} {
    global g;
    if {$n=="1"} {
        focus $g(listbox1)
        $g(title1) config -relief solid -bd 1
        $g(title2) config -relief flat -bd 1
        #$g(title1) config -bg #e0e080
        #$g(title2) config -bg #e0e0e0
    } else {
        focus $g(listbox2)
        $g(title2) config -relief solid -bd 1
        $g(title1) config -relief flat -bd 1
        #$g(title2) config -bg #e0e080
        #$g(title1) config -bg #e0e0e0
    }
}

proc getfocusedpane {} {
    global g

    set focus [focus]
    if {$focus == $g(listbox1)} {
        return 1
    } elseif {$focus == $g(listbox2)} {
        return 2
    } else {
        error {Dunno which pane!}
    }
}

proc busy {{msg {}}} {
    global g

    if {$msg != ""} {
        $g(status) config -text $msg
        update idletasks
        . config -cursor watch
    } else {
        $g(status) config -text "Ready"
        update idletasks
        . config -cursor ""
    }
}

proc status {lb} {
    global g

    set sel [llength [$g(listbox$lb) curselection]]
    set num [$g(listbox$lb) index end]

    $g(status$lb) config -text "$sel selected out of $num"
}

proc fileOpen {} {
    global g config

    set fname $config(vectorfile)

    catch {cd [file dirname $fname]}
    set fname [file tail $fname]
    set fname [tk_getOpenFile -defaultextension ".vec" \
               -initialdir [pwd] -initialfile $fname \
               -filetypes {{{Output vectors} {*.vec *.vec.gz}} \
                           {{All files} {*}}}]

    if {$fname != ""} {
        loadVectorFile $fname
        set config(vectorfile) $fname
    }
}

proc fileExit {} {
    exitCleanup
    saveConfig "~/.ploverc"
    exit
}

proc makeTitle {id} {
    global vec config

    set tit $config(titlefmt)
    regsub {\$FILENAME}  $tit [file tail $vec($id,fname)]  tit
    regsub {\$FILEPATH}  $tit $vec($id,fname)  tit
    regsub {\$ID}        $tit $vec($id,vecid)  tit
    regsub {\$NAME}      $tit $vec($id,name)   tit
    regsub {\$MODULE}    $tit $vec($id,module) tit
    regsub {\$MULT}      $tit $vec($id,mult)   tit

    set vec($id,title)  $tit
}

proc loadVectorFile {fname} {
    global g vec config vec

    if {![file readable $fname] || [file type $fname]=="directory"} {
        tk_messageBox -icon warning -type ok -title Error \
             -message "File $fname doesn't exist or there's no read permission."
        return
    }

    busy "Running: grep \"^vector \" \"$fname\""

    set zipped [string match "*.gz" $fname]

    if {$zipped} {
       set cmd "$config(zcat) \"$fname\" | $config(grep) \"^vector\""
       busy "Running: $cmd"
       if [catch {set lines [exec $config(sh) << $cmd]} errmsg] {
           tk_messageBox -icon warning -type ok -title Error \
                         -message "Error spawning sh with zcat+grep: $errmsg"
           busy
           return
       }
    } else {
       busy "Running: grep \"^vector \" \"$fname\""
       if [catch {set lines [exec $config(grep) "^vector" "$fname"]} errmsg] {
           tk_messageBox -icon warning -type ok -title Error \
                         -message "Error spawning grep: $errmsg"
           busy
           return
       }
    }

    busy

    set lines [split $lines "\n"]

    foreach line $lines {
        set id $vec(nextid)
        incr vec(nextid)

        set vec($id,fname)  $fname
        set vec($id,vecid)  [lindex $line 1]
        set vec($id,module) [string trim [lindex $line 2] {"}]
        set vec($id,name)   [string trim [lindex $line 3] {"}]
        set vec($id,mult)   [lindex $line 4]
        set vec($id,style)  "default"
        set vec($id,title)  [makeTitle $id]
        set vec($id,zipped) $zipped
        set vec($id,filter) "none"
        set vec($id,filtpars) ""
        set vec($id,filtpfx) ""
        $g(listbox1) insert end "$vec($id,title) $g(spaces) $id"
    }
    status 1
}

proc moveOrCopyVectors {fr to} {
    global config

    if {$config(rightarrow) == "copy"} {
       copyVectors $fr $to
    } elseif {$config(rightarrow) == "move"} {
       moveVectors $fr $to
    } else {
       error "Bad config(rightarrow) value"
    }
}

proc moveVectors {fr to} {

    global g

    set sel [$g(listbox$fr) curselection]
    if {$sel == ""} {
        set v [$g(listbox$fr) get active]
        if {$v != "" } {
            $g(listbox$fr) delete active
            $g(listbox$to) insert end $v
            $g(listbox$to) selection set end
        }
    } else {
        foreach i $sel {
            set v [$g(listbox$fr) get $i]
            $g(listbox$to) insert end $v
            $g(listbox$to) selection set end
        }
        set sel [lsort -integer -decreasing $sel]
        foreach i $sel {
            $g(listbox$fr) delete $i
        }
    }
    focusToPanel $to
    status 1
    status 2
}

proc copyVectors {fr to} {
    global g vec

    set sel [$g(listbox$fr) curselection]
    if {$sel == ""} {
        set id [lindex [$g(listbox$fr) get active] end]
        dupVector $id $to
    } else {
        foreach v $sel {
            set id [lindex [$g(listbox$fr) get $v] end]
            dupVector $id $to
        }
        $g(listbox$fr) selection clear 0 end
    }
    focusToPanel $to
    status 1
    status 2
}

proc delVectors {{lb {}}} {
    global g vec
    if {$lb==""} {set lb [getfocusedpane]}

    set sel [$g(listbox$lb) curselection]
    if {$sel == ""} {
        set id [lindex [$g(listbox$lb) get active] end]
        foreach i [array names vec "$id,*"] {
            unset vec($i)
        }
        $g(listbox$lb) delete active
    } else {
        set sel [lsort -integer -decreasing $sel]
        foreach v $sel {
            set id [lindex [$g(listbox$lb) get $v] end]
            foreach i [array names vec "$id,*"] {
                unset vec($i)
            }
            $g(listbox$lb) delete $v
        }
    }
    status $lb
}

proc dupVector {id to} {
    global g vec

    if {[array names vec "$id,*"]==""} {return}

    set newid $vec(nextid)
    incr vec(nextid)

    foreach i [array names vec "$id,*"] {
        regsub "^$id," $i "$newid," newi
        set vec($newi) $vec($i)
    }
    $g(listbox$to) insert end "$vec($newid,title) $g(spaces) $newid"
    $g(listbox$to) selection set end
}

proc dupVectors {{lb {}}} {
    global g
    if {$lb==""} {set lb [getfocusedpane]}

    set sel [$g(listbox$lb) curselection]
    if {$sel == ""} {
        set id [lindex [$g(listbox$lb) get active] end]
        if {$id != ""} {
            dupVector $id $lb
        }
    } else {
        foreach i $sel {
            dupVector [lindex [$g(listbox$lb) get $i] end] $lb
        }
    }
    status $lb
}

proc selectAll {{lb {}}} {
    global g
    if {$lb==""} {set lb [getfocusedpane]}

    $g(listbox$lb) selection set 0 end
    status $lb
}

proc invertSelection {{lb {}}} {
    global g
    if {$lb==""} {set lb [getfocusedpane]}

    set n [$g(listbox$lb) index end]
    for {set i 0} {$i<$n} {incr i} {
        if [$g(listbox$lb) selection includes $i] {
            $g(listbox$lb) selection clear $i
        } else {
            $g(listbox$lb) selection set $i
        }
    }
    status $lb
}

proc selectVectors {{lb {}}} {
    global g vec
    if {$lb==""} {set lb [getfocusedpane]}

    createOkCancelDialog .ize "Select vectors"

    # add entry fields and focus on first one
    frame .ize.f.f1 -relief groove -border 2
    label-entry .ize.f.f1.title "Pattern in titles:"
    commentlabel .ize.f.f1.c {*,? wildcards are OK. Use {} for range: {a-z}.}
    pack .ize.f.f1.title -anchor center -expand 0 -fill x -side top
    pack .ize.f.f1.c -anchor center -expand 0 -fill x -side top
    pack .ize.f.f1 -expand 0 -fill x -side top

    global tmp
    set tmp(type) replace

    frame .ize.f.f2 -relief groove -border 2
    label .ize.f.f2.l -text "Mode:"
    radiobutton .ize.f.f2.r1 -text "replace selection" -value "replace" -variable tmp(type)
    radiobutton .ize.f.f2.r2 -text "extend selection" -value "extend" -variable tmp(type)

    pack .ize.f.f2.l -anchor w -expand 0 -fill none -side top
    pack .ize.f.f2.r1 -anchor w -expand 0 -fill none -side top
    pack .ize.f.f2.r2 -anchor w -expand 0 -fill none -side top
    pack .ize.f.f2 -expand 0 -fill x -side top

    focus .ize.f.f1.title.e

    # exec the dialog, extract its contents if OK was pressed, then delete dialog
    if {[execOkCancelDialog .ize] == 1} {

        if {$tmp(type)=="replace"} {
            $g(listbox$lb) selection clear 0 end
        }

        set patt [.ize.f.f1.title.e get]

        regsub -all -- {\[} $patt {\\[} patt
        regsub -all -- {\]} $patt {\\]} patt
        regsub -all -- {\{} $patt {[} patt
        regsub -all -- {\}} $patt {]} patt

        set n [$g(listbox$lb) size]
        for {set i 0} {$i < $n} {incr i} {
            if [string match "*$patt*" [$g(listbox$lb) get $i]] {
                $g(listbox$lb) selection set $i
            }
        }
        status $lb
    }
    destroy .ize

}

proc replaceInTitles {{lb {}}} {
    global g vec
    if {$lb==""} {set lb [getfocusedpane]}

    createOkCancelDialog .ize "Replace in titles"

    # add entry fields and focus on first one
    label-entry .ize.f.find "Find string:"
    label-entry .ize.f.repl "Replace with:"
    commentlabel .ize.f.c {Find string can contain *,? wildcards, and {} for range (like {a-z}. Replacing occurs only in SELECTED vectors.}
    pack .ize.f.find -anchor center -expand 0 -fill x -side top
    pack .ize.f.repl -anchor center -expand 0 -fill x -side top
    pack .ize.f.c -anchor center -expand 0 -fill x -side top
    focus .ize.f.find.e

    # exec the dialog, extract its contents if OK was pressed, then delete dialog
    if {[execOkCancelDialog .ize] == 1} {
        set find [.ize.f.find.e get]
        set repl [.ize.f.repl.e get]

        set find [regexp_to_stringmatch $find]

        set sel [$g(listbox$lb) curselection]
        if {$sel == ""} {
           set n [$g(listbox$lb) size]
           for {set i [expr $n-1]} {$i>=0} {incr i -1} {lappend sel $i}
        } else {
           set sel [lsort -integer -decreasing $sel]
        }

        foreach pos $sel {
            set str [$g(listbox$lb) get $pos]
            if [regexp -- $find $str] {
                set id [lindex $str end]
                regsub -- $find $vec($id,title) $repl vec($id,title)
                $g(listbox$lb) delete $pos
                $g(listbox$lb) insert $pos "$vec($id,title) $g(spaces) $id"
                $g(listbox$lb) selection set $pos
            }
        }
        status $lb
    }
    destroy .ize
}

proc vectorInfoDialog {id} {

    global vec g env

    # create dialog with OK and Cancel buttons
    createOkCancelDialog .ize "Vector info"

    set env_home $env(HOME)
    regsub -all -- {\\} $env_home {\\\\} env_home
    regsub -all -- {\.} $env_home {\\.} env_home
    regsub -all -- {\^} $env_home {\\^} env_home
    regsub -all -- {\$} $env_home {\\$} env_home
    regsub -all -- {\[} $env_home {\\[} env_home
    regsub -all -- {\]} $env_home {\\]} env_home
    regsub -all -- {\(} $env_home {\\(} env_home
    regsub -all -- {\)} $env_home {\\)} env_home
    regsub -all -- {\+} $env_home {\\+} env_home
    regsub -all -- {\*} $env_home {\\*} env_home
    regsub -all -- {\?} $env_home {\\?} env_home

    regsub "^$env_home/" $vec($id,fname) "~/" fname

    # add entry fields and focus on first one
    label-entry       .ize.f.title "Title:" $vec($id,title)
    label-sunkenlabel .ize.f.name  "Name:" $vec($id,name)
    label-sunkenlabel .ize.f.mod   "Module:" $vec($id,module)
    label-sunkenlabel .ize.f.vid   "File/Id:" "$fname  #$vec($id,vecid)"
    label-sunkenlabel .ize.f.mult  "Multiplicity:" $vec($id,mult)
    pack .ize.f.title -anchor center -expand 0 -fill x -side top
    pack .ize.f.name  -anchor center -expand 0 -fill x -side top
    pack .ize.f.mod   -anchor center -expand 0 -fill x -side top
    pack .ize.f.vid   -anchor center -expand 0 -fill x -side top
    pack .ize.f.mult  -anchor center -expand 0 -fill x -side top

    .ize.f.title.e config -width 40
    focus .ize.f.title.e

    # exec the dialog, extract its contents if OK was pressed, then delete dialog
    if {[execOkCancelDialog .ize] == 1} {
        set title [.ize.f.title.e get]

        destroy .ize

        return [list $title]
    }
    destroy .ize
}

proc vectorInfo {{lb {}} {y {}}} {

    global vec g
    if {$lb==""} {set lb [getfocusedpane]}

    set sel [$g(listbox$lb) curselection]
    if {[llength $sel] < 2} {
        if {$y == ""} {
            set sel [$g(listbox$lb) index active]
        } else {
            set sel [$g(listbox$lb) nearest $y]
        }
        $g(listbox$lb) selection clear 0 end
        $g(listbox$lb) selection set $sel
        set id [lindex [$g(listbox$lb) get $sel] end]
    } else {
        set id "-"
    }
    if {$id == ""} {
        tk_messageBox -icon info -title "Vector info" -type ok -message "No vectors selected."
        return
    }

    set res [vectorInfoDialog $id]

    if {$res!=""} {
        set title [lindex $res 0]
        set mult  [expr [string compare $id "-"] == 0]

        foreach pos $sel {
            set id [lindex [$g(listbox$lb) get $pos] end]

            if $mult {
            } else {
                set vec($id,title) $title

                $g(listbox$lb) delete $pos
                $g(listbox$lb) insert $pos "$vec($id,title) $g(spaces) $id"
                $g(listbox$lb) selection set $pos
            }
        }
        status $lb
    }
}

proc editVectorFilters {{lb {}}} {
    global vec g filt
    if {$lb==""} {set lb [getfocusedpane]}

    set sel [$g(listbox$lb) curselection]
    if {$sel == ""} {
        set sel [$g(listbox$lb) index active]
        set ids [lindex [$g(listbox$lb) get $sel] end]
    } else {
        set ids {}
        foreach s $sel {
            lappend ids [lindex [$g(listbox$lb) get $s] end]
        }
    }
    if {$ids == ""} {
        tk_messageBox -title "Vector plotting options" -icon info -type ok -message "No vectors selected."
        return
    }

    set res [filterParDialog $ids]

    if {$res != ""} {
        set title  [lindex $res 0]
        set style  [lindex $res 1]
        set prefix [lindex $res 2]
        set name   [lindex $res 3]
        set pars   [lindex $res 4]

        set mult  [expr [llength $ids]!=1]

        foreach pos $sel {
            set id [lindex [$g(listbox$lb) get $pos] end]

            if $mult {
                set vec($id,title) [cut_prefix $vec($id,title) $vec($id,filtpfx)]
                set vec($id,title)  "$prefix$vec($id,title)"
                set vec($id,filtpfx) $prefix
            } else {
                set vec($id,title) $title
                set vec($id,filtpfx) $prefix
            }
            $g(listbox$lb) delete $pos
            $g(listbox$lb) insert $pos "$vec($id,title) $g(spaces) $id"
            $g(listbox$lb) selection set $pos

            if {$style!=""} {
               set vec($id,style) $style
            }
            if {$name!=""} {
               set vec($id,filter) $name
               set vec($id,filtpars) $pars
            }
        }
    }
}

proc saveVectorConfig {} {

   global g gp vec config

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

       busy "Saving vector config file $fname..."

       puts $fout "\n# gnuplot config"
       foreach i [lsort [array names gp]] {
          puts $fout "set gp($i) \{$gp($i)\}"
       }

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

       busy

       set config(vectorconfigfile) $fname
    }
}

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

       busy "Loading config file $fname..."

       $g(listbox1) delete 0 end
       $g(listbox2) delete 0 end

       # just evaluate saved file as TCL script
       if [catch {source $fname} errmsg] {
          tk_messageBox -icon warning -type ok -message "Error: $errmsg"
          return
       }

       busy
       status 1
       status 2

       set config(vectorconfigfile) $fname
    }

}

