#=================================================================
#  FILTER.TCL - part of
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

proc initFilters {} {

    global filt

    foreach i {
        {none      {}          {changes nothing}          prog {}}
        {add       {const +}   {add constant}             expr {x+$(constant)}}
        {mult      {const *}   {multiply by constant}     expr {x*$(constant)}}
        {mean      {mean}      {mean on [0,t]}            awk  {see later!}}
        {trange    {t-zoomed}  {ignore samples outside (T1,T2)}  awk  {see later!}}
        {xlimit    {x-limited} {limit to (X1,X2)}         awk  {see later!}}
        {truncN    {truncated} {delete first N samples}   awk  {see later!}}
        {truncT    {truncated} {delete samples before T0} awk  {see later!}}
        {winavgN   {smoothed}  {replace each N samples with their mean} awk  {see later!}}
        {winavgT   {smoothed}  {in each T wide window, replace samples with their mean} awk  {see later!}}
        {histogram {histogram of} {equal cell size histogram} awk {see later!}}
    } {
        set name [lindex $i 0]
        lappend filt(names) $name
        set filt($name,prefix) [lindex $i 1]
        set filt($name,descr)  [lindex $i 2]
        set filt($name,type)   [lindex $i 3]
        set filt($name,expr)   [lindex $i 4]
    }

    set filt(mean,expr)   {{ sum+=x;  x=sum/++n;  print }}

    set filt(trange,expr) {{ if (t>=$(T1) && t<=$(T2))  print }}

    set filt(xlimit,expr) \
{{
  if (x<=$(X1)) x=$(X1)
  if (x>=$(X2)) x=$(X2)
  print
}}

    set filt(truncN,expr) {{ if (++n>=$(N)) print }}

    set filt(truncT,expr) {{ if (t>=$(T0)) print }}

    set filt(winavgN,expr) \
{{
  if (t0=="") t0=t
  sum+=x
  if (++i==$(N)) {print $1, (t0+t)/2, sum/$(N); sum=i=0; t0=t;}
}}

    set filt(winavgT,expr) \
{{
  if (t>=t1) {
    if (n>0) print $1, t1-$(T)/2, sum/n;
    sum=n=0; t1+=$(T);
  }
  sum+=x; ++n
}}

    set filt(histogram,expr) \
{# histogram $(lower_limit) $(upper_limit) $(cell_size)
{
  if (x>=$(lower_limit) && x<$(upper_limit))
    cell[int((x-$(lower_limit))/$(cell_size))]++
}
END {
  for (d=$(lower_limit);d<$(upper_limit); d+=$(cell_size))
    print 0, d, cell[int((d-$(lower_limit))/$(cell_size))]+0
}}

}


proc word_regsub {what str to} {

    # just simply substitutes '$to' for '$what' but only whole words
    # yes, it's as simple as this...

    regsub -all -- {\(} $what {\(} what
    regsub -all -- {\)} $what {\)} what
    regsub -all -- {\$} $what {\$} what

    set na {[^_A-Za-z0-9]+}
    regsub -all -- "^$what\$" $str $to str
    regsub -all -- "^$what\($na)" $str "$to\\1" str
    regsub -all -- "($na)$what\$" $str "\\1$to" str
    regsub -all -- "($na)$what\($na)" $str "\\1$to\\2" str

    # must be done twice due to a silly bug in regexp
    # (forgets to replace every other occurrence)
    regsub -all -- "^$what\$" $str $to str
    regsub -all -- "^$what\($na)" $str "$to\\1" str
    regsub -all -- "($na)$what\$" $str "\\1$to" str
    regsub -all -- "($na)$what\($na)" $str "\\1$to\\2" str

    return $str
}

