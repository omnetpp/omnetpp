#=================================================================
#  DOPLOT.TCL - part of
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

proc adjustfname {fname} {
    global tcl_platform

    if {$tcl_platform(platform) == "windows"} {
        regsub -all -- "/" $fname "\\" fname
    }
    return $fname
}

proc adjustfname_gnuplot {fname} {
    # some gnuplots on Windows want "/", others want "\"
    global tcl_platform config

    if {$tcl_platform(platform) == "windows"} {
        if {$config(gp-slash)} {
            regsub -all -- "\\\\" $fname "/" fname
        } else {
            regsub -all -- "/" $fname "\\" fname
        }
    }
    return $fname
}

proc write_tempfile {fname contents} {

    # this proc only needed on Windows

    if [catch {
       set fout [open $fname w]
       puts $fout $contents
       close $fout
    } err] {
       tk_messageBox -icon warning -type ok -message "Error writing temp file: $err"
       return
    }
}

proc add_tempfile {fname} {
    global g

    if {[lsearch $fname $g(tempfiles)] == -1} {
        lappend g(tempfiles) $fname
    }
}

proc makeEchoScript_windows {text fname} {
    # create a batch file that echos $text into $fname
    set bat $text
    regsub -all -- "%" $bat "%%" bat
    regsub -all -- "<" $bat "^<" bat
    regsub -all -- ">" $bat "^>" bat
    regsub -all -- "\\&" $bat "^&" bat
    regsub -all -- "\\|" $bat "^|" bat
    regsub -all -- "\n" $bat " >>$fname\necho " bat
    set bat "echo $bat"
    set bat "rem *** the following echo commands will only work on NT/Win2K!\ndel $fname\n$bat"
    return $bat
}

proc browsePicFile {w} {

    # helper for savePicture

    set fname [$w get]
    catch {cd [file dirname $fname]}
    set fname [file tail $fname]
    set fname [tk_getSaveFile -defaultextension "" \
              -initialdir [pwd] -initialfile $fname \
              -filetypes {{{All files} {*}}}]

    if {$fname!=""} {
        $w del 0 end
        $w insert 0 $fname
    }
}

proc savePicture {} {

    global gp

    # create dialog with OK and Cancel buttons
    createOkCancelDialog .ize "Save picture"

    set termlist {
        {postscript eps color}
        {latex courier 10}
        {latex roman 12}
        {emtex courier 10}
        {emtex roman 12}
        {gif transparent small size 200, 150}
        {gif transparent medium size 640,480}
        {gif transparent large size 1000,750}
        {png small monochrome}
        {png small gray}
        {png small color}
        {png medium monochrome}
        {png medium gray}
        {png medium color}
    }

    # add entry fields and focus on first one
    frame .ize.f.f1 -relief groove -border 2
    label-combo .ize.f.f1.term "Output type:" $termlist $gp(picterm)
    commentlabel .ize.f.f1.c "The above are only some of the possible Gnuplot outputs. Type 'set term' in Gnuplot for a list of output types, and 'help postscript', 'help latex' or 'help gif', etc for specific options available for different output types."
    commentlabel .ize.f.f1.c2 "On Windows, you may also use the clipboard instead of this dialog -- try the Options|Copy to Clipboard item in the system menu of the main Gnuplot window, then pasting it into Word."

    frame .ize.f.f2 -relief groove -border 2
    label-entry .ize.f.f2.fname "File name:" $gp(picfile)
    button .ize.f.f2.but -text "Browse..." -command {browsePicFile .ize.f.f2.fname.e}

    combo-onchange .ize.f.f1.term.e "adjustFilename .ize.f.f1.term.e .ize.f.f2.fname.e"

    pack .ize.f.f1 .ize.f.f2 -anchor center -expand 1 -fill x -side top
    pack .ize.f.f1.term -anchor center -expand 1 -fill x -side top
    pack .ize.f.f1.c -anchor center -expand 0 -fill x -side top
    pack .ize.f.f1.c2 -anchor center -expand 0 -fill x -side top
    pack .ize.f.f2.fname -anchor n -expand 1 -fill x -side left
    pack .ize.f.f2.but -anchor n -expand 0 -fill none -side right
    focus .ize.f.f2.fname.e

    if {[execOkCancelDialog .ize] == 1} {
        set gp(picterm) [.ize.f.f1.term.e get]
        set gp(picfile) [.ize.f.f2.fname.e get]

        doPlot picture
    }
    destroy .ize
}

