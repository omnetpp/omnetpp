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

proc editGeneralOptions {} {

    global config

    # create dialog with OK and Cancel buttons
    createOkCancelDialog .ize "General configuration"

    # add entry fields and focus on first one
    frame .ize.f.f1 -relief groove -border 2
    label-entry .ize.f.f1.titlefmt "Default title format:" $config(titlefmt)
    .ize.f.f1.titlefmt.e config -width 30
    commentlabel .ize.f.f1.c "Setting doesn't affect already loaded vectors."
    commentlabel .ize.f.f1.c2 "Following macros can be used:\n\$NAME,\$MODULE,\$ID,\$MULT,\$FILENAME,\$FILEPATH"
    label-entry .ize.f.f1.ofmt "Awk's OFMT:" $config(ofmt)

    pack .ize.f.f1 -expand 0 -fill x -side top
    pack .ize.f.f1.titlefmt -anchor center -expand 0 -fill x -side top
    pack .ize.f.f1.c -anchor center -expand 0 -fill x -side top
    pack .ize.f.f1.c2 -anchor center -expand 0 -fill x -side top
    pack .ize.f.f1.ofmt -anchor center -expand 0 -fill x -side top

    global tmp
    set tmp(type) $config(rightarrow)

    frame .ize.f.f2 -relief groove -border 2
    label .ize.f.f2.l -text "Double-click in left pane"
    radiobutton .ize.f.f2.r1 -text "copies vectors to right pane" -value "copy" -variable tmp(type)
    radiobutton .ize.f.f2.r2 -text "moves vectors to right pane" -value "move" -variable tmp(type)

    pack .ize.f.f2.l -anchor w -expand 0 -fill none -side top
    pack .ize.f.f2.r1 -anchor w -expand 0 -fill none -side top
    pack .ize.f.f2.r2 -anchor w -expand 0 -fill none -side top
    pack .ize.f.f2 -expand 0 -fill x -side top

    # exec the dialog, extract its contents if OK was pressed, then delete dialog
    if {[execOkCancelDialog .ize] == 1} {
        set config(titlefmt) [.ize.f.f1.titlefmt.e get]
        set config(ofmt) [.ize.f.f1.ofmt.e get]
        set config(rightarrow) $tmp(type)
    }
    destroy .ize
}

proc editGnuplotOptions {} {

    global gp

    # create dialog with OK and Cancel buttons
    createOkCancelDialog .ize "Gnuplot configuration"

    label .ize.f.f0 -text "See also Edit|Vector plotting options for additional, per-vector settings." -anchor w

    # add entry fields and focus on first one
    frame .ize.f.f1 -relief groove -border 2
    label-entry .ize.f.f1.title "Title:" $gp(title)

    set plotstyles {lines points linespoints impulses dots steps boxes}

    frame .ize.f.f2 -relief groove -border 2
    label-combo .ize.f.f2.style "Default style:" $plotstyles $gp(defaultstyle)
    label-entry .ize.f.f2.xlabel "X label:" $gp(xlabel)
    label-entry .ize.f.f2.ylabel "Y label:" $gp(ylabel)

    frame .ize.f.f3 -relief groove -border 2
    label-entry .ize.f.f3.xrange "X range (lo:hi):" $gp(xrange)
    label-entry .ize.f.f3.yrange "Y range (lo:hi):" $gp(yrange)

    frame .ize.f.f4 -relief groove -border 2
    label-entry .ize.f.f4.xtics  "X tics (start,inc,end),:" $gp(xtics)
    label-entry .ize.f.f4.ytics  "Y tics (start,inc,end):" $gp(ytics)

    frame .ize.f.f5 -relief groove -border 2
    label .ize.f.f5.lab -text "Additional Gnuplot commands:"
    label-text  .ize.f.f5.bef "Before 'plot...':" 4 $gp(before)
    label-text  .ize.f.f5.aft "After 'plot...':" 2 $gp(after)

    pack .ize.f.f0 .ize.f.f1 .ize.f.f2 .ize.f.f3 .ize.f.f4 -expand 0 -fill x -side top -pady 3
    pack .ize.f.f5 -expand 1 -fill both -side top
    pack .ize.f.f1.title  -anchor center -expand 0 -fill x -side top
    pack .ize.f.f2.style  -anchor center -expand 0 -fill x -side top
    pack .ize.f.f2.xlabel -anchor center -expand 0 -fill x -side top
    pack .ize.f.f2.ylabel -anchor center -expand 0 -fill x -side top
    pack .ize.f.f3.xrange -anchor center -expand 0 -fill x -side top
    pack .ize.f.f3.yrange -anchor center -expand 0 -fill x -side top
    pack .ize.f.f4.xtics  -anchor center -expand 0 -fill x -side top
    pack .ize.f.f4.ytics  -anchor center -expand 0 -fill x -side top
    pack .ize.f.f5.lab    -anchor w      -expand 0 -fill none -side top
    pack .ize.f.f5.bef    -anchor center -expand 1 -fill both -side top
    pack .ize.f.f5.aft    -anchor center -expand 1 -fill both -side top
    focus .ize.f.f1.title.e

    # exec the dialog, extract its contents if OK was pressed, then delete dialog
    if {[execOkCancelDialog .ize] == 1} {
        set gp(title)   [.ize.f.f1.title.e get]
        set gp(defaultstyle) [.ize.f.f2.style.e cget -value]
        set gp(xlabel)  [.ize.f.f2.xlabel.e get]
        set gp(ylabel)  [.ize.f.f2.ylabel.e get]
        set gp(xrange)  [.ize.f.f3.xrange.e get]
        set gp(yrange)  [.ize.f.f3.yrange.e get]
        set gp(xtics)   [.ize.f.f4.xtics.e get]
        set gp(ytics)   [.ize.f.f4.ytics.e get]
        set gp(before)  [string trim [.ize.f.f5.bef.t get 1.0 end] {\n}]
        set gp(after)   [string trim [.ize.f.f5.aft.t get 1.0 end] {\n}]
    }
    destroy .ize
}

