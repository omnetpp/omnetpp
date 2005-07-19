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

proc makeListboxRow {id} {
    global vec g
    set s ""
    if {$vec($id,filter)!=""} {
        set s " \["
        set n [opp_compoundfiltertype $vec($id,filter) numSubfilters]
        for {set i 0} {$i<$n} {incr i} {
            if {$i!=0} {append s ","}
            append s [opp_compoundfiltertype $vec($id,filter) subfilterType $i]
        }
        append s "\]"
    }
    return "$vec($id,title)$s $g(spaces) $id"
}

proc loadVectorFile {fname} {
    global g vec config vec

    if {![file readable $fname] || [file type $fname]=="directory"} {
        tk_messageBox -icon warning -type ok -title Error \
             -message "File $fname doesn't exist or there's no read permission."
        return
    }

    set zipped [string match "*.gz" $fname]

    if {$zipped} {
        tk_messageBox -icon warning -type ok -title Error \
                     -message "Sorry, zipped vector files not yet supported with new Plove engine"
        return
    }

    # grep vectors from file
    busyCursor "Scanning $fname..."
    if [catch {set vectors [opp_getvectorlist $fname]} err] {
        idleCursor
        tk_messageBox -icon error -type ok -title Error -message "Error scanning $fname: $err"
        return
    }
    idleCursor

    # fill vec() array
    set idlist {}
    foreach vector $vectors {
        set id $vec(nextid)
        incr vec(nextid)
        lappend idlist $id

        set vec($id,fname)  $fname
        set vec($id,vecid)  [lindex $vector 0]
        set vec($id,module) [lindex $vector 1]
        set vec($id,name)   [lindex $vector 2]
        set vec($id,mult)   1
        set vec($id,title)  [makeTitle $id]
        set vec($id,zipped) $zipped
        set vec($id,filter) ""
        set vec($id,filtpfx) ""
    }

    # sort vectors
    set idlist [lsort -command compareVectors $idlist]

    # insert them into the listbox
    foreach id $idlist {
        $g(listbox1) insert end [makeListboxRow $id]
    }

    status 1
}