proc adjustFilename {termcombo fnameentry} {
    set term [$termcombo get]
    set fname [$fnameentry get]

    set ext ""
    if [regexp "postscript" $term] {
        set ext ".ps"
    } elseif [regexp "latex" $term] {
        set ext ".tex"
    } elseif [regexp "emtex" $term] {
        set ext ".tex"
    } elseif [regexp "gif" $term] {
        set ext ".gif"
    } elseif [regexp "png" $term] {
        set ext ".png"
    }
    if {$ext!=""} {
        regsub {\.[^.]*$} $fname $ext fname
        $fnameentry delete 0 end
        $fnameentry insert 0 $fname
    }
    return 1
}

proc saveScript {} {
    global g vec gp config tcl_platform

    if {$tcl_platform(platform) == "windows"} {
        set ext ".bat"
    } else {
        set ext ".sh"
    }

    set fname $config(scriptfile)
    catch {cd [file dirname $fname]}
    set fname [file tail $fname]
    set fname [tk_getSaveFile -defaultextension $ext \
              -initialdir [pwd] -initialfile $fname \
              -filetypes { {{Unix script} {*.sh}} {{Windows batch file} {*.bat}} {{All files} {*}}
                         }]

    if {$fname != ""} {

        busy "Writing script..."
        if [catch {
           if [string match -nocase "*.bat" $fname]  {
               set fout [open $fname w]
               puts $fout [makeScript windows]
               close $fout
           } else {
               set fout [open $fname w]
               puts $fout [makeScript unix]
               close $fout
               if {$tcl_platform(platform) == "unix"} {
                   exec chmod a+x $fname
               }
           }
        } errmsg] {
           tk_messageBox -icon warning -type ok -message "Error: $errmsg"
        }
        busy

        set config(scriptfile) $fname
    }
}


proc getVectorsToPlot {} {
    global g

    # listbox empty?
    if {[$g(listbox2) index end]==0} {
       return {}
    }

    # if no selection, select current item
    set sel [$g(listbox2) curselection]
    if {$sel == ""} {
         set sel [$g(listbox2) index active]
         $g(listbox2) selection set $sel
    }

    # collect ids of selected vectors
    set idlist {}
    foreach i $sel {
        set line [$g(listbox2) get $i]
        set id [lindex $line end]
        lappend idlist $id
    }
    return $idlist
}


proc makeGnuplotConfigCmd {} {
    global gp

    set gnuplotcmd ""
    if {$gp(title)  != ""} {append gnuplotcmd "set title \"$gp(title)\"\n"}
    if {$gp(xlabel) != ""} {append gnuplotcmd "set xlabel \"$gp(xlabel)\"\n"}
    if {$gp(ylabel) != ""} {append gnuplotcmd "set ylabel \"$gp(ylabel)\"\n"}
    if {$gp(xrange) != ""} {append gnuplotcmd "set xrange \[$gp(xrange)\]\n"}
    if {$gp(yrange) != ""} {append gnuplotcmd "set yrange \[$gp(yrange)\]\n"}
    if {$gp(xtics)  != ""} {append gnuplotcmd "set xtics  $gp(xtics)\n"}
    if {$gp(ytics)  != ""} {append gnuplotcmd "set ytics  $gp(ytics)\n"}
    if {$gp(before) != ""} {append gnuplotcmd "$gp(before)\n"}

    return $gnuplotcmd
}

proc makeGnuplotScript {pipebasefname idlist platform {pic {}}} {
    global vec gp

    # create configuration commands
    set gnuplotcmd [makeGnuplotConfigCmd]
    if {$pic != ""} {
        append gnuplotcmd "set terminal $gp(picterm)\n"
        append gnuplotcmd "set output   '$gp(picfile)'\n"
    }

    # append plot command
    append gnuplotcmd "plot"
    set comma ""
    foreach id $idlist {
        set vecstyle $vec($id,style)
        if {$vecstyle == "default"} {set vecstyle $gp(defaultstyle)}
        append gnuplotcmd "$comma \"$pipebasefname-$id\" using 2:3 \
                   title \"$vec($id,title)\" with $vecstyle"
        set comma ","
    }
    append gnuplotcmd "\n"

    # append post-plot command
    if {$gp(after) != ""} {
        append gnuplotcmd "$gp(after)\n"
    }

    # append pause command if necessary
    if {$pic == ""} {
        if {$platform == "unix"} {
            append gnuplotcmd "pause 1000\n"
        } else {
            append gnuplotcmd "pause -1 \"Press a key\"\n"
        }
    }
    return $gnuplotcmd
}


