#=================================================================
#  BLTGRAPH.TCL - part of
#
#                     OMNeT++/OMNEST
#            Discrete System Simulation in C++
#
#=================================================================

#----------------------------------------------------------------#
#  Copyright (C) 2004-2005 Andras Varga
#
#  This file is distributed WITHOUT ANY WARRANTY. See the file
#  `license' for details on this and other legal matters.
#----------------------------------------------------------------#


set graphnumber 0

proc createBltGraph {graphtype {graphtitle ""}} {

    global help_tips icons
    global graphnumber

    if {$graphtype!="graph" && $graphtype!="barchart"} {error "invalid graphtype: must be graph of barchart"}

    # create container window if doesn't exist yet
    set w .bltwin
    if ![winfo exist $w] {
        toplevel $w
        wm deiconify $w
        wm title $w "Graphs"
        wm protocol $w WM_DELETE_WINDOW {bltGraph_CloseWindow}
        wm geometry $w "640x540"

        # toolbar
        set toolbar $w.toolbar
        frame $toolbar -relief raised -borderwidth 1
        foreach i {
          {sep0    -separator}
          {copy    -image $icons(copy)     -command bltGraph_Copy}
          {savepic -image $icons(savepic)  -command bltGraph_SavePicture}
          {saveps  -image $icons(saveps)   -command bltGraph_SavePostscript}
          {sep1    -separator}
          {zoomout -image $icons(find)     -command bltGraph_ZoomOut}
          {sep2    -separator}
          {config  -image $icons(config)   -command bltGraph_Properties}
        } {
          set b [eval iconbutton $toolbar.$i]
          pack $b -anchor n -expand 0 -fill none -side left -padx 0 -pady 2
        }

        # close button
        set b [button $toolbar.close -image $icons(close) -command bltGraph_Close -relief flat]
        pack $b -anchor n -expand 0 -fill none -side right -padx 0 -pady 2

        set help_tips($toolbar.copy) {Copy to clipboard}
        set help_tips($toolbar.savepic) {Save picture...}
        set help_tips($toolbar.saveps) {Save Postscript...}
        set help_tips($toolbar.zoomout) {Zoom out}
        set help_tips($toolbar.config) {Properties...}

        set help_tips($toolbar.close)   {Close}

        pack $toolbar -side top -fill x

        # tabnotebook
        blt::tabnotebook $w.nb -tearoff no -relief flat
        pack $w.nb -expand 1 -fill both

        bind $w <1>      {.popup unpost}
        bind $w <Escape> {.popup unpost}
        bind $w.nb <3>   {%W select active; .popup post %X %Y}
    }

    # create page for graph
    incr graphnumber
    set graphname $graphtitle
    if {$graphname==""} {set graphname "Chart $graphnumber"}
    set tabId [$w.nb insert end -text $graphname]
    set page $w.nb.page$tabId
    frame $page
    $w.nb tab configure $tabId -window $page -fill both
    $w.nb select $tabId

    set graph $page.g

    # create graph or barchart
    if {$graphtype=="graph"} {
        blt::graph $graph
        $graph pen configure "activeLine" -color navy -linewidth 1 -symbol none
    } elseif {$graphtype=="barchart"} {
        blt::barchart $graph
        $graph configure -barmode aligned
    }
    $graph configure -title $graphtitle

    # optimize default settings for pasting graph into documents
    $graph configure -plotborderwidth 1
    $graph configure -plotrelief solid
    $graph configure -plotpadx 8
    $graph configure -plotpady 8
    $graph crosshairs configure -dashes {1 1}
    $graph legend configure -position right
    $graph legend configure -anchor ne
    $graph legend configure -relief solid
    $graph legend configure -borderwidth 1
    $graph legend configure -font [$graph axis cget x -tickfont]

    pack $graph -expand 1 -fill both

    Blt_ActiveLegend $graph
    blt::ZoomStack $graph ButtonPress-1 ButtonPress-2
    blt::Crosshairs $graph
    #blt::ClosestPoint $graph
    bltGraph_ShowCoordinates $graph

    bind $graph <3>  {.popup post %X %Y}
    return $graph
}

