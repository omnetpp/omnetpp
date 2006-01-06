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
        wm geometry $w "660x540"

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
          {sep3    -separator}
          {savedef -image $icons(check)    -command bltGraph_saveAsDefaults1}
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
        set help_tips($toolbar.savedef) {Store settings as defaults}

        set help_tips($toolbar.close)   {Close}

        pack $toolbar -side top -fill x

        # tabnotebook
        blt::tabnotebook $w.nb -tearoff no -relief flat
        pack $w.nb -expand 1 -fill both

        bind $w <1>      {.popup unpost}
        bind $w <Escape> {bltGraph_CancelZoom; .popup unpost}
        bind $w.nb <3>   {%W select active; bltGraph_CancelZoom; .popup post %X %Y}
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
        # configure highlight color
        $graph pen configure "activeLine" -linewidth 1
        $graph grid configure -hide 0
        $graph grid configure -minor 1
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
    $graph legend configure -position plotarea
    $graph legend configure -anchor ne
    $graph legend configure -relief solid
    $graph legend configure -borderwidth 0
    $graph legend configure -font [$graph axis cget x -tickfont]
    pack $graph -expand 1 -fill both

    # Attention! 'blt::' commands somehow don't get loaded until at least
    # one 'Blt_' command was executed before them
    Blt_Crosshairs $graph
    blt::ZoomStack $graph ButtonPress-1 ButtonPress-2
    bltGraph_ShowCoordinates $graph
    bltGraph_ActiveLegend $graph

    bind $graph <3>  {bltGraph_CancelZoom; .popup post %X %Y}
    return $graph
}

proc createBltGraphPopup {} {
    menu .popup -tearoff 0
    foreach i {
      {command -label {Zoom out} -underline 0 -command bltGraph_ZoomOut}
      {separator}
      {command -label {Title...} -underline 0 -command {bltGraph_Properties titles} }
      {command -label {Axes...}  -underline 0 -command {bltGraph_Properties axes} }
      {command -label {Lines...} -underline 0 -command {bltGraph_Properties lines} }
      {command -label {Bars...}  -underline 0 -command {bltGraph_Properties bars} }
      {command -label {Legend...} -underline 1 -command {bltGraph_Properties legend} }
      {separator}
      {command -label {Copy to clipboard} -underline 0  -command bltGraph_Copy}
      {command -label {Save picture...} -underline 0    -command bltGraph_SavePicture}
      {command -label {Save Postscript...} -underline 2 -command bltGraph_SavePostscript}
      {separator}
      {command -label {Store settings as defaults} -underline 1 -command bltGraph_saveAsDefaults1}
      {separator}
      {command -label {Close} -underline 2 -command bltGraph_Close}
    } {
       eval .popup add $i
    }
}