proc filterCommand {platform id name varlist} {

    # varlist is a list of {name value} pairs

    global filt config

    set filtexpr $filt($name,expr)
    set filtexpr [word_regsub {t}  $filtexpr  {$2}]
    set filtexpr [word_regsub {x}  $filtexpr  {$3}]

    foreach i $varlist {
       set var [lindex $i 0]
       set val [lindex $i 1]
       set filtexpr [word_regsub "\$\($var\)"  $filtexpr $val]
    }

    if {$filt($name,type) == "expr"} {
        set filtcmd "$config(awk) 'BEGIN \{OFMT=\"$config(ofmt)\"\}\n\{\$3 = $filtexpr; print\}'"
    } elseif {$filt($name,type) == "awk"} {
        set filtexpr "BEGIN \{OFMT=\"$config(ofmt)\"\}\n$filtexpr"
        if {$platform == "unix"} {
            set filtcmd "$config(awk) '$filtexpr'"
        } else {
            set awkprog [adjustfname [file join $config(tmp) "filt$id.awk"]]
            add_tempfile $awkprog
            write_tempfile $awkprog $filtexpr
            set filtcmd "$config(awk) -f $awkprog"
        }
    } elseif {$filt($name,type) == "prog"} {
        set filtcmd "$filtexpr"
    } else {
        error "Bad filter type"
    }

    return $filtcmd
}

proc getFilterParList {name} {

    # collects parameter names "$(parameter)" from filter

    global filt

    set expr $filt($name,expr)
    set pars ""

    while {[regexp -- {\$\(([^)]*)\)} $expr match par]} {
        regsub  -all -- "\\$\\($par\\)" $expr {@} expr
        if {$par != "id"} {lappend pars $par}
    }
    return $pars
}

proc filterSelected {} {

    # callback for filterParDialog

    global filt global tmp

    set name [.ize.f.f1.filt.e cget -value]

    if {[lsearch -exact $filt(names) $name] == -1} {
        tk_messageBox -title Error -icon warning -message "No filter named $name." -type ok
        return
    }

    # adjust filter prefix
    set tit [.ize.f.f0.title.e get]
    set tit [cut_prefix $tit $tmp(prefix)]

    if {$filt($name,prefix) == ""} {
        set tmp(prefix) ""
    } else {
        set tmp(prefix) "$filt($name,prefix) "
    }
    set tit "$tmp(prefix)$tit"
    .ize.f.f0.title.e delete 0 end
    .ize.f.f0.title.e insert 0 $tit

    .ize.f.f1.descr.e config -text $filt($name,descr)

    # refresh frame with parameters
    destroy .ize.f.f1.ff
    frame .ize.f.f1.ff -border 0 -relief groove

    set parlist [getFilterParList $name]

    if {$parlist != ""} {

        label .ize.f.f1.ff.lab -text "Parameters:"
        pack .ize.f.f1.ff -anchor center -expand 0 -fill x -side top
        pack .ize.f.f1.ff.lab -anchor center -expand 0 -fill none -side top -anchor w
        set i 0
        foreach par $parlist {
            set val "0"
            foreach tmp_par $tmp(pars) {
                if {[lindex $tmp_par 0] == $par} {
                    set val [lindex $tmp_par 1]
                }
            }
            label-entry .ize.f.f1.ff.p$i $par $val
            pack .ize.f.f1.ff.p$i -anchor center -expand 0 -fill x -side top
            incr i
        }
    }
}

proc same {ids field default} {
    global vec

    set f $vec([lindex $ids 0],$field)
    foreach id $ids {
        if {[string compare $f $vec($id,$field)]!=0} {
            return $default
        }
    }
    return $f
}