proc createBltGraphPopup {} {
    menu .popup -tearoff 0
    foreach i {
      {command -label {Zoom out} -underline 0 -command bltGraph_ZoomOut}
      {separator}
      {command -label {Title...} -underline 0    -command {bltGraph_Properties titles} }
      {command -label {Axes...} -underline 0      -command {bltGraph_Properties axes} }
      {command -label {Lines...} -underline 0     -command {bltGraph_Properties lines} }
      {command -label {Bars...} -underline 0     -command {bltGraph_Properties bars} }
      {command -label {Legend...} -underline 1    -command {bltGraph_Properties legend} }
      {separator}
      {command -label {Copy to clipboard} -underline 0  -command bltGraph_Copy}
      {command -label {Save picture...} -underline 0    -command bltGraph_SavePicture}
      {command -label {Save Postscript...} -underline 0 -command bltGraph_SavePostscript}
      {separator}
      {command -label {Close} -underline 2 -command bltGraph_Close}
    } {
       eval .popup add $i
    }
}

proc bltGraph_Close {} {
    # delete tab
    set w .bltwin
    set f [$w.nb tab cget select -window]

    # delete vectors used by the graph
    set graph $f.g
    foreach e [$graph element names] {
       # note "catch": not sure -x is was given as BLT vector name -- Tcl list also possible
       catch {[$graph element cget $e -x] delete}
       catch {[$graph element cget $e -y] delete}
    }

    # delete tab page (and graph)
    $w.nb delete select
    destroy $f

    # with the last tab, close whole window
    if {[$w.nb size]==0} {
        destroy $w
    }
}

proc bltGraph_CloseWindow {} {
    # delete all graphs (last graph will remove window)
    set w .bltwin
    while {[winfo exist $w]} {
        bltGraph_Close
    }
}


proc bltGraph_SetWhiteBg {graph} {
    global tmp

    set tmp(graph-orig-bgcolor) [$graph cget -background]
    set tmp(graph-orig-plotbgcolor) [$graph cget -plotbackground]
    $graph config -background white
    $graph config -plotbackground white
}

proc bltGraph_RestoreBg {graph} {
    global tmp

    $graph config -background $tmp(graph-orig-bgcolor)
    $graph config -plotbackground $tmp(graph-orig-plotbgcolor)
}

proc bltGraph_Copy {} {
    set w .bltwin
    set graph [$w.nb tab cget select -window].g

    bltGraph_SetWhiteBg $graph
    if [catch {$graph snap -format emf CLIPBOARD}] {
        tk_messageBox -parent $w -icon error -type ok -title Error \
             -message "Sorry, copying the graph to the clipboard only works on Windows."
    }
    bltGraph_RestoreBg $graph
}

proc bltGraph_ZoomOut {} {
    set w .bltwin
    set graph [$w.nb tab cget select -window].g
    blt::ResetZoom $graph
}

proc bltGraph_SavePicture {} {
    set w .bltwin
    set graph [$w.nb tab cget select -window].g

    set fname [tk_getSaveFile -defaultextension ".gif" -parent .bltwin \
               -initialdir [pwd] -initialfile "graph.gif" \
               -filetypes {{{GIF files} {*.gif}} {{All files} {*}}}]


    bltGraph_SetWhiteBg $graph
    if {$fname!=""} {
       if [catch {
           set img [image create photo]
           $graph snap $img
           # or this: blt::winop snap $graph $img
           $img write "graph.gif" -format "GIF"
       } err] {
           bltGraph_RestoreBg $graph
           tk_messageBox -icon error -type ok  -parent .bltwin -message "Error: $err"
       }
    }
    bltGraph_RestoreBg $graph
}

proc bltGraph_SavePostscript {} {
    set w .bltwin
    set graph [$w.nb tab cget select -window].g

    set fname [tk_getSaveFile -defaultextension ".ps" -parent .bltwin \
               -initialdir [pwd] -initialfile "graph.ps" \
               -filetypes {{{Postscript files} {*.ps}} {{All files} {*}}}]

    bltGraph_SetWhiteBg $graph
    if {$fname!=""} {
        if [catch {
            $graph postscript output $fname
        } err] {
            tk_messageBox -icon error -type ok  -parent .bltwin -message "Error: $err"
            return
        }
    }
    bltGraph_RestoreBg $graph
}

proc bltGraph_ShowCoordinates {graph} {
    bind $graph <Motion>  {bltGraph_ShowLabel %W %x %y}
}

