#==========================================================================
#  BLTGRAPH.TCL -
#            part of OMNeT++
#==========================================================================

#----------------------------------------------------------------#
#  Copyright (C) 2004 Andras Varga
#
#  This file is distributed WITHOUT ANY WARRANTY. See the file
#  `license' for details on this and other legal matters.
#----------------------------------------------------------------#


set graphnumber 0

proc createBltGraph {graphtype {graphname ""}} {

    global help_tips icons
    global graphnumber

    if {$graphtype!="graph" && $graphtype!="barchart"} {error "invalid graphtype: must be graph of barchart"}

    # create container window if doesn't exist yet
    set w .bltwin
    if ![winfo exist $w] {
        toplevel $w
        wm deiconify $w
        wm title $w "Charts"
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
    blt::ClosestPoint $graph

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
    } else {
        if {[$graph element names]=={}} {
            set tmp(showsymbols) "yes"
            set tmp(symbolsize)  5
            set tmp(showlines)   "yes"
            set tmp(linesmooth)  "linear"
        } else {
            set e [lindex [$graph element names] 0]
            set tmp(showsymbols) [expr [$graph element cget $e -pixels]==0 ? "no" : "yes"]
            set tmp(symbolsize)  [$graph element cget $e -pixels]
            if {$tmp(symbolsize)==0} {set tmp(symbolsize) 5}
            set tmp(showlines)   [expr [$graph element cget $e -linewidth]==0 ? "no" : "yes"]
            set tmp(linesmooth)  [$graph element cget $e -smooth]
        }
    }
    set tmp(legendshow) [expr [$graph legend cget -hide]==0 ? "yes" : "no"]
    set tmp(legendpos) [$graph legend cget -position]
    set tmp(legendanchor) [$graph legend cget -anchor]
    set tmp(legendrelief) [$graph legend cget -relief]
    set tmp(legendfont) [$graph legend cget -font]

    # create the dialog
    set w .bltwin.graphprops
    createOkCancelDialog $w "Graph Properties"
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
    label-combo $f.showsym "Display symbols" {yes no}
    label-combo $f.symsize "Symbol size" {3 5 7 9 11}
    label-combo $f.showlin "Connect points" {yes no}
    label-combo $f.smooth "Line type" {linear step natural}
    pack $f.showsym $f.symsize $f.showlin $f.smooth -side top -expand 0 -fill x
    $f.showsym.e configure -textvariable tmp(showsymbols)
    $f.symsize.e configure -textvariable tmp(symbolsize)
    $f.showlin.e configure -textvariable tmp(showlines)
    $f.smooth.e configure -textvariable tmp(linesmooth)

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
    if {[execOkCancelDialog $w] == 1} {
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
                set pixels $tmp(symbolsize)
                if {$tmp(showsymbols)=="no"} {set pixels 0}
                catch {$graph element configure $i -pixels $pixels}
                set linewidth 1
                if {$tmp(showlines)=="no"} {set linewidth 0}
                catch {$graph element configure $i -linewidth $linewidth}
                catch {$graph element configure $i -smooth $tmp(linesmooth)}
            }
        }

        catch {$graph legend config -hide [expr $tmp(legendshow)=="no"]}
        catch {$graph legend config -position $tmp(legendpos)}
        catch {$graph legend config -anchor $tmp(legendanchor)}
        catch {$graph legend config -relief $tmp(legendrelief)}
        catch {$graph legend config -font $tmp(legendfont)}
    }
    set tmp(graphprops-last-open-tab) [$nb index select]
    destroy $w
}