proc makeGrepCommand {platform id pipefname {log {}}} {

    global vec config

    # grep
    if {$platform == "unix"} {
        if {$vec($id,zipped)} {
            set grepcmd "$config(zcat) \"$vec($id,fname)\" | $config(grep) \"^$vec($id,vecid)\\>\""
        } else {
            set grepcmd "$config(grep) \"^$vec($id,vecid)\\>\" '$vec($id,fname)'"
        }
    } else {
        set vecfname [adjustfname $vec($id,fname)]
        if {$vec($id,zipped)} {
            set grepcmd "$config(zcat) $vecfname | $config(grep) \"^$vec($id,vecid)\\>\""
        } else {
            set grepcmd "$config(grep) \"^$vec($id,vecid)\\>\" $vecfname"
        }
    }

    # filters
    if {$vec($id,filter) != "none"} {
        set filtcmd [filterCommand $platform $id $vec($id,filter) $vec($id,filtpars)]
        append grepcmd " | $filtcmd"
    }

    # redirection
    if {$platform == "unix"} {
        if {$log != ""} {
            append grepcmd " > $pipefname 2>>'$log' &\n"
        } else {
            append grepcmd " > $pipefname &\n"
        }
    } else {
        append grepcmd " > $pipefname\n"
    }
    return $grepcmd
}


proc createPipe_unix {pipe} {
    # create named pipe
    global config

    if [file exists $pipe] {
        if {[file type $pipe] != "fifo"} {
            tk_messageBox -icon warning -type ok -title Warning \
               -message "Cannot create named pipe $pipe, file name already exists!"
        }
    } else {
        if [catch {exec $config(mknod) $pipe p} errmsg] {
            tk_messageBox -icon warning -type ok -title Warning \
               -message "Cannot create named pipe $pipe: $errmsg"
        }
    }
}


proc doPlot {{pic {}}} {
    global tcl_platform config

    set platform $tcl_platform(platform)

    # vectors to plot
    set idlist [getVectorsToPlot]
    if {$idlist == ""} {
        return
    }

    # call platform-specific plotting routine
    if {$tcl_platform(platform) == "unix"} {
        doPlot_unix $idlist $pic
    } elseif {$tcl_platform(platform) == "windows"} {
        doPlot_windows $idlist $pic
    } else {
        tk_messageBox -icon error -type ok -title Error -message "Unsupported platform!"
    }
}

proc doPlot_unix {idlist pic} {
    global config

    # hourglass...
    busy "Running greps and gnuplot..."

    # generate pipe file name
    set pipebasefname [file join $config(tmp) "pipe[pid]"]
    set pipebasefname_gp $pipebasefname
    set log [file join $config(tmp) log[pid]]

    # gnuplot command
    set gnuplotcmd [makeGnuplotScript $pipebasefname_gp $idlist unix $pic]

    # make all necessary pipes
    foreach id $idlist {
        createPipe_unix $pipebasefname-$id
    }

    # launch grep and filter commands
    foreach id $idlist {
        set grepcmd [makeGrepCommand unix $id $pipebasefname-$id $log]
        if [catch {exec $config(sh) << $grepcmd 2>> $log-$id &} errmsg] {
            busy
            tk_messageBox -icon warning -type ok -title Error \
                  -message "Error spawning sh with greps and filters: $errmsg"
            return
        }
    }

    # launch gnuplot
    if [catch {exec $config(gnuplot) << $gnuplotcmd > $log 2>> $log &} errmsg] {
        busy
        tk_messageBox -icon warning -type ok -title Error \
                      -message "Error spawning gnuplot: $errmsg"
        return
    }

    busy
}