proc bltGraph_Close {} {
    global zoomInfo

    # identify tab
    set w .bltwin
    set f [$w.nb tab cget select -window]

    # cancel zoom's marching ants, otherwise it causes runtime error
    set graph $f.g
    if { [info exists zoomInfo($graph,afterId)] } {
        after cancel $zoomInfo($graph,afterId)
    }

    # delete vectors used by the graph
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

proc bltGraph_ActiveLegend {graph} {
    # BLT's built-in ActiveLegend interferes with zoom window selection
    $graph legend bind all <ButtonPress-1> [list bltGraph_HighlightLegend]
}

proc bltGraph_HighlightLegend {} {
    global zoomInfo
    set w .bltwin
    set graph [$w.nb tab cget select -window].g

    # don't react if zoom window selection is in progress
    if { [info exists zoomInfo($graph,corner)] } {
        if {$zoomInfo($graph,corner) == "B"} {
            return
        }
    }

    # remove highlight from all lines
    set elem [$graph legend get current]
    set relief [$graph element cget $elem -labelrelief]

    foreach e [$graph element names] {
        if {[$graph element cget $e -labelrelief]!="flat"} {
            $graph element configure $e -labelrelief flat
            $graph element deactivate $e
        }
    }

    # then highlight this one if it wasn't highlighted before
    if { $relief == "flat" } {
        if {[winfo class $graph]!="Barchart"} {
            $graph pen configure "activeLine" -color "#ff1000" \
                   -symbol [$graph element cget $elem -symbol] \
                   -pixels [$graph element cget $elem -pixels]
        }
        $graph element configure $elem -labelrelief solid
        $graph element activate $elem

        # flash it a little
        foreach i {1 2 3} {
            update idletasks
            after 80
            $graph element deactivate $elem
            update idletasks
            after 80
            $graph element activate $elem
        }
    }

    # cancel zoom window selection which also started at click
    after idle bltGraph_CancelZoom
}

proc bltGraph_CancelZoom {} {
    global zoomInfo
    set w .bltwin
    set graph [$w.nb tab cget select -window].g
    if { [info exists zoomInfo($graph,corner)] } {
        if {$zoomInfo($graph,corner) == "B"} {
            blt::ResetZoom $graph
        }
    }
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
           $img write $fname -format "GIF"
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
    bind $graph <Leave>   {bltGraph_RemoveLabel %W}
}

proc bltGraph_ShowLabel { graph x y } {
    catch {
        global fonts
        set markerName "marker"
        catch { $graph marker delete $markerName }
        if [$graph element closest $x $y info] {
            set nx $info(x)
            set ny $info(y)
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
}

proc bltGraph_RemoveLabel { graph } {
    set markerName "marker"
    catch { $graph marker delete $markerName }
}

proc bltGraph_Properties {{what ""}} {
    set w .bltwin
    set graph [$w.nb tab cget select -window].g

    if {$what=="lines" && [winfo class $graph]=="Barchart"} {
        tk_messageBox -icon warning -type ok  -parent .bltwin -message "Cannot configure lines on a bar chart, sorry."
        return
    }
    if {$what=="bars" && [winfo class $graph]!="Barchart"} {
        tk_messageBox -icon warning -type ok  -parent .bltwin -message "Cannot configure bars on a graph, sorry."
        return
    }

    bltGraph_PropertiesDialog $graph $what
}

set tmp(graphprops-last-open-tab) 0

proc bltGraph_PropertiesDialog {graph {tabtoopen ""}} {
    global tmp

    # this must be done before widget creations, otherwise combo doesn't obey the settings
    set linenames {(all)}
    foreach e [$graph element names] {
        lappend linenames [$graph element cget $e -label]
    }
    set tmp(selectedline) "(all)"
    bltGraph_PropertiesDialogRead $graph

    # create the dialog
    set w .bltwin.graphprops
    createOkCancelDialog $w "Graph Properties"
    button $w.buttons.applybutton  -text {Apply} -width 10
    button $w.buttons.savedefbutton  -text {Set as defaults}
    pack $w.buttons.applybutton $w.buttons.savedefbutton -anchor n -side right -padx 2
    wm geometry $w 430x350

    # tabnotebook
    set nb $w.f.nb
    blt::tabnotebook $nb -tearoff no -relief flat
    pack $nb -expand 1 -fill both
    if {[winfo class $graph]=="Barchart"} {
        set pages {titles "Titles" axes "Axes" bars  "Bars"  legend "Legend"}
    } else {
        set pages {titles "Titles" axes "Axes" lines "Lines" legend "Legend"}
    }
    foreach {tab title} $pages {
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
    labelframe $f.t -relief groove -border 2 -text "Graph title"
    label-entry $f.t.title "Graph title"
    label-entry $f.t.font "Title font"
    labelframe $f.a -relief groove -border 2 -text "Axis titles"
    label-entry $f.a.xlabel "X axis title"
    label-entry $f.a.ylabel "Y axis title"
    label-entry $f.a.titlefont "Axis title font"
    label-entry $f.a.tickfont "Label font"
    label-combo $f.a.xrotate "Rotate X labels by" {0 30 45 60 90}
    $f.t.title.e config -textvariable tmp(graphtitle)
    $f.t.font.e configure -textvariable tmp(titlefont)
    $f.a.xlabel.e configure -textvariable tmp(axisxlabel)
    $f.a.ylabel.e configure -textvariable tmp(axisylabel)
    $f.a.titlefont.e configure -textvariable tmp(axistitlefont)
    $f.a.tickfont.e configure -textvariable tmp(axistickfont)
    $f.a.xrotate.e configure -textvariable tmp(axisxrotate)
    pack $f.t.title $f.t.font -side top -anchor w -expand 0 -fill x
    pack $f.a.xlabel $f.a.ylabel $f.a.titlefont $f.a.tickfont $f.a.xrotate -side top -anchor w -expand 0 -fill x
    pack $f.t $f.a -side top -anchor w -expand 0 -fill x

    # Axes page
    set f $nb.axes
    labelframe $f.b -relief groove -border 2 -text "Axis bounds"
    label $f.b.xlabel -text "X axis"
    label $f.b.ylabel -text "Y axis"
    label $f.b.minlabel -text "Min"
    label $f.b.maxlabel -text "Max"
    entry $f.b.xmin -textvariable tmp(axisxmin)
    entry $f.b.xmax -textvariable tmp(axisxmax)
    entry $f.b.ymin -textvariable tmp(axisymin)
    entry $f.b.ymax -textvariable tmp(axisymax)
    grid x           $f.b.minlabel  $f.b.maxlabel -sticky w -padx 3 -pady 3
    grid $f.b.xlabel $f.b.xmin      $f.b.xmax     -sticky w -padx 3 -pady 3
    grid $f.b.ylabel $f.b.ymin      $f.b.ymax     -sticky w -padx 3 -pady 3
    grid columnconfig $f.b 0 -weight 1
    grid columnconfig $f.b 1 -weight 2
    grid columnconfig $f.b 2 -weight 2
    labelframe $f.l -relief groove -border 2 -text "Axis options"
    checkbutton $f.l.xlog -text "logarithmic X axis" -variable tmp(axisxlog)
    checkbutton $f.l.ylog -text "logarithmic Y axis" -variable tmp(axisylog)
    checkbutton $f.l.invxy -text "invert X,Y" -variable tmp(invertxy)
    labelframe $f.g -relief groove -border 2 -text "Grid"
    radiobutton $f.g.gnone -text "none" -value "none" -variable tmp(grid)
    radiobutton $f.g.gmaj -text "at major ticks" -value "major" -variable tmp(grid)
    radiobutton $f.g.gmin -text "at all ticks" -value "minor" -variable tmp(grid)
    pack $f.l.xlog $f.l.ylog $f.l.invxy -side top -expand 0 -fill none -anchor w
    pack $f.g.gnone $f.g.gmaj $f.g.gmin -side top -expand 0 -fill none -anchor w
    # NOTE: "axis -subdivisions" doesn't seem to work in BLT
    #label-combo $f.xdiv "X axis subdivisions" {1 2 4 5 10}
    #label-combo $f.ydiv "Y axis subdivisions" {1 2 4 5 10}
    #$f.xdiv.e configure -textvariable tmp(xdiv)
    #f.ydiv.e configure -textvariable tmp(ydiv)
    grid $f.b - -sticky new
    grid $f.l $f.g -sticky new
    grid rowconfig $f 0 -weight 0
    grid rowconfig $f 1 -weight 1
    grid columnconfig $f 0 -weight 1
    grid columnconfig $f 1 -weight 1
    #pack $f.xdiv $f.ydiv -side top -anchor w -expand 0 -fill x

    if {[winfo class $graph]!="Barchart"} {
        # Lines page
        # TBD set color, separate page with checkboxes to turn individual lines on/off
        # only if {[winfo class $graph]=="Graph"} ?
        set f $nb.lines
        label-combo $f.sel "Apply to lines:" $linenames
        $f.sel.e configure -textvariable tmp(selectedline)
        combo-onchange $f.sel.e [list bltGraph_PropertiesDialogReadSelectedLine $graph]

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
        checkbutton $f.hide -text "Hide" -variable tmp(linehide)

        pack $f.sel -side top -expand 0 -fill x
        pack $f.symf $f.linesf -side top -expand 0 -fill x
        pack $f.hide -side top -expand 0 -fill none -anchor w
    }

    if {[winfo class $graph]=="Barchart"} {
        # Bars page -- all elements together
        # only if {[winfo class $graph]=="Barchart"} ?
        set f $nb.bars
        label-entry $f.barbaseline "Baseline"
        label-combo $f.barmode "Bar placement" {aligned overlap infront stacked}
        pack $f.barbaseline $f.barmode -side top -expand 0 -fill x
        $f.barbaseline.e configure -textvariable tmp(barbaseline)
        $f.barmode.e configure -textvariable tmp(barmode)
    }

    # Legend page
    set f $nb.legend
    checkbutton $f.show -text "Display legend" -variable tmp(legendshow)
    labelframe $f.app -relief groove -border 2 -text "Appearance"
    label-entry $f.app.font "Legend font"
    checkbutton $f.app.bd -text "Border" -variable tmp(legendborder)
    #label-combo $f.relief "Relief" {flat solid raised sunken}
    pack $f.app.bd -side top -expand 0 -fill none -anchor w
    pack $f.app.font -side top -anchor w -fill x
    #$f.relief.e configure -textvariable tmp(legendrelief)
    labelframe $f.pos -relief groove -border 2 -text "Position"
    radiobutton $f.pos.pi -text "inside the plot" -value "plotarea" -variable tmp(legendpos)
    radiobutton $f.pos.pa -text "above" -value "topmargin" -variable tmp(legendpos)
    radiobutton $f.pos.pb -text "below" -value "bottommargin" -variable tmp(legendpos)
    radiobutton $f.pos.pl -text "left" -value "leftmargin" -variable tmp(legendpos)
    radiobutton $f.pos.pr -text "right" -value "rightmargin" -variable tmp(legendpos)
    grid $f.pos.pi $f.pos.pa $f.pos.pl -sticky w
    grid x         $f.pos.pb $f.pos.pr -sticky w
    grid columnconfig $f.pos 0 -weight 0
    grid columnconfig $f.pos 1 -weight 0
    grid columnconfig $f.pos 2 -weight 1
    labelframe $f.anch -relief groove -border 2 -text "Anchoring"
    radiobutton $f.anch.n -text "north" -value "n" -variable tmp(legendanchor)
    radiobutton $f.anch.s -text "south" -value "s" -variable tmp(legendanchor)
    radiobutton $f.anch.e -text "east" -value "e" -variable tmp(legendanchor)
    radiobutton $f.anch.w -text "west" -value "w" -variable tmp(legendanchor)
    radiobutton $f.anch.ne -text "northeast" -value "ne" -variable tmp(legendanchor)
    radiobutton $f.anch.nw -text "northwest" -value "nw" -variable tmp(legendanchor)
    radiobutton $f.anch.se -text "southeast" -value "se" -variable tmp(legendanchor)
    radiobutton $f.anch.sw -text "southwest" -value "sw" -variable tmp(legendanchor)
    grid $f.anch.n  $f.anch.e $f.anch.ne $f.anch.se -sticky w
    grid $f.anch.s  $f.anch.w $f.anch.nw $f.anch.sw -sticky w
    grid columnconfig $f.anch 0 -weight 0
    grid columnconfig $f.anch 1 -weight 0
    grid columnconfig $f.anch 2 -weight 0
    grid columnconfig $f.anch 3 -weight 1
    pack $f.show -side top -expand 0 -fill none -anchor w
    pack $f.app -side top -anchor w -fill x
    pack $f.pos -side top -anchor w -fill x
    pack $f.anch -side top -anchor w -fill x
    $f.app.font.e configure -textvariable tmp(legendfont)

    # execute dialog
    $w.buttons.applybutton config -command [list bltGraph_PropertiesDialogApply $graph]
    $w.buttons.savedefbutton config -command [list bltGraph_PropertiesDialogSaveAsDefault $graph]
    if {[execOkCancelDialog $w] == 1} {
        bltGraph_PropertiesDialogApply $graph
    }
    set tmp(graphprops-last-open-tab) [$nb index select]
    destroy $w
}

proc bltGraph_PropertiesDialogRead {graph} {
    global tmp

    # fill dialog with data
    set tmp(graphtitle) [$graph cget -title]
    set tmp(titlefont) [$graph cget -font]

    set tmp(axisxlabel) [$graph axis cget x -title]
    set tmp(axisylabel) [$graph axis cget y -title]
    set tmp(axistitlefont) [$graph axis cget x -titlefont]
    set tmp(axistickfont) [$graph axis cget x -tickfont]
    set tmp(axisxrotate) [$graph axis cget x -rotate]
    set tmp(axisxdiv) [$graph axis cget x -subdivisions]
    set tmp(axisydiv) [$graph axis cget y -subdivisions]
    set tmp(axisxmin) [$graph axis cget x -min]
    set tmp(axisxmax) [$graph axis cget x -max]
    set tmp(axisymin) [$graph axis cget y -min]
    set tmp(axisymax) [$graph axis cget y -max]
    set tmp(axisxlog) [$graph axis cget x -logscale]
    set tmp(axisylog) [$graph axis cget y -logscale]
    set tmp(invertxy) [$graph cget -invertxy]

    if {[$graph grid cget -hide]} {
        set tmp(grid) "none"
    } elseif {[$graph grid cget -minor]} {
        set tmp(grid) "minor"
    } else {
        set tmp(grid) "major"
    }

    if {[winfo class $graph]!="Barchart"} {
        bltGraph_PropertiesDialogReadSelectedLine $graph
    }

    if {[winfo class $graph]=="Barchart"} {
        set tmp(barmode) [$graph cget -barmode]
        set tmp(barbaseline) [$graph cget -baseline]
    }

    set tmp(legendshow) [expr [$graph legend cget -hide]==0 ? 1 : 0]
    set tmp(legendpos) [$graph legend cget -position]
    set tmp(legendanchor) [$graph legend cget -anchor]
    set tmp(legendrelief) [$graph legend cget -relief]
    set tmp(legendborder) [$graph legend cget -border]
    set tmp(legendfont) [$graph legend cget -font]
}

proc bltGraph_PropertiesDialogReadSelectedLine {graph} {
    global tmp

    if {[$graph element names]=={}} {
        set tmp(showsymbols) "yes"
        set tmp(symboltype)  "(no change)"
        set tmp(symbolsize)  3
        set tmp(linetype)   "step"
        set tmp(linehide)   0
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
        set tmp(linehide)  [$graph element cget $e -hide]
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
    catch {$graph axis config x -min $tmp(axisxmin)}
    catch {$graph axis config x -max $tmp(axisxmax)}
    catch {$graph axis config y -min $tmp(axisymin)}
    catch {$graph axis config y -max $tmp(axisymax)}
    catch {$graph axis config x -logscale $tmp(axisxlog)}
    catch {$graph axis config y -logscale $tmp(axisylog)}
    catch {$graph config -invertxy $tmp(invertxy)}

    if {$tmp(grid)=="major"} {
        catch {$graph grid configure -hide 0}
        catch {$graph grid configure -minor 0}
    } elseif {$tmp(grid)=="minor"} {
        catch {$graph grid configure -hide 0}
        catch {$graph grid configure -minor 1}
    } else {
        catch {$graph grid configure -hide 1}
    }

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
                catch {$graph element configure $i -hide $tmp(linehide)}
            }
        }
    }

    catch {$graph legend config -hide [expr $tmp(legendshow)==0]}
    catch {$graph legend config -position $tmp(legendpos)}
    catch {$graph legend config -anchor $tmp(legendanchor)}
    catch {$graph legend config -border $tmp(legendborder)}
    catch {$graph legend config -relief $tmp(legendrelief)}
    catch {$graph legend config -font $tmp(legendfont)}

    # update dialog with what we've done
    bltGraph_PropertiesDialogRead $graph
}

proc bltGraph_PropertiesDialogSaveAsDefault {graph} {
    bltGraph_PropertiesDialogApply $graph
    bltGraph_saveAsDefaults $graph
}

#
# Utility for creating charts and plots
#
proc getChartColor {i} {
    set graphcolors {
        blue red2 green orange #008000
        darkgray #a00000 #008080 cyan #808000
        #8080ff yellow black purple gray
    }
    set color [lindex $graphcolors [expr $i % [llength $graphcolors]]]
    return $color
}

#
# Utility for creating charts and plots
#
proc getChartSymbol {i} {
    set list {
        square circle diamond plus cross triangle
    }
    set sel [lindex $list [expr $i % [llength $list]]]
    return $sel
}


#
# Apply saved defaults to the graph
#
proc bltGraph_applyDefaults {graph} {
    global config

    catch {$graph config -font $config(bltgraph-titlefont)}
    catch {$graph axis config x -titlefont $config(bltgraph-axistitlefont)}
    catch {$graph axis config y -titlefont $config(bltgraph-axistitlefont)}
    catch {$graph axis config x -tickfont $config(bltgraph-axistickfont)}
    catch {$graph axis config y -tickfont $config(bltgraph-axistickfont)}
    catch {$graph axis config x -rotate $config(bltgraph-axisxrotate)}
    catch {$graph axis config x -subdivisions $config(bltgraph-axisxdiv)}
    catch {$graph axis config y -subdivisions $config(bltgraph-axisydiv)}
    catch {$graph axis config x -logscale $config(bltgraph-axisxlog)}
    catch {$graph axis config y -logscale $config(bltgraph-axisylog)}
    catch {$graph config -invertxy $config(bltgraph-invertxy)}
    catch {$graph grid configure -hide $config(bltgraph-gridhide)}
    catch {$graph grid configure -minor $config(bltgraph-gridminor)}

    catch {$graph configure -barmode $config(bltgraph-barmode)}
    catch {$graph configure -baseline $config(bltgraph-barbaseline)}

    catch {$graph legend config -hide $config(bltgraph-legendshow)}
    catch {$graph legend config -position $config(bltgraph-legendpos)}
    catch {$graph legend config -anchor $config(bltgraph-legendanchor)}
    catch {$graph legend config -border $config(bltgraph-legendborder)}
    catch {$graph legend config -relief $config(bltgraph-legendrelief)}
    catch {$graph legend config -font $config(bltgraph-legendfont)}

    if {[winfo class $graph]!="Barchart"} {
        foreach i [$graph element names] {
            catch {$graph element configure $i -pixels $config(bltgraph-elempixels)}
            catch {$graph element configure $i -linewidth $config(bltgraph-elemlinewidth)}
            catch {$graph element configure $i -smooth $config(bltgraph-elemsmooth)}
        }
    }
}

#
# Save current graph settings as defaults
#
proc bltGraph_saveAsDefaults {graph} {
    global config

    set config(bltgraph-titlefont) [$graph cget -font]
    set config(bltgraph-axistitlefont) [$graph axis cget x -titlefont]
    set config(bltgraph-axistitlefont) [$graph axis cget y -titlefont]
    set config(bltgraph-axistickfont) [$graph axis cget x -tickfont]
    set config(bltgraph-axistickfont) [$graph axis cget y -tickfont]
    set config(bltgraph-axisxrotate) [$graph axis cget x -rotate]
    set config(bltgraph-axisxdiv) [$graph axis cget x -subdivisions]
    set config(bltgraph-axisydiv) [$graph axis cget y -subdivisions]
    set config(bltgraph-axisxlog) [$graph axis cget x -logscale]
    set config(bltgraph-axisylog) [$graph axis cget y -logscale]
    set config(bltgraph-invertxy) [$graph cget -invertxy]
    set config(bltgraph-gridhide) [$graph grid cget -hide]
    set config(bltgraph-gridminor) [$graph grid cget -minor]

    set config(bltgraph-barmode) [$graph cget -barmode]
    set config(bltgraph-barbaseline) [$graph cget -baseline]

    set config(bltgraph-legendshow) [$graph legend cget -hide]
    set config(bltgraph-legendpos) [$graph legend cget -position]
    set config(bltgraph-legendanchor) [$graph legend cget -anchor]
    set config(bltgraph-legendborder) [$graph legend cget -border]
    set config(bltgraph-legendrelief) [$graph legend cget -relief]
    set config(bltgraph-legendfont) [$graph legend cget -font]

    if {[winfo class $graph]!="Barchart"} {
        set e [lindex [$graph element names] 0]
        set config(bltgraph-elempixels) [$graph element cget $e -pixels]
        set config(bltgraph-elemsmooth) [$graph element cget $e -smooth]
        set config(bltgraph-elemlinewidth) [$graph element cget $e -linewidth]
    }
}


#
# Invike bltGraph_saveAsDefaults for currently visible graph
#
proc bltGraph_saveAsDefaults1 {} {
    # identify tab
    set w .bltwin
    set f [$w.nb tab cget select -window]
    set graph $f.g
    bltGraph_saveAsDefaults $graph
}