proc filterParDialog {ids} {

    global filt vec gp

    if {[llength $ids]!=1} {
        set vectitle    {(several vectors)}
        set vecprefix   [same $ids filtpfx {}]
        set vecstyle    [same $ids style {-}]
        set vecfilt     [same $ids filter {-}]
        set vecfiltpars [same $ids filtpars {}]
    } else {
        set vectitle    $vec($ids,title)
        set vecprefix   $vec($ids,filtpfx)
        set vecstyle    $vec($ids,style)
        set vecfilt     $vec($ids,filter)
        set vecfiltpars $vec($ids,filtpars)
    }

    set plotstyles {default lines points linespoints impulses dots steps boxes}

    global tmp  ; #for use by filterSelected
    set tmp(pars)   $vecfiltpars
    set tmp(prefix) $vecprefix
    set tmp(setdefstyle) 0

    # create dialog with OK and Cancel buttons
    createOkCancelDialog .ize "Vector plotting options"

    label .ize.f.lbl -text "See also Options|Gnuplot options for global settings." -anchor w
    pack .ize.f.lbl -anchor center -expand 0 -fill x -side top -pady 3

    # add entry fields and focus on first one
    frame .ize.f.f0 -relief groove -border 2
    pack .ize.f.f0 -expand 1 -fill both -side top -pady 3

    label-entry .ize.f.f0.title "Vector title:" $vectitle
    label-combo .ize.f.f0.style "Style:" $plotstyles $vecstyle
    label-check .ize.f.f0.setdefstyle "" "set default style (current is $gp(defaultstyle))" tmp(setdefstyle)
    pack .ize.f.f0.title -anchor center -expand 0 -fill x -side top
    pack .ize.f.f0.style -anchor center -expand 0 -fill x -side top
    pack .ize.f.f0.setdefstyle -anchor e -expand 0 -fill x -side top

    frame .ize.f.f1 -relief groove -border 2
    pack .ize.f.f1 -expand 1 -fill both -side top -pady 3

    label-combo .ize.f.f1.filt  "Filter:" $filt(names) $vecfilt
    label-sunkenlabel .ize.f.f1.descr "Filter descr:"
    frame .ize.f.f1.ff

    button .ize.f.f1.filtcfg -text "  Manage filters...  " \
       -command "editFilterConfig; comboconfig .ize.f.f1.filt.e \$filt(names) \"filterSelected\""
    pack .ize.f.f1.filt -anchor center -expand 0 -fill x -side top
    pack .ize.f.f1.descr -anchor center -expand 0 -fill x -side top
    pack .ize.f.f1.filtcfg -anchor e -expand 0 -fill none -side top -padx 2 -pady 2

    .ize.f.f0.title.e config -width 40
    .ize.f.f1.descr.e config -width 40
    if {$vecfilt!="-"} filterSelected

    focus .ize.f.f0.title.e

    .ize.f.f1.filt.e configure -command "filterSelected ;#"

    # exec the dialog, extract its contents if OK was pressed, then delete dialog
    if {[execOkCancelDialog .ize] == 1} {
        set vectitle [.ize.f.f0.title.e get]
        set titbase [cut_prefix $vectitle $tmp(prefix)]
        set titbase [regexp_to_stringmatch $titbase]
        if {![regsub -- $titbase $vectitle "" vecprefix]} {set vecprefix ""}

        set vecstyle [.ize.f.f0.style.e cget -value]
        if {$vecstyle=="-"} {
           set vecstyle ""
        }
        if {$vecstyle!="" && $vecstyle!="default" && $tmp(setdefstyle)} {
           set gp(defaultstyle) $vecstyle
        }

        set vecfilt  [.ize.f.f1.filt.e cget -value]
        if {$vecfilt=="-"} {
           set vecfilt ""
           set vecfiltpars ""
        } else {
           set vecfiltpars ""
           foreach w [winfo children .ize.f.f1.ff] {
               if {$w != ".ize.f.f1.ff.lab"} {
                  set pname [$w.l cget -text]
                  set pval  [$w.e get]
                  lappend vecfiltpars "$pname $pval"
               }
           }
        }
        destroy .ize

        return [list $vectitle $vecstyle $vecprefix $vecfilt $vecfiltpars]
    }

    destroy .ize
    return ""
}