proc doPlot_windows {idlist pic} {
    global config

    # hourglass...
    busy "Running greps and gnuplot..."

    # generate temp file name
    set pipebasefname [adjustfname [file join $config(tmp) "tmp"]]
    set pipebasefname_gp [adjustfname_gnuplot $pipebasefname]
    set log [adjustfname [file join $config(tmp) log]]

    # generate gnuplot command
    set gnuplotcmd [makeGnuplotScript $pipebasefname_gp $idlist windows $pic]

    # create command file for gnuplot
    set gpfile [adjustfname [file join $config(tmp) "gnuplot.plt"]]
    add_tempfile $gpfile
    write_tempfile $gpfile $gnuplotcmd

    # write grep, filter and gnuplot commands into a batch file
    set grepcmd ""
    foreach id $idlist {
        append grepcmd [makeGrepCommand windows $id $pipebasefname-$id $log]
    }
    append grepcmd "$config(gnuplot) $gpfile\n"
    foreach id $idlist {
        append grepcmd "del $pipebasefname-$id\n"
    }

    # write temp batch file and launch it
    set bat [adjustfname [file join $config(tmp) "plotvectors.bat"]]
    add_tempfile $bat
    write_tempfile $bat "$grepcmd"
    if [catch {exec $bat} errmsg] {
        busy
        tk_messageBox -icon warning -type ok -title Error \
             -message "Error running greps and filters: $errmsg"
        return
    }
    busy
}


proc makeScript {platform} {

    # vectors to plot
    set idlist [getVectorsToPlot]
    if {$idlist == ""} {
        return
    }

    # call platform-specific script generator code
    if {$platform == "unix"} {
        makeScript_unix $idlist
    } elseif {$platform == "windows"} {
        makeScript_windows $idlist
    } else {
        tk_messageBox -icon error -type ok -title Error -message "Unsupported platform!"
    }
}

proc makeScript_windows {idlist} {
    global vec gp config

    # generate temp file name
    set pipebasefname [adjustfname [file join $config(tmp) "tmp"]]
    set pipebasefname_gp [adjustfname_gnuplot $pipebasefname]
    set log [adjustfname [file join $config(tmp) log]]

    # generate gnuplot command
    append gnuplotcmd "## uncomment this for file output:\n"
    append gnuplotcmd "# set terminal $gp(picterm)\n"
    append gnuplotcmd "# set output   '$gp(picfile)'\n"
    append gnuplotcmd [makeGnuplotScript $pipebasefname_gp $idlist windows ""]

    set gpfile [adjustfname [file join $config(tmp) "gnuplot.plt"]]
    set grepcmd [makeEchoScript_windows $gnuplotcmd $gpfile]

    # write grep, filter and gnuplot commands into a batch file
    foreach id $idlist {
        append grepcmd [makeGrepCommand windows $id $pipebasefname-$id $log]
    }
    append grepcmd "$config(gnuplot) $gpfile\n"
    foreach id $idlist {
        append grepcmd "del $pipebasefname-$id\n"
    }

    return $grepcmd
}

proc makeScript_unix {idlist} {
    global g vec gp config

    set pipebasefname [file join $config(tmp) pipe[pid]]
    set log [file join $config(tmp) log[pid]]

    # gnuplot command
    set gnuplotcmd ""
    append gnuplotcmd "## uncomment this for file output:\n"
    append gnuplotcmd "# set terminal $gp(picterm)\n"
    append gnuplotcmd "# set output   '$gp(picfile)'\n"
    append gnuplotcmd [makeGnuplotScript $pipebasefname $idlist unix ""]

    # pipes
    set pipecmd ""
    foreach id $idlist {
        append pipecmd "rm -f $pipebasefname-$id; mknod $pipebasefname-$id p\n"
    }

    # grep and filter commands
    set grepcmd ""
    foreach id $idlist {
        append grepcmd [makeGrepCommand unix $id $pipebasefname-$id $log]
    }

    # assemble script
    set script "#\n# This file was generated by Plove\n#\n"
    append script "$pipecmd\n"
    append script "$grepcmd\n"
    append script "$config(gnuplot) << END\n$gnuplotcmd\nEND\n"

    return $script
}


proc exitCleanup {} {

    global g

    foreach f $g(tempfiles) {
        catch {file delete $f}
    }
}