proc editExtProgs {} {
    global config tcl_platform tmp

    # create dialog with OK and Cancel buttons
    createOkCancelDialog .ize "Configuration"

    # add entry fields and focus on first one
    frame .ize.f.f1
    label-entry .ize.f.f1.grep    "grep:" $config(grep)
    label-entry .ize.f.f1.zcat    "zcat:" $config(zcat)
    label-entry .ize.f.f1.gnuplot "gnuplot:" $config(gnuplot)
    if {$tcl_platform(platform) == "windows"} {
        catch {unset tmp}
        set tmp(gp-slash) $config(gp-slash)
        checkbutton .ize.f.f1.slash -variable tmp(gp-slash) \
            -text "my gnuplot prefers filenames with fwd slashes"
    }
    label-entry .ize.f.f1.awk     "awk:" $config(awk)
    label-entry .ize.f.f1.mknod   "mknod:" $config(mknod)
    label-entry .ize.f.f1.sh      "sh:" $config(sh)
    label-entry .ize.f.f1.tmp     "tmp:" $config(tmp)

    pack .ize.f.f1         -expand 0 -fill x -side top
    pack .ize.f.f1.grep    -expand 0 -fill x -side top
    pack .ize.f.f1.zcat    -expand 0 -fill x -side top
    pack .ize.f.f1.gnuplot -expand 0 -fill x -side top
    if {$tcl_platform(platform) == "windows"} {
        pack .ize.f.f1.slash   -expand 0 -fill x -side top
    }
    pack .ize.f.f1.awk     -expand 0 -fill x -side top
    pack .ize.f.f1.mknod   -expand 0 -fill x -side top
    pack .ize.f.f1.sh      -expand 0 -fill x -side top
    pack .ize.f.f1.tmp     -expand 0 -fill x -side top

    .ize.f.f1.awk.e config -width 40

    focus .ize.f.f1.grep.e

    # exec the dialog, extract its contents if OK was pressed, then delete dialog
    if {[execOkCancelDialog .ize] == 1} {
        set config(grep)    [.ize.f.f1.grep.e get]
        set config(zcat)    [.ize.f.f1.zcat.e get]
        set config(gnuplot) [.ize.f.f1.gnuplot.e get]
        if {$tcl_platform(platform) == "windows"} {
            set config(gp-slash) $tmp(gp-slash)
        }
        set config(awk)     [.ize.f.f1.awk.e get]
        set config(mknod)   [.ize.f.f1.mknod.e get]
        set config(sh)      [.ize.f.f1.sh.e get]
        set config(tmp)     [.ize.f.f1.tmp.e get]
    }
    destroy .ize
}

proc saveConfig {{fname {}}} {
   global g gp filt config

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

       if [catch {open $fname w} fout] {
          tk_messageBox -icon warning -type ok -message "Error: $fout"
          return
       }

       busy "Saving config file $fname..."

       puts $fout "\n# general config"
       foreach i [lsort [array names config]] {
          puts $fout "set config($i) \{$config($i)\}"
       }

       puts $fout "\n# gnuplot config"
       foreach i [lsort [array names gp]] {
          puts $fout "set gp($i) \{$gp($i)\}"
       }

       puts $fout "\n# filters"
       foreach i [lsort [array names filt]] {
          puts $fout "set filt($i) \{$filt($i)\}"
       }

       close $fout

       busy

       set config(configfile) $fname
    }
}

proc loadConfig {{fname {}}} {
   global g gp filt config

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
          return
       }

       busy

       # if it has no '$' sign (old format), overwrite it
       if ![regexp {\$} $config(titlefmt)] {
          set config(titlefmt)   "\$NAME in \$MODULE (\$FILENAME)"
       }

       set config(configfile) $fname
       set config(vectorfile) ""  ;# this we won't preserve
    }

}