proc bltGraph_ShowLabel { graph x y } {
    global fonts
    set markerName "marker"
    catch { $graph marker delete $markerName }
    if [$graph element closest $x $y info -interpolate yes] {
        # $info(x) and $info(y) are buggy: they return cursor coordinates $x and $y
        set xvec [$graph element cget $info(name) -xdata]
        set yvec [$graph element cget $info(name) -ydata]
        set nx [$xvec range $info(index) $info(index)]
        set ny [$yvec range $info(index) $info(index)]
        set font $fonts(bold)
    } else {
        set coords [$graph invtransform $x $y]
        set nx [lindex $coords 0]
        set ny [lindex $coords 1]
        set font $fonts(normal)
    }
    set label [format "(%g, %g)" $nx $ny]
    if {$y<50}  {set anchor "n"} else {set anchor "s"}
    if {$x<200} {append anchor "w"} else {append anchor "e"}
    $graph marker create text -coords [list $nx $ny] -name $markerName \
       -text $label -font $font -anchor $anchor -justify left -yoffset 0 -bg {}
}

proc bltGraph_Properties {{what ""}} {
    set w .bltwin
    set graph [$w.nb tab cget select -window].g
    bltGraph_PropertiesDialog $graph $what
}

#FIXME to some better place
set tmp(graphprops-last-open-tab) 0

