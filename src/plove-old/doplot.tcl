#==========================================================================
#  DOPLOT.TCL -
#            part of OMNeT++
#==========================================================================

#----------------------------------------------------------------#
#  Copyright (C) 1992,99 Andras Varga
#  Technical University of Budapest, Dept. of Telecommunications,
#  Stoczek u.2, H-1111 Budapest, Hungary.
#
#  This file is distributed WITHOUT ANY WARRANTY. See the file
#  `license' for details on this and other legal matters.
#----------------------------------------------------------------#

proc file_join {args} {
    global tcl_platform config

    set f [eval file join $args]

    # Windows: transform "/" to "\" if needed
    if {$tcl_platform(platform) == "windows" && !$config(gp-slash)} {
        regsub -all -- "/" $f "\\" f
    }
    return $f
}

proc write_tempfile {fname contents} {

    # this proc only needed on Windows

    if [catch {
       set fout [open $fname w]
       puts $fout $contents
       close $fout
    } err] {
       tk_messageBox -icon warning -type ok \
              -message "Error writing temp file: $err"
       return
    }
}

proc add_tempfile {fname} {
    global g

    if {[lsearch $fname $g(tempfiles)] == -1} {
        lappend g(tempfiles) $fname
    }
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

    # add entry fields and focus on first one
    frame .ize.f.f1 -relief groove -border 2
    label-entry .ize.f.f1.term "set terminal:" $gp(picterm)
    commentlabel .ize.f.f1.c "Type 'set term' in gnuplot to see options available in your gnuplot."

    frame .ize.f.f2 -relief groove -border 2
    label-entry .ize.f.f2.fname "File name:" $gp(picfile)
    button .ize.f.f2.but -text "Browse..." -command {browsePicFile .ize.f.f2.fname.e}

    pack .ize.f.f1 .ize.f.f2 -anchor center -expand 1 -fill x -side top
    pack .ize.f.f1.term -anchor center -expand 1 -fill x -side top
    pack .ize.f.f1.c -anchor center -expand 0 -fill x -side top
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

proc saveScript {} {
    global g vec gp config tcl_platform

    set fname $config(scriptfile)

    catch {cd [file dirname $fname]}
    set fname [file tail $fname]
    set fname [tk_getSaveFile -defaultextension ".sh" \
              -initialdir [pwd] -initialfile $fname \
              -filetypes { {{Unix script} {*.sh}} {{All files} {*}}
                         }]

    if {$fname != ""} {

        busy "Preparing commands..."
        if [catch {

           set fout [open $fname w]
           puts $fout [makeScript "unix"]
           close $fout

           if {$tcl_platform(platform) == "unix"} {
               exec chmod a+x $fname
           }
        } errmsg] {
           tk_messageBox -icon warning -type ok -message "Error: $errmsg"
        }
        busy

        set config(scriptfile) $fname
    }
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

proc makeGrepCmd {platform id pipei {log {}}} {

    global vec config

    if {$platform == "unix"} {
        if {$vec($id,zipped)} {
            set grepcmd "$config(zcat) \"$vec($id,fname)\" | $config(grep) \"^$vec($id,vecid)\\>\""
        } else {
            set grepcmd "$config(grep) \"^$vec($id,vecid)\\>\" '$vec($id,fname)'"
        }
    } else {
        if {$vec($id,zipped)} {
            set grepcmd "$config(zcat) $vec($id,fname) | $config(grep) \"^$vec($id,vecid) \""
        } else {
            set grepcmd "$config(grep) \"^$vec($id,vecid) \" $vec($id,fname)"
        }
    }

    if {$vec($id,filter) != "none"} {
        set filtcmd [filterCommand $platform $id $vec($id,filter) $vec($id,filtpars)]
        append grepcmd " | $filtcmd"
    }

    # redirection
    if {$platform == "unix"} {
        if {$log != ""} {
            append grepcmd " > $pipei 2>>'$log' &\n"
        } else {
            append grepcmd " > $pipei &\n"
        }
    } else {
        append grepcmd " > $pipei\n"
    }
    return $grepcmd
}

proc makeGnuplotCmd {id pipei} {

    global vec

    set gnuplotcmd "\"$pipei\" using 2:3 \
                   title \"$vec($id,title)\" \
                   with $vec($id,style)"

    return $gnuplotcmd
}

proc makeScript {platform} {
    global g vec gp config

    if {[$g(listbox2) index end]==0} {
       busy
       return
    }

    set pipe [file_join $config(tmp) pipe]

    set grepcmd ""
    set gnuplotcmd [makeGnuplotConfigCmd]
    append gnuplotcmd "# uncomment this for file output:\n"
    append gnuplotcmd "# set terminal $gp(picterm)\n"
    append gnuplotcmd "# set output   '$gp(picfile)'\n"
    append gnuplotcmd "plot "

    set comma ""
    set sel [$g(listbox2) curselection]
    if {$sel == ""} {
         set sel [$g(listbox2) index active]
         $g(listbox2) selection set $sel
    }

    foreach i $sel {
        set line [$g(listbox2) get $i]
        set id [lindex $line end]

        if {$platform == "unix"} {
            append pipecmd "rm -f $pipe.$i; mknod $pipe.$i p\n"
        }
        append grepcmd [makeGrepCmd $platform $id $pipe.$i]
        append gnuplotcmd "$comma [makeGnuplotCmd $id $pipe.$i]"
        set comma ","
    }
    append gnuplotcmd "\n"
    if {$gp(after) != ""} {append gnuplotcmd "$gp(after)\n"}

    if {$platform == "unix"} {
        append gnuplotcmd "pause 1000\n"
    } else {
        append gnuplotcmd "pause -1 \"Press a key\"\n"
    }

    set script "#\n# This file was generated by Plove\n#\n"
    append script "$pipecmd\n$grepcmd\n"
    append script "$config(gnuplot) << END\n$gnuplotcmd\nEND\n"

    return $script
}

proc createPipe {pipe} {

    # create named pipe

    global tcl_platform config

    if {$tcl_platform(platform) == "unix"} {
        if [file exists $pipe] {
            if {[file type $pipe] != "fifo"} {
                tk_messageBox -icon warning -type ok -title Warning \
                   -message "Cannot create named pipe $pipe, already exists as file!"
            }
        } else {
            if [catch {exec $config(mknod) $pipe p} errmsg] {
                tk_messageBox -icon warning -type ok -title Warning \
                   -message "Cannot create named pipe $pipe: $errmsg"
            }
        }
    } else {
        # on Windows, just use plain files...

        if [catch {open $pipe w} fout] {
           tk_messageBox -icon warning -type ok \
                         -message "Error opening temporary file $pipe: $fout"
           return
        } else {
           close $fout
        }
    }
}

proc doPlot {{pic {}}} {
    global g vec gp config tcl_platform

    if {[$g(listbox2) index end]==0} {
       busy
       return
    }

    busy "Preparing commands..."

    set platform $tcl_platform(platform)

    if {$platform == "unix"} {set pid [pid]} else {set pid ""}
    set pipe [file_join $config(tmp) pipe$pid]
    set log  [file_join $config(tmp) log$pid]

    add_tempfile $log

    set grepcmd ""
    set gnuplotcmd [makeGnuplotConfigCmd]
    if {$pic != ""} {
        append gnuplotcmd "set terminal $gp(picterm)\n"
        append gnuplotcmd "set output   '$gp(picfile)'\n"
    }
    append gnuplotcmd "plot "

    set comma ""
    set sel [$g(listbox2) curselection]
    if {$sel == ""} {
         set sel [$g(listbox2) index active]
         $g(listbox2) selection set $sel
    }

    foreach i $sel {
        set line [$g(listbox2) get $i]
        set id [lindex $line end]

        add_tempfile $pipe.$i
        createPipe $pipe.$i

        append grepcmd [makeGrepCmd $platform $id $pipe.$i $log]
        append gnuplotcmd "$comma [makeGnuplotCmd $id $pipe.$i]"
        set comma ","
    }
    append gnuplotcmd "\n"
    if {$gp(after) != ""} {append gnuplotcmd "$gp(after)\n"}

    if {$platform == "unix"} {
        set pausecmd "pause 1000\n"
    } else {
        set pausecmd "pause -1 \"Press a key\"\n"
    }

    if {$pic == ""} {append gnuplotcmd $pausecmd}

    runPrograms $grepcmd $gnuplotcmd $log

    busy
}

proc runPrograms {grepcmd gnuplotcmd log} {

    global config tcl_platform

    # must be a better way than this... maybe expect?

    busy "Running greps and gnuplot..."

    # puts $grepcmd
    # puts $gnuplotcmd

    catch {file delete $log}

    if {$tcl_platform(platform) == "unix"} {
        if [catch {exec $config(sh) << $grepcmd 2>> $log &} errmsg] {
            tk_messageBox -icon warning -type ok -title Error \
                  -message "Error spawning sh with greps and filters: $errmsg"
            return
        }
        if [catch {exec $config(gnuplot) << $gnuplotcmd > $log 2>> $log &} errmsg] {
            tk_messageBox -icon warning -type ok -title Error \
                          -message "Error spawning gnuplot: $errmsg"
            return
        }
    } else {
        set bat [file_join $config(tmp) "greps.bat"]
        add_tempfile $bat
        write_tempfile $bat $grepcmd
        if [catch {exec $bat} errmsg] {
            tk_messageBox -icon warning -type ok -title Error \
                  -message "Error running greps and filters: $errmsg"
            return
        }

        set gpfile [file_join $config(tmp) "gnuplot.tmp"]
        add_tempfile $gpfile
        write_tempfile $gpfile $gnuplotcmd
        if [catch {exec $config(gnuplot) $gpfile} errmsg] {
            tk_messageBox -icon warning -type ok -title Error \
                          -message "Error spawning gnuplot: $errmsg"
            return
        }
    }
}

proc exitCleanup {} {

    global g

    foreach f $g(tempfiles) {
        catch {file delete $f}
    }
}