proc compareVectors {ida idb} {
    global vec
    set a $vec($ida,title)
    set b $vec($idb,title)
    if {$a==$b} {
        return 0
    } elseif {[lindex [lsort -dictionary [list $a $b]] 0]==$a} {
        return -1
    } else {
        return 1
    }
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
        # delete filter and array elements
        set id [lindex [$g(listbox$lb) get active] end]
        if {$id!=""} {
            if {$vec($id,filter)!=""} {
                opp_compoundfiltertype $vec($id,filter) delete
            }
            foreach i [array names vec "$id,*"] {
                unset vec($i)
            }
            $g(listbox$lb) delete active
        }
    } else {
        set sel [lsort -integer -decreasing $sel]
        foreach v $sel {
            # delete filter and array elements
            set id [lindex [$g(listbox$lb) get $v] end]
            if {$vec($id,filter)!=""} {
                opp_compoundfiltertype $vec($id,filter) delete
            }
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

    # copy array elements and duplicate filter
    foreach i [array names vec "$id,*"] {
        regsub "^$id," $i "$newid," newi
        set vec($newi) $vec($i)
    }
    if {$vec($id,filter)!=""} {
        set vec($newid,filter) [generateUniqFilterName]
        opp_compoundfiltertype $vec($id,filter) clone $vec($newid,filter)
    }
    $g(listbox$to) insert end [makeListboxRow $newid]
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

    set w .dlg
    createOkCancelDialog $w "Select vectors"

    # add entry fields and focus on first one
    frame $w.f.f1 -relief groove -border 2
    label-entry $w.f.f1.title "Pattern in titles:"
    commentlabel $w.f.f1.c {*,? wildcards are OK. Use {} for range: {a-z}.}
    pack $w.f.f1.title -anchor center -expand 0 -fill x -side top
    pack $w.f.f1.c -anchor center -expand 0 -fill x -side top
    pack $w.f.f1 -expand 0 -fill x -side top

    global tmp
    set tmp(type) replace

    frame $w.f.f2 -relief groove -border 2
    label $w.f.f2.l -text "Mode:"
    radiobutton $w.f.f2.r1 -text "replace selection" -value "replace" -variable tmp(type)
    radiobutton $w.f.f2.r2 -text "extend selection" -value "extend" -variable tmp(type)

    pack $w.f.f2.l -anchor w -expand 0 -fill none -side top
    pack $w.f.f2.r1 -anchor w -expand 0 -fill none -side top
    pack $w.f.f2.r2 -anchor w -expand 0 -fill none -side top
    pack $w.f.f2 -expand 0 -fill x -side top

    focus $w.f.f1.title.e

    # exec the dialog, extract its contents if OK was pressed, then delete dialog
    if {[execOkCancelDialog $w] == 1} {

        if {$tmp(type)=="replace"} {
            $g(listbox$lb) selection clear 0 end
        }

        set patt [$w.f.f1.title.e get]

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
    destroy $w

}

proc replaceInTitles {{lb {}}} {
    global g vec
    if {$lb==""} {set lb [getfocusedpane]}

    set w .dlg
    createOkCancelDialog $w "Replace in titles"

    # add entry fields and focus on first one
    label-entry $w.f.find "Find string:"
    label-entry $w.f.repl "Replace with:"
    commentlabel $w.f.c {Find string can contain *,? wildcards, and {} for range (like {a-z}. Replacing occurs only in SELECTED vectors.}
    pack $w.f.find -anchor center -expand 0 -fill x -side top
    pack $w.f.repl -anchor center -expand 0 -fill x -side top
    pack $w.f.c -anchor center -expand 0 -fill x -side top
    focus $w.f.find.e

    # exec the dialog, extract its contents if OK was pressed, then delete dialog
    if {[execOkCancelDialog $w] == 1} {
        set find [$w.f.find.e get]
        set repl [$w.f.repl.e get]

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
                $g(listbox$lb) insert $pos [makeListboxRow $id]
                $g(listbox$lb) selection set $pos
            }
        }
        status $lb
    }
    destroy $w
}

proc vectorInfoDialog {id} {

    global vec g env

    # create dialog with OK and Cancel buttons
    set w .dlg
    createOkCancelDialog $w "Vector info"

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
    label-entry       $w.f.title "Title:" $vec($id,title)
    label-sunkenlabel $w.f.name  "Name:" $vec($id,name)
    label-sunkenlabel $w.f.mod   "Module:" $vec($id,module)
    label-sunkenlabel $w.f.vid   "File/Id:" "$fname  #$vec($id,vecid)"
    label-sunkenlabel $w.f.mult  "Multiplicity:" $vec($id,mult)
    pack $w.f.title -anchor center -expand 0 -fill x -side top
    pack $w.f.name  -anchor center -expand 0 -fill x -side top
    pack $w.f.mod   -anchor center -expand 0 -fill x -side top
    pack $w.f.vid   -anchor center -expand 0 -fill x -side top
    pack $w.f.mult  -anchor center -expand 0 -fill x -side top

    $w.f.title.e config -width 40
    focus $w.f.title.e

    # exec the dialog, extract its contents if OK was pressed, then delete dialog
    if {[execOkCancelDialog $w] == 1} {
        set title [$w.f.title.e get]

        destroy $w

        return [list $title]
    }
    destroy $w
}

proc vectorInfo {{lb ""} {y ""}} {

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
    if {$id == ""} return

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
                $g(listbox$lb) insert $pos [makeListboxRow $id]
                $g(listbox$lb) selection set $pos
            }
        }
        status $lb
    }
}

proc editVectorFilters {{lb ""}  {y ""}} {
    global vec g

    # build idlist
    if {$lb==""} {set lb [getfocusedpane]}

    set sel [$g(listbox$lb) curselection]
    if {$sel == ""} {
        if {$y == ""} {
            set sel [$g(listbox$lb) index active]
        } else {
            set sel [$g(listbox$lb) nearest $y]
        }
        $g(listbox$lb) selection clear 0 end
        $g(listbox$lb) selection set $sel
        set ids [lindex [$g(listbox$lb) get $sel] end]
    } else {
        set ids {}
        foreach s $sel {
            lappend ids [lindex [$g(listbox$lb) get $s] end]
        }
    }
    if {$ids == ""} return

    # invoke dialog with idlist
    set res [editFilterForVector $ids]

    # update pane content
    if {$res!=""} {
        foreach pos $sel {
            set id [lindex [$g(listbox$lb) get $pos] end]
            $g(listbox$lb) delete $pos
            $g(listbox$lb) insert $pos [makeListboxRow $id]
            $g(listbox$lb) selection set $pos
        }
    }
}

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