proc bltGraph_PropertiesDialog {graph {tabtoopen ""}} {
    global tmp

    # fill dialog with data
    # this must be done before widget creations, otherwise combo doesn't obey the settings
    set tmp(graphtitle) [$graph cget -title]
    set tmp(titlefont) [$graph cget -font]

    set tmp(axisxlabel) [$graph axis cget x -title]
    set tmp(axisylabel) [$graph axis cget y -title]
    set tmp(axistitlefont) [$graph axis cget x -titlefont]
    set tmp(axistickfont) [$graph axis cget x -tickfont]
    set tmp(axisxrotate) [$graph axis cget x -rotate]
    set tmp(axisxdiv) [$graph axis cget x -subdivisions]
    set tmp(axisydiv) [$graph axis cget y -subdivisions]

    if {[winfo class $graph]=="Barchart"} {
        set tmp(barmode) [$graph cget -barmode]
        set tmp(barbaseline) [$graph cget -baseline]
    }

    set linenames {(all)}
    foreach e [$graph element names] {
        lappend linenames [$graph element cget $e -label]
    }
    set tmp(selectedline) "(all)"

    bltGraph_ReadPropertiesOfSelectedLine $graph

    set tmp(legendshow) [expr [$graph legend cget -hide]==0 ? "yes" : "no"]
    set tmp(legendpos) [$graph legend cget -position]
    set tmp(legendanchor) [$graph legend cget -anchor]
    set tmp(legendrelief) [$graph legend cget -relief]
    set tmp(legendfont) [$graph legend cget -font]

    # create the dialog
    set w .bltwin.graphprops
    createOkCancelDialog $w "Graph Properties"
    button $w.buttons.applybutton  -text {Apply} -width 10
    pack $w.buttons.applybutton  -anchor n -side right -padx 2
    wm geometry $w 360x300

    # tabnotebook
    set nb $w.f.nb
    blt::tabnotebook $nb -tearoff no -relief flat
    pack $nb -expand 1 -fill both
    foreach {tab title} {titles "Title" axes "Axes" lines "Lines" bars "Bars" legend "Legend"} {
        frame $nb.$tab
        set tabs($tab) [$nb insert end -text $title -window $nb.$tab  -fill both]
    }
    if {$tabtoopen==""} {
        $nb select $tmp(graphprops-last-open-tab)
    } else {
        $nb select $tabs($tabtoopen)
    }

    # Titles page
    set f $nb.titles
    label-entry $f.title "Graph title"
    label-entry $f.font "Font"
    $f.title.e config -textvariable tmp(graphtitle)
    $f.font.e configure -textvariable tmp(titlefont)
    pack $f.title $f.font -side top -anchor w -expand 0 -fill x

    # Axes page
    set f $nb.axes
    label-entry $f.xlabel "X axis title"
    label-entry $f.ylabel "Y axis title"
    label-entry $f.titlefont "Title font"
    label-entry $f.tickfont "Label font"
    label-combo $f.xrotate "Rotate X labels by" {0 30 45 60 90}
    label-combo $f.xdiv "X axis subdivisions" {1 2 4 5 10}
    label-combo $f.ydiv "Y axis subdivisions" {1 2 4 5 10}
    # FIXME axis min, max, logarithmic; x labels off (for bar charts); -invertxy
    # also: label-combo $f.display "Grid lines" {off {at major ticks} {at all ticks}}
    $f.xlabel.e configure -textvariable tmp(axisxlabel)
    $f.ylabel.e configure -textvariable tmp(axisylabel)
    $f.titlefont.e configure -textvariable tmp(axistitlefont)
    $f.tickfont.e configure -textvariable tmp(axistickfont)
    $f.xrotate.e configure -textvariable tmp(axisxrotate)
    $f.xdiv.e configure -textvariable tmp(xdiv)
    $f.ydiv.e configure -textvariable tmp(ydiv)
    pack $f.xlabel $f.ylabel $f.titlefont $f.tickfont $f.xrotate -side top -anchor w -expand 0 -fill x
    #FIXME "axis -subdivisions" doesn't seem to work in BLT
    #pack $f.xdiv $f.ydiv -side top -anchor w -expand 0 -fill x

    # Lines page -- all elements together
    # only if {[winfo class $graph]=="Graph"} ?
    set f $nb.lines
    label-combo $f.sel "Apply to lines:" $linenames
    $f.sel.e configure -textvariable tmp(selectedline)
    combo-onchange $f.sel.e [list bltGraph_ReadPropertiesOfSelectedLine $graph]

    labelframe $f.symf -relief groove -border 2 -text "Symbols"
    checkbutton $f.symf.on -text "Display symbols" -variable tmp(showsymbols)
    label-combo $f.symf.type "   Symbol type" {"(no change)" square circle diamond plus cross splus scross triangle}
    label-combo $f.symf.size "   Symbol size" {1 3 5 7 9 11}
    $f.symf.type.e configure -textvariable tmp(symboltype)
    $f.symf.size.e configure -textvariable tmp(symbolsize)
    pack $f.symf.on $f.symf.type $f.symf.size -side top -expand 0 -fill none -anchor w

    labelframe $f.linesf -relief groove -border 2 -text "Lines"
    radiobutton $f.linesf.none -text "none" -value "none"  -variable tmp(linetype)
    radiobutton $f.linesf.lin  -text "linear" -value "linear"  -variable tmp(linetype)
    radiobutton $f.linesf.step -text "step" -value "step"  -variable tmp(linetype)
    radiobutton $f.linesf.natu -text "spline" -value "catrom"  -variable tmp(linetype)
    pack $f.linesf.none $f.linesf.lin $f.linesf.step $f.linesf.natu -side top -expand 0 -fill none -anchor w

    pack $f.sel $f.symf $f.linesf -side top -expand 0 -fill x

    # Bars page -- all elements together
    # only if {[winfo class $graph]=="Barchart"} ?
    set f $nb.bars
    label-entry $f.barbaseline "Baseline"
    label-combo $f.barmode "Bar placement" {aligned overlap infront stacked}
    pack $f.barbaseline $f.barmode -side top -expand 0 -fill x
    $f.barbaseline.e configure -textvariable tmp(barbaseline)
    $f.barmode.e configure -textvariable tmp(barmode)

    # Legend page
    set f $nb.legend
    label-combo $f.show "Display legend" {yes no}
    label-combo $f.pos "Position" {plotarea leftmargin rightmargin topmargin bottommargin}
    label-combo $f.anchor "Anchor" {n s e w ne nw se sw}
    label-combo $f.relief "Relief" {flat solid raised sunken}
    label-entry $f.font "Font"
    $f.show.e configure -textvariable tmp(legendshow)
    $f.pos.e configure -textvariable tmp(legendpos)
    $f.anchor.e configure -textvariable tmp(legendanchor)
    $f.relief.e configure -textvariable tmp(legendrelief)
    $f.font.e configure -textvariable tmp(legendfont)
    pack $f.show $f.pos $f.anchor $f.relief $f.font -side top -anchor w -fill x

    # execute dialog
    $w.buttons.applybutton config -command [list bltGraph_PropertiesDialogApply $graph]
    if {[execOkCancelDialog $w] == 1} {
        bltGraph_PropertiesDialogApply $graph
    }
    set tmp(graphprops-last-open-tab) [$nb index select]
    destroy $w
}

proc bltGraph_ReadPropertiesOfSelectedLine {graph} {
    global tmp

    if {[$graph element names]=={}} {
        set tmp(showsymbols) "yes"
        set tmp(symboltype)  "(no change)"
        set tmp(symbolsize)  3
        set tmp(linetype)   "step"
    } else {
        set e [lindex [$graph element names] 0]
        foreach i [$graph element names] {
            if {$tmp(selectedline)=="(all)" || [$graph element cget $i -label]==$tmp(selectedline)} {
                set e $i
                break
            }
        }
        set tmp(showsymbols) [expr [$graph element cget $e -pixels]==0 ? 0 : 1]
        if {$tmp(selectedline)=="(all)"} {
            set tmp(symboltype) "(no change)"
        } else {
            set tmp(symboltype) [$graph element cget $e -symbol]
        }
        set tmp(symbolsize)  [$graph element cget $e -pixels]
        if {$tmp(symbolsize)==0} {set tmp(symbolsize) 3}
        set tmp(linetype)  [$graph element cget $e -smooth]
        if {$tmp(linetype)=="quadratic" || $tmp(linetype)=="cubic"} {
            set tmp(linetype) "catrom"
        }
        if {[$graph element cget $e -linewidth]==0} {set tmp(linetype) "none"}
    }
}

proc bltGraph_PropertiesDialogApply {graph} {
    global tmp

    # the catch{} statements below may make detecting errors more difficult
    # but spare us the validation
    catch {$graph config -title $tmp(graphtitle)}
    catch {$graph config -font $tmp(titlefont)}

    catch {$graph axis config x -title $tmp(axisxlabel)}
    catch {$graph axis config y -title $tmp(axisylabel)}
    catch {$graph axis config x -titlefont $tmp(axistitlefont)}
    catch {$graph axis config y -titlefont $tmp(axistitlefont)}
    catch {$graph axis config x -tickfont $tmp(axistickfont)}
    catch {$graph axis config y -tickfont $tmp(axistickfont)}
    catch {$graph axis config x -rotate $tmp(axisxrotate)}
    catch {$graph axis config x -subdivisions $tmp(axisxdiv)}
    catch {$graph axis config y -subdivisions $tmp(axisydiv)}

    if {[winfo class $graph]=="Barchart"} {
        catch {$graph configure -barmode $tmp(barmode)}
        catch {$graph configure -baseline $tmp(barbaseline)}
    } else {
        foreach i [$graph element names] {
            if {$tmp(selectedline)=="(all)" || [$graph element cget $i -label]==$tmp(selectedline)} {
                set pixels $tmp(symbolsize)
                if {$tmp(showsymbols)==0} {set pixels 0}
                catch {$graph element configure $i -pixels $pixels}
                if {$tmp(symboltype)!="(no change)"} {
                    catch {$graph element configure $i -symbol $tmp(symboltype)}
                }
                set linewidth 1
                set linesmooth $tmp(linetype)
                if {$tmp(linetype)=="none"} {
                    set linewidth 0
                    set linesmooth "linear"
                }
                catch {$graph element configure $i -linewidth $linewidth}
                catch {$graph element configure $i -smooth $linesmooth}
            }
        }
    }

    catch {$graph legend config -hide [expr $tmp(legendshow)=="no"]}
    catch {$graph legend config -position $tmp(legendpos)}
    catch {$graph legend config -anchor $tmp(legendanchor)}
    catch {$graph legend config -relief $tmp(legendrelief)}
    catch {$graph legend config -font $tmp(legendfont)}

    # update dialog with what we've done
    bltGraph_ReadPropertiesOfSelectedLine $graph
}


#
# Utility for creating charts and plots
#
proc getChartColor {i} {
    set graphcolors {
        red blue green cyan yellow black orange purple gray magenta
        turquoise lightgray violet wheat maroon tan darkgray
        red4 green4 blue4
    }
    set color [lindex $graphcolors [expr $i % [llength $graphcolors]]]
    return $color
}

#
# Utility for creating charts and plots
#
proc getChartSymbol {i} {
    set list {
        square circle diamond plus cross splus scross triangle
    }
    set sel [lindex $list [expr $i % [llength $list]]]
    return $sel
}

